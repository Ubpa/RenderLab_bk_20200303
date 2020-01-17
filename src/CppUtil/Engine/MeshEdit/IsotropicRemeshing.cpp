#include <CppUtil/Engine/MeshEdit/IsotropicRemeshing.h>

#include <CppUtil/Engine/TriMesh.h>
#include <CppUtil/Basic/Math.h>

#include <unordered_set>
#include <set>
#include <array>
#include <tuple>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace std;

IsotropicRemeshing::IsotropicRemeshing(Ptr<TriMesh> triMesh)
	: heMesh(HEMesh<V>::New()) {
	Init(triMesh);
}

void IsotropicRemeshing::Clear() {
	triMesh = nullptr;
	heMesh->Clear();
}

bool IsotropicRemeshing::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::IsotropicRemeshing::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	if (!heMesh->Init(triangles)) {
		printf("ERROR::IsotropicRemeshing::Init:\n"
			"\t""HEMesh init fail\n");
		return false;
	}

	if (!heMesh->IsTriMesh() || heMesh->HaveBoundary()) {
		printf("ERROR::IsotropicRemeshing::Init:\n"
			"\t""trimesh is not a triangle mesh or has boundaries\n");
		heMesh->Clear();
		return false;
	}

	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i];
	}

	this->triMesh = triMesh;
	return true;
}

bool IsotropicRemeshing::Run(size_t n) {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::IsotropicRemeshing::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	bool success = Kernel(n);
	if (!success) {
		printf("ERROR::IsotropicRemeshing::Run\n"
			"\t""run Kernel fail\n");
		return false;
	}
	
	if (!heMesh->IsTriMesh() || heMesh->HaveBoundary()) {
		printf("ERROR::IsotropicRemeshing::Run\n"
			"\t""!heMesh->IsTriMesh() || heMesh->HaveBoundary(), algorithm error\n");
		return false;
	}

	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();
	vector<Point3> positions;
	vector<uint> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);
	for (auto v : heMesh->Vertices())
		positions.push_back(v->pos);
	for (auto triangle : heMesh->Export()) {
		for (auto idx : triangle)
			indice.push_back(static_cast<uint>(idx));
	}

	triMesh->Init(indice, positions);

	return true;
}

bool IsotropicRemeshing::Kernel(size_t n) {
	// 1. mean of edges length
	printf("1. mean of edges length\n");
	float L = 0.f;
	for (auto e : heMesh->Edges())
		L += e->Length();

	L /= heMesh->NumEdges();
	float minL = 0.8f * L;
	float maxL = 4.f / 3.f * L;

	for (size_t i = 0; i < n; i++) {
		{// 2. spilt edges with length > maxL
			printf("2. spilt edges with length > maxL\n");
			unordered_set<Ptr<E>> dEs(heMesh->Edges().begin(), heMesh->Edges().end()); // dynamic edges
			while (dEs.size() > 0) {
				auto iter = dEs.begin();
				auto e = *iter;
				dEs.erase(iter);

				if (e->Length() > maxL) {
					auto v = heMesh->SpiltEdge(e, e->Centroid());

					for (auto adjE : v->AdjEdges())
						dEs.insert(adjE);
				}
			}
		}

		{// 3. collapse edges with length < minL
			printf("3. collapse edges with length < minL\n");
			unordered_set<Ptr<E>> dEs(heMesh->Edges().begin(), heMesh->Edges().end()); // dynamic edges
			while (dEs.size() > 0) {
				auto iter = dEs.begin();
				auto e = *iter;
				dEs.erase(iter);

				if (e->HalfEdge() == nullptr)
					continue;

				if (e->Length() < minL) {
					auto v = heMesh->CollapseEdge(e, e->Centroid());
					if (v != nullptr) {
						for (auto adjE : v->AdjEdges())
							dEs.insert(adjE);
					}
				}
			}
		}

		// 4. filp edges which can balance degree (6)
		printf("4. filp edges which can balance degree (6)\n");
		const int D = 6; // balance degree
		for (auto e : heMesh->Edges()) {
			auto he01 = e->HalfEdge();

			auto v0d = static_cast<int>(he01->Origin()->Degree());
			if (v0d == 3)
				continue;
			auto v1d = static_cast<int>(he01->End()->Degree());
			if (v1d == 3)
				continue;
			auto v2d = static_cast<int>(he01->Next()->End()->Degree());
			auto v3d = static_cast<int>(he01->Pair()->Next()->End()->Degree());

			auto cost = pow(v0d - D, 2) + pow(v1d - D, 2) + pow(v2d - D, 2) + pow(v3d - D, 2);
			auto flipCost = pow(v0d - 1 - D, 2) + pow(v1d - 1 - D, 2) + pow(v2d + 1 - D, 2) + pow(v3d + 1 - D, 2);
			if (flipCost < cost)
				heMesh->RotateEdge(e);
		}

		// 5. vertex normal
		printf("5. vertex normal\n");
		const size_t nV = heMesh->NumVertices();
		vector<Vec3> sWNs(nV); // sum weighted normals
		for (auto triangle : heMesh->Export()) {
			auto v0 = triangle[0];
			auto v1 = triangle[1];
			auto v2 = triangle[2];

			auto pos0 = heMesh->Vertices().at(v0)->pos;
			auto pos1 = heMesh->Vertices().at(v1)->pos;
			auto pos2 = heMesh->Vertices().at(v2)->pos;

			auto d10 = pos0 - pos1;
			auto d12 = pos2 - pos1;
			auto wN = d12.Cross(d10);

			sWNs[v0] += wN;
			sWNs[v1] += wN;
			sWNs[v2] += wN;
		}

		// 6. tangential smoothing
		printf("6. tangential smoothing\n");
		const float w = 0.2f;
		for (auto v : heMesh->Vertices()) {
			// offset
			Vec3 adjVCentroid;
			auto adjVs = v->AdjVertices();
			for (auto adjV : adjVs)
				adjVCentroid += adjV->pos;
			adjVCentroid /= static_cast<float>(adjVs.size());
			Vec3 offset = adjVCentroid - v->pos;

			// normal
			Vec3 normal = sWNs[heMesh->Index(v)].Normalize();

			// tangent offset
			Vec3 tangentOffset = offset - offset.Dot(normal) * normal;
			v->newPos = v->pos + w * tangentOffset;
		}

		// 7. update pos
		printf("7. update pos\n");
		for (auto v : heMesh->Vertices()) {
			if(!v->newPos.HasNaN())
				v->pos = v->newPos;
		}
	}

	return true;
}
