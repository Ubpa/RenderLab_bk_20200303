#include <CppUtil/Engine/MeshEdit/LoopSubdivision.h>

#include <CppUtil/Engine/TriMesh.h>
#include <CppUtil/Basic/Math.h>
#include <thread>

#include <omp.h>

#ifdef NDEBUG
#define THREAD_NUM omp_get_num_procs() - 1
#else
#define THREAD_NUM 1
//#define THREAD_NUM omp_get_num_procs() - 1
#endif //  NDEBUG

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace std;

LoopSubdivision::LoopSubdivision(Basic::Ptr<TriMesh> triMesh)
	: heMesh(HEMesh<V>::New()) { Init(triMesh); }

void LoopSubdivision::Clear() {
	triMesh = nullptr;
	heMesh->Clear();
}

bool LoopSubdivision::Init(Basic::Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::LoopSubdivision::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	heMesh->Init(triangles);

	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i];
	}

	if (!heMesh->IsTriMesh() || heMesh->HaveBoundary()) {
		printf("ERROR::LoopSubdivision::Init:\n"
			"\t""trimesh is not a triangle mesh or has boundaries\n");
		heMesh->Clear();
		return false;
	}

	this->triMesh = triMesh;
	return true;
}

bool LoopSubdivision::Run(size_t n){
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::LoopSubdivision::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	for (size_t i = 0; i < n; i++)
		HEMeshSubdivision();

	if (!heMesh->IsTriMesh() || heMesh->HaveBoundary()) {
		printf("ERROR::LoopSubdivision::Run\n"
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
	for (auto f : heMesh->Polygons()) {
		for (auto v : f->BoundaryVertice())
			indice.push_back(static_cast<uint>(heMesh->Index(v)));
	}
	
	triMesh->Init(indice, positions);

	return true;
}

void LoopSubdivision::HEMeshSubdivision() {
	// 1. update vertex pos
	{
		auto & vertices = heMesh->Vertices();
		auto work = [&vertices](size_t id) {
			for (size_t i = id; i < vertices.size(); i += THREAD_NUM) {
				auto v = vertices[i];
				auto adjVs = v->AdjVertices();
				size_t n = adjVs.size();
				float u = n == 3 ? 3.f / 16.f : 3.f / (8.f*n);
				Vec3 sumPos;
				for (auto adjV : adjVs)
					sumPos += adjV->pos;
				v->newPos = (1.f - n * u)*v->pos + u * sumPos;
				v->isNew = false;
			}
		};

		vector<thread> workers;
		for (int i = 0; i < THREAD_NUM; i++)
			workers.push_back(thread(work, i));

		// wait workers
		for (auto & worker : workers)
			worker.join();
	}

	// 2. compute pos of new vertice on edges
	{
		auto & edges = heMesh->Edges();
		auto work = [&edges](size_t id) {
			for (size_t i = id; i < edges.size(); i += THREAD_NUM) {
				auto e = edges[i];
				auto pos0 = e->HalfEdge()->Origin()->pos;
				auto pos1 = e->HalfEdge()->Pair()->Origin()->pos;
				auto pos2 = e->HalfEdge()->Next()->End()->pos;
				auto pos3 = e->HalfEdge()->Pair()->Next()->End()->pos;

				e->newPos = (3.f*(pos0 + pos1) + pos2 + pos3) / 8.f;
			}
		};

		vector<thread> workers;
		for (int i = 0; i < THREAD_NUM; i++)
			workers.push_back(thread(work, i));

		// wait workers
		for (auto & worker : workers)
			worker.join();
	}

	// 3. spilt edges
	auto edges = heMesh->Edges(); // must copy
	vector<Ptr<E>> newEdges;
	newEdges.reserve(2 * edges.size());
	for (auto e : edges) {
		auto v0 = e->HalfEdge()->Origin();
		auto v1 = e->HalfEdge()->End();

		auto v = heMesh->SpiltEdge(e); // e is deleted in HEMesh
		v->isNew = true;
		v->newPos = e->newPos;

		for (auto he : v->AdjOutHEs()) {
			if (he->End() != v0 && he->End() != v1)
				newEdges.push_back(he->Edge());
		}
	}

	// 4. flip new edge with old and new vertex
	for (auto e : newEdges) {
		if (e->HalfEdge()->Origin()->isNew + e->HalfEdge()->Pair()->Origin()->isNew != 1 )
			continue;

		heMesh->RotateEdge(e);
	}

	// 5. update vertex pos
	{
		for (auto v : heMesh->Vertices())
			v->pos = v->newPos;

		auto & vertices = heMesh->Vertices();
		auto work = [&vertices](size_t id) {
			for (size_t i = id; i < vertices.size(); i += THREAD_NUM) {
				auto v = vertices[i];
				v->pos = v->newPos;
			}
		};

		vector<thread> workers;
		for (int i = 0; i < THREAD_NUM; i++)
			workers.push_back(thread(work, i));

		// wait workers
		for (auto & worker : workers)
			worker.join();
	}
}
