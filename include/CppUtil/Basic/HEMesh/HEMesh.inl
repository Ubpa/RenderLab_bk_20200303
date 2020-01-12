template<typename V, typename _0, typename _1, typename _2>
const Ptr<typename HEMesh<V, _0, _1, _2>::HE> HEMesh<V, _0, _1, _2>::NewHalfEdge() {
	auto he = Basic::New<HE>();
	halfEdges.insert(he);
	return he;
}

template<typename V, typename _0, typename _1, typename _2>
template<typename ...Args>
const Ptr<V> HEMesh<V, _0, _1, _2>::NewVertex(Args&& ... args) {
	auto v = Basic::New<V>(std::forward<Args>(args)...);
	vertices.insert(v);
	return v;
}

template<typename V, typename _0, typename _1, typename _2>
template<typename ...Args>
const Ptr<typename V::E_t> HEMesh<V, _0, _1, _2>::NewEdge(Args&& ... args) {
	auto e = Basic::New<E>(std::forward<Args>(args)...);
	edges.insert(e);
	return e;
}

template<typename V, typename _0, typename _1, typename _2>
template<typename ...Args>
const Ptr<typename V::P_t> HEMesh<V, _0, _1, _2>::NewPolygon(Args&& ... args) {
	auto p = Basic::New<P>(std::forward<Args>(args)...);
	polygons.insert(p);
	return p;
}

template<typename V, typename _0, typename _1, typename _2>
template<typename ...Args>
const Ptr<typename V::E_t> HEMesh<V, _0, _1, _2>::_AddEdge(Ptr<V> v0, Ptr<V> v1, Ptr<E> e, Args && ... args) {
	if (v0 == v1) {
		printf("ERROR::HEMesh::AddEdge\n"
			"\t""v0 == v1\n");
		return nullptr;
	}
	if (V::IsConnected(v0, v1)) {
		printf("ERROR::HEMesh::AddEdge\n"
			"\t""v0 is already connected with v1\n");
		return nullptr;
	}

	bool newEdge = false;
	if (!e) {
		e = NewEdge(std::forward<Args>(args)...);
		newEdge = true;
	}
	
	auto he0 = NewHalfEdge();
	auto he1 = NewHalfEdge();
	// [init]
	e->SetHalfEdge(he0);

	he0->SetNext(he1);
	he0->SetPair(he1);
	he0->SetOrigin(v0);
	he0->SetEdge(e);

	he1->SetNext(he0);
	he1->SetPair(he0);
	he1->SetOrigin(v1);
	he1->SetEdge(e);

	// [link he0]
	if (!v0->IsIsolated()) {
		auto inV0 = v0->FindFreeIncident();
		assert(inV0 != nullptr);
		auto outV0 = inV0->Next();

		inV0->SetNext(he0);
		he1->SetNext(outV0);
	}
	else
		v0->SetHalfEdge(he0);

	// [link he1]
	if (!v1->IsIsolated()) {
		auto inV1 = v1->FindFreeIncident();
		assert(inV1 != nullptr);
		auto outV1 = inV1->Next();

		inV1->SetNext(he1);
		he0->SetNext(outV1);
	}
	else
		v1->SetHalfEdge(he1);

	return e;
}

