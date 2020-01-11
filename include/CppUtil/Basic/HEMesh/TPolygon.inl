template<typename V, typename E, typename P>
const std::vector<Ptr<THalfEdge<V, E, P>>> TPolygon<V, E, P>::BoundaryHEs(Ptr<HE> begin, Ptr<HE> end) {
	std::vector<Ptr<HE>> hes;
	auto he = begin;
	do {
		hes.push_back(he);
		he = he->Next();
	} while (he != end);
	return hes;
}

template<typename V, typename E, typename P>
const std::vector<Ptr<E>> TPolygon<V, E, P>::BoundaryEdges() {
	std::vector<Ptr<E>> edges;
	for (auto he : BoundaryHEs())
		edges.push_back(he->Edge());
	return edges;
}

template<typename V, typename E, typename P>
const std::vector<Ptr<V>> TPolygon<V, E, P>::BoundaryVertice() {
	std::vector<Ptr<V>> vertice;
	for (auto he : BoundaryHEs())
		vertice.push_back(he->Origin());
	return vertice;
}

template<typename V, typename E, typename P>
const Ptr<THalfEdge<V,E,P>> TPolygon<V, E, P>::HalfEdge(Ptr<V> v) {
	auto he = halfedge;
	while (he->Origin() != v)
		he = he->Next();
	return he;
}