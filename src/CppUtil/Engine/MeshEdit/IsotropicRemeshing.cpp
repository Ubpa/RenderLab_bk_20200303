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
using namespace Ubpa;

IsotropicRemeshing::IsotropicRemeshing(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>()) {
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

	if (!heMesh->IsTriMesh()) {
		printf("ERROR::IsotropicRemeshing::Init:\n"
			"\t""trimesh is not a triangle mesh\n");
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
	
	if (!heMesh->IsTriMesh()) {
		printf("ERROR::IsotropicRemeshing::Run\n"
			"\t""!heMesh->IsTriMesh(), algorithm error\n");
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
			unordered_set<E*> dEs(heMesh->Edges().begin(), heMesh->Edges().end()); // dynamic edges
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
			unordered_set<E*> dEs(heMesh->Edges().begin(), heMesh->Edges().end()); // dynamic edges
			while (dEs.size() > 0) {
				auto iter = dEs.begin();
				auto e = *iter;
				dEs.erase(iter);

				if (e->HalfEdge()->Origin()->IsBoundary() || e->HalfEdge()->End()->IsBoundary())
					continue;

				if (e->Length() < minL) {
					const auto c = e->Centroid();
					for (auto adjV : e->AdjVertices()) {
						if (Point3::Distance(adjV->pos, c) > maxL)
							continue;
					}
					auto eAdjEs = e->AdjEdges();

					auto v = heMesh->CollapseEdge(e, c);
					if (v != nullptr) {
						for (auto eAdjE : eAdjEs)
							dEs.erase(eAdjE);
						for (auto adjE : v->AdjEdges())
							dEs.insert(adjE);
					}
				}
			}
		}

		// 4. filp edges which can balance degree (6)
		printf("4. filp edges which can balance degree (6)\n");
		constexpr int innerD = 6;
		constexpr int boundD = 4;
		for (auto e : heMesh->Edges()) {
			if (e->IsBoundary())
				continue;
			auto he01 = e->HalfEdge();

			auto v0 = he01->Origin();
			auto v0d = static_cast<int>(v0->Degree());
			if (v0d <= 3)
				continue;
			auto v1 = he01->End();
			auto v1d = static_cast<int>(v1->Degree());
			if (v1d <= 3)
				continue;
			auto v2 = he01->Next()->End();
			auto v2d = static_cast<int>(v2->Degree());
			auto v3 = he01->Pair()->Next()->End();
			auto v3d = static_cast<int>(v3->Degree());

			double v0Cost = pow(v0d - (v0->IsBoundary() ? 4 : 6), 2);
			double v1Cost = pow(v1d - (v1->IsBoundary() ? 4 : 6), 2);
			double v2Cost = pow(v2d - (v2->IsBoundary() ? 4 : 6), 2);
			double v3Cost = pow(v3d - (v3->IsBoundary() ? 4 : 6), 2);
			double flipedV0Cost = pow(v0d - 1 - (v0->IsBoundary() ? 4 : 6), 2);
			double flipedV1Cost = pow(v1d - 1 - (v1->IsBoundary() ? 4 : 6), 2);
			double flipedV2Cost = pow(v2d + 1 - (v2->IsBoundary() ? 4 : 6), 2);
			double flipedV3Cost = pow(v3d + 1 - (v3->IsBoundary() ? 4 : 6), 2);

			double cost = v0Cost + v1Cost + v2Cost + v3Cost;
			double flipedCost = flipedV0Cost + flipedV1Cost + flipedV2Cost + flipedV3Cost;
			if (flipedCost < cost)
				heMesh->FlipEdge(e);
		}

		// 5. vertex normal
		printf("5. vertex normal\n");
		unordered_map<V*, Vec3> sWNs; // sum Weighted Normal
		unordered_map<HEMesh<V>::P*, float> triAreas; // triangle areas
		for (auto triangle : heMesh->Polygons()) {
			auto vertices = triangle->BoundaryVertice();
			assert(vertices.size() == 3);

			auto v0 = vertices[0];
			auto v1 = vertices[1];
			auto v2 = vertices[2];

			auto pos0 = v0->pos;
			auto pos1 = v1->pos;
			auto pos2 = v2->pos;

			auto d10 = pos0 - pos1;
			auto d12 = pos2 - pos1;
			auto wN = d12.Cross(d10);

			sWNs[v0] += wN;
			sWNs[v1] += wN;
			sWNs[v2] += wN;
			triAreas[triangle] = wN.Norm();
		}

		// 6. tangential smoothing
		printf("6. tangential smoothing\n");
		constexpr float w = 0.2f;// avoid oscillation
		for (auto v : heMesh->Vertices()) {
			if (v->IsBoundary()) {
				v->newPos = v->pos;
				continue;
			}

			// gravity-weighted offset
			Vec3 gravityCentroid;
			float sumArea = 0.f;
			for (auto outHE : v->OutHEs()) {
				auto p0 = outHE->Polygon();
				auto p1 = outHE->Pair()->Polygon();
				float area = 0.5f * (triAreas[p0] + triAreas[p1]);
				
				sumArea += area;
				gravityCentroid += area * outHE->End()->pos;
			}
			gravityCentroid /= sumArea;
			Vec3 offset = gravityCentroid - v->pos;

			// normal
			Vec3 normal = sWNs[v].Normalize();

			// tangent offset
			Vec3 tangentOffset = offset - offset.Dot(normal) * normal;
			auto newPos = v->pos + w * tangentOffset;

			// project back
			v->newPos = v->Project(newPos, normal);
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

const Vec3 IsotropicRemeshing::V::Project(const Vec3& p, const Normalf& norm) const {
	Ray ray(p, -norm);
	const auto adjVs = AdjVertices();
	for (size_t i = 0; i < adjVs.size(); i++) {
		size_t next = (i + 1) % adjVs.size();
		auto rst = ray.IntersectTriangle(pos, adjVs[i]->pos, adjVs[next]->pos);
		if (get<0>(rst)) // isIntersect
			return ray.At(get<2>(rst));
	}
	return p;
}