template<typename V, typename _0, typename _1, typename _2>
template<typename ...Args>
const Ptr<typename V::P_t> HEMesh<V, _0, _1, _2>::AddPolygon(const std::vector<Ptr<HE>> heLoop, Args && ... args) {
	if (heLoop.size() == 0) {
		printf("ERROR::HEMesh::AddPolygon:\n"
			"\t""heLoop is empty\n");
		return nullptr;
	}
	for (int i = 0; i < heLoop.size(); i++) {
		if (!heLoop[i]->IsFree()) {
			printf("ERROR::HEMesh::AddPolygon:\n"
				"\t""heLoop[%d] isn't free\n", i);
			return nullptr;
		}
		int next = (i + 1) % heLoop.size();
		if (heLoop[i]->End() != heLoop[next]->Origin()) {
			printf("ERROR::HEMesh::AddPolygon:\n"
				"\t""heLoop[%d]'s end isn't heLoop[%d]'s origin\n", i, next);
			return nullptr;
		}
	}

	// reorder link
	for (int i = 0; i < heLoop.size(); i++) {
		int next = (i + 1) % heLoop.size();
		if (!HE::MakeAdjacent(heLoop[i], heLoop[next])) {
			printf("ERROR::HEMesh::AddPolygon:\n"
				"\t""the polygon would introduce a non-monifold condition\n");
			return nullptr;
		}
	}

	// link polygon and heLoop
	auto polygon = NewPolygon(std::forward<Args>(args)...);

	polygon->SetHalfEdge(heLoop[0]);
	for (auto he : heLoop)
		he->SetPolygon(polygon);

	return polygon;
}

template<typename V, typename _0, typename _1, typename _2>
void HEMesh<V, _0, _1, _2>::RemovePolygon(Ptr<P> polygon) {
	assert(polygon != nullptr);
	for (auto he : polygon->BoundaryHEs())
		he->SetPolygon(nullptr);
	polygons.erase(polygon);
}

template<typename V, typename _0, typename _1, typename _2>
void HEMesh<V, _0, _1, _2>::RemoveEdge(Ptr<E> e, bool needErase) {
	assert(e != nullptr);
	auto he0 = e->HalfEdge();
	auto he1 = he0->Pair();

	if (!he0->IsFree())
		RemovePolygon(he0->Polygon());
	if (!he1->IsFree())
		RemovePolygon(he1->Polygon());

	// init
	auto v0 = he0->Origin();
	auto v1 = he1->Origin();
	auto inV0 = he0->Pre();
	auto inV1 = he1->Pre();
	auto outV0 = he0->RotateNext();
	auto outV1 = he1->RotateNext();

	// [link off he0]
	if (v0->HalfEdge() == he0)
		v0->SetHalfEdge(outV0 == he0 ? nullptr : outV0);

	inV0->SetNext(outV0);

	// [link off he1]
	if (v1->HalfEdge() == he1)
		v1->SetHalfEdge(outV1 == he1 ? nullptr : outV1);

	inV1->SetNext(outV1);

	// delete
	halfEdges.erase(he0);
	halfEdges.erase(he1);
	if(needErase)
		edges.erase(e);
}

template<typename V, typename _0, typename _1, typename _2>
void HEMesh<V, _0, _1, _2>::RemoveVertex(Ptr<V> v) {
	for (auto e : v->AdjEdges())
		RemoveEdge(e);
	vertices.erase(v);
}

template<typename V, typename _0, typename _1, typename _2>
const Ptr<V> HEMesh<V, _0, _1, _2>::SpiltEdge(Ptr<E> e) {
	auto he01 = e->HalfEdge();
	auto he10 = he01->Pair();

	if (he01->IsBoundary() && he10->IsBoundary()) {
		printf("ERROR::HEMesh::SpiltEdge:\n"
			"\t""two side of edge are boundaries\n");
		return nullptr;
	}

	if (he01->IsBoundary() || he10->IsBoundary()) {
		if (he01->IsBoundary())
			std::swap(he01, he10);

		// he0 isn't boundary, he1 is boundary

		if (he01->Polygon()->Degree() != 3) {
			printf("ERROR::HEMesh::SpiltEdge:\n"
				"\t""polygon's degree %zd is not 3\n", he01->Polygon()->Degree());
			return nullptr;
		}
		
		auto he12 = he01->Next();
		auto he20 = he12->Next();

		auto v0 = he01->Origin();
		auto v1 = he10->Origin();
		auto v2 = he12->End();

		RemoveEdge(e);
		
		auto v3 = AddVertex();

		auto e30 = AddEdge(v3, v0);
		auto e31 = AddEdge(v3, v1);
		auto e32 = AddEdge(v3, v2);

		auto he31 = e31->HalfEdge();
		auto he32 = e32->HalfEdge();
		auto he23 = he32->Pair();
		auto he03 = e30->HalfEdge()->Pair();

		AddPolygon({ he31, he12, he23 });
		AddPolygon({ he32, he20, he03 });

		return v3;
	}

	if (he01->Polygon()->Degree() != 3 || he10->Polygon()->Degree() != 3)
	{
		printf("ERROR::HEMesh::SpiltEdge:\n"
			"\t""polygon's degree (%zd, %zd) is not 3\n",
			he01->Polygon()->Degree(), he10->Polygon()->Degree());
		return nullptr;
	}

	auto he12 = he01->Next();
	auto he20 = he12->Next();
	auto he03 = he10->Next();
	auto he31 = he03->Next();

	auto v0 = he01->Origin();
	auto v1 = he12->Origin();
	auto v2 = he20->Origin();
	auto v3 = he31->Origin();

	RemoveEdge(e);

	auto v4 = AddVertex();

	auto e40 = AddEdge(v4, v0);
	auto e41 = AddEdge(v4, v1);
	auto e42 = AddEdge(v4, v2);
	auto e43 = AddEdge(v4, v3);

	auto he40 = e40->HalfEdge();
	auto he41 = e41->HalfEdge();
	auto he42 = e42->HalfEdge();
	auto he43 = e43->HalfEdge();

	auto he04 = he40->Pair();
	auto he14 = he41->Pair();
	auto he24 = he42->Pair();
	auto he34 = he43->Pair();

	AddPolygon({ he40,he03,he34 });
	AddPolygon({ he41,he12,he24 });
	AddPolygon({ he42,he20,he04 });
	AddPolygon({ he43,he31,he14 });

	return v4;
}

template<typename V, typename _0, typename _1, typename _2>
bool HEMesh<V, _0, _1, _2>::RotateEdge(Ptr<E> e) {
	auto he01 = e->HalfEdge();
	auto he10 = he01->Pair();

	auto poly0 = he01->Polygon();
	auto poly1 = he10->Polygon();
	
	if (poly0 == poly1) {
		printf("ERROR::HEMesh::SpiltEdge:\n"
			"\t""two side of edge are same\n");
		return false;
	}

	auto v0 = he01->Origin();
	auto v1 = he10->Origin();

	auto he02 = he10->Next();
	auto he13 = he01->Next();
	
	auto v2 = he02->End();
	auto v3 = he13->End();

	auto heLoop0 = he13->Next()->NextTo(he01);
	auto heLoop1 = he02->Next()->NextTo(he10);

	RemoveEdge(e, false); // don't erase
	_AddEdge(v2, v3, e); // use old edge

	auto he23 = e->HalfEdge();
	auto he32 = he23->Pair();
	
	heLoop0.push_back(he02);
	heLoop0.push_back(he23);
	heLoop1.push_back(he13);
	heLoop1.push_back(he32);

	AddPolygon(heLoop0);
	AddPolygon(heLoop1);

	return true;
}

template<typename V, typename _0, typename _1, typename _2>
bool HEMesh<V, _0, _1, _2>::Init(std::vector<std::vector<size_t>> polygons) {
	Clear();

	if (polygons.empty()) {
		printf("WARNNING::HEMesh::Init\n"
			"\t""polygons is empty\n");
		return false;
	}
	
	size_t max = 0;
	size_t min = SIZE_MAX;
	for (const auto& polygon : polygons) {
		if (polygon.size() <= 2) {
			printf("ERROR::HEMesh::Init\n"
				"\t""polygon's size <= 2\n");
			return false;
		}

		for (auto idx : polygon) {
			if (idx > max)
				max = idx;
			if (idx < min)
				min = idx;
		}
	}

	if (min != 0) {
		printf("ERROR::HEMesh::Init\n"
			"\t""min idx != 0\n");
		return false;
	}

	for (size_t i = 0; i <= max; i++)
		NewVertex();

	for (auto polygon : polygons) {
		vector<Ptr<HE>> heLoop;
		for (size_t i = 0; i < polygon.size(); i++) {
			size_t next = (i + 1) % polygon.size();
			auto u = vertices[polygon[i]];
			auto v = vertices[polygon[next]];
			auto he = V::FindHalfEdge(u, v);
			if (!he)
				he = AddEdge(u, v)->HalfEdge();
			heLoop.push_back(he);
		}
		AddPolygon(heLoop);
	}

	return true;
}

template<typename V, typename _0, typename _1, typename _2>
bool HEMesh<V, _0, _1, _2>::Init(std::vector<size_t> polygons, size_t sides) {
	if (polygons.size() % sides != 0) {
		printf("ERROR::HEMesh::Init:\n"
		"\t""polygons.size() isn't an integer multiple of sides\n")
	}
	std::vector<std::vector<size_t>> arrangedPolygons;
	for (size_t i = 0; i < polygons.size(); i += sides) {
		arrangedPolygons.emplace_back();
		for (size_t j = 0; j < sides; j++)
			arrangedPolygons.back().push_back(polygons[i + j]);
	}
	return Init(arrangedPolygons);
}

template<typename V, typename _0, typename _1, typename _2>
void HEMesh<V, _0, _1, _2>::Clear() {
	vertices.clear();
	halfEdges.clear();
	edges.clear();
	polygons.clear();
}

template<typename V, typename _0, typename _1, typename _2>
void HEMesh<V, _0, _1, _2>::Reserve(size_t n) {
	vertices.reserve(n);
	halfEdges.reserve(6*n);
	edges.reserve(3*n);
	polygons.reserve(2*n);
}

template<typename V, typename _0, typename _1, typename _2>
bool HEMesh<V, _0, _1, _2>::HaveBoundary() const {
	for (auto he : halfEdges){
		if (he->IsBoundary())
			return true;
	}
	return false;
}

template<typename V, typename _0, typename _1, typename _2>
bool HEMesh<V, _0, _1, _2>::IsTriMesh() const {
	for (auto poly : polygons) {
		if (poly->Degree() != 3)
			return false;
	}
	return true;
}

template<typename V, typename _0, typename _1, typename _2>
const std::vector<std::vector<Ptr<typename V::HE>>> HEMesh<V, _0, _1, _2>::Boundaries() {
	std::vector<std::vector<Ptr<HE>>> boundaries;
	std::set<Ptr<HE>> found;
	for (auto he : halfEdges) {
		if (he->IsBoundary() && found.find(he) == found.end()) {
			boundaries.push_back(std::vector<Ptr<HE>>());
			auto cur = he;
			do {
				boundaries.back().push_back(cur);
				found.insert(cur);
				cur = cur->Next();
			} while (cur != he);
		}
	}
	return boundaries;
}

template<typename V, typename _0, typename _1, typename _2>
const Ptr<typename V::P_t> HEMesh<V, _0, _1, _2>::EraseVertex(Ptr<V> v) {
	bool isBoundary = v->IsBoundary();
	auto pHE = v->HalfEdge()->Next();
	RemoveVertex(v);
	if (isBoundary)
		return nullptr;
	return AddPolygon(pHE->Loop());
}

template<typename V, typename _0, typename _1, typename _2>
template<typename ...Args>
const Ptr<V> HEMesh<V, _0, _1, _2>::AddPolygonVertex(Ptr<HE> he, Args && ... args) {
	auto p = he->Polygon();

	auto pre = he->Pre();
	auto v0 = he->Origin();
	auto v1 = AddVertex(std::forward<Args>(args)...);
	
	auto he01 = NewHalfEdge();
	auto he10 = NewHalfEdge();
	auto e01 = NewEdge();

	he01->SetNext(he10);
	he01->SetPair(he10);
	he01->SetOrigin(v0);
	he01->SetEdge(e01);
	he01->SetPolygon(p);

	he10->SetNext(he);
	he10->SetPair(he01);
	he10->SetOrigin(v1);
	he10->SetEdge(e01);
	he10->SetPolygon(p);

	e01->SetHalfEdge(he01);
	v1->SetHalfEdge(he10);

	pre->SetNext(he01);

	return v1;
}

template<typename V, typename _0, typename _1, typename _2>
template<typename ...Args>
const Ptr<V> HEMesh<V, _0, _1, _2>::AddPolygonVertex(Ptr<P> p, Ptr<V> v, Args&& ... args) {
	Ptr<HE> he;
	if (P::IsBoundary(p)) {
		for (auto outHE : v->AdjOutHEs()) {
			if (P::IsBoundary(outHE->Polygon())) {
				he = outHE;
				break;
			}
		}
		if (he == nullptr) {
			printf("ERROR::HEMesh::AddPolygonVertex:\n"
				"\t""p is boundary(nullptr) but v is not on boundary\n");
			return nullptr;
		}
	}
	else
		he = p->HalfEdge()->NextAt(v);
	
	return AddPolygonVertex(he, std::forward<Args>(args)...);
}

template<typename V, typename _0, typename _1, typename _2>
const Ptr<typename V::E_t> HEMesh<V, _0, _1, _2>::ConnectVertex(Ptr<HE> he0, Ptr<HE> he1) {
	auto p = he0->Polygon();
	if (p != he1->Polygon()) {
		printf("ERROR::HEMesh::ConnectVertex:\n"
			"\t""he0->Polygon != he1->Polygon\n");
		return nullptr;
	}

	auto v0 = he0->Origin();
	auto v1 = he1->Origin();
	if (v0 == v1) {
		printf("ERROR::HEMesh::ConnectVertex:\n"
			"\t""he0->Origin == he1->Origin\n");
		return nullptr;
	}

	if (V::IsConnected(v0, v1)) {
		printf("ERROR::HEMesh::ConnectVertex:\n"
			"\t""v0 and v1 is already connected\n");
		return nullptr;
	}

	if (!P::IsBoundary(p))
		RemovePolygon(p);

	auto he0Pre = he0->Pre();
	auto he1Pre = he1->Pre();

	auto he0Loop = he0->NextTo(he1);
	auto he1Loop = he1->NextTo(he0);
	
	auto e01 = NewEdge();
	auto he01 = NewHalfEdge();
	auto he10 = NewHalfEdge();

	e01->SetHalfEdge(he01);

	he01->SetNext(he1);
	he0Pre->SetNext(he01);
	he01->SetPair(he10);
	he01->SetOrigin(v0);
	he01->SetEdge(e01);
	
	he10->SetNext(he0);
	he1Pre->SetNext(he10);
	he10->SetPair(he01);
	he10->SetOrigin(v1);
	he10->SetEdge(e01);

	he0Loop.push_back(he10);
	he1Loop.push_back(he01);

	if (!P::IsBoundary(p)) {
		AddPolygon(he0Loop);
		AddPolygon(he1Loop);
	}

	return e01;
}

template<typename V, typename _0, typename _1, typename _2>
const Ptr<V> HEMesh<V, _0, _1, _2>::CollapseEdge(Ptr<E> e) {
	auto v0 = e->HalfEdge()->Origin();
	auto v1 = e->HalfEdge()->End();
	if (v0->Degree() == 1 && v1->Degree() == 1) {
		RemoveEdge(e);
		return AddVertex();
	}

	// on bounding polygon
	auto he = v0->Degree() > 1 ? e->HalfEdge()->RotateNext()->Pair()->Pre() : e->HalfEdge()->Next()->Pair()->Pre();

	vector<Ptr<HE>> hes; // origin is adjacent vertex of edge endpointsw
	for (auto outHE : e->AdjOutHEs()) { // AdjOutEnds isn't empty
		if (outHE->Next()->End() == v0 || outHE->Next()->End() == v1)
			continue;
		hes.push_back(outHE->Next());
	}

	RemoveVertex(v0);
	RemoveVertex(v1);

	auto p = AddPolygon(he->Loop());
	auto v = AddPolygonVertex(hes[0]);

	for (size_t i = 1; i < hes.size(); i++)
		ConnectVertex(v->HalfEdge(), hes[i]);

	return v;
}
