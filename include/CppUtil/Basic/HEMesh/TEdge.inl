template<typename V, typename E, typename P>
const std::vector<Ptr<THalfEdge<V, E, P>>> TEdge<V, E, P>::AdjOutHEs() {
	std::vector<Ptr<THalfEdge<V, E, P>>> hes;
	auto he01 = HalfEdge(); // v0 => v1
	for (auto he = he01->RotateNext(); he != he01; he = he->RotateNext())
		hes.push_back(he);
	for (auto he = he01->Next(); he != he01->Pair(); he = he->RotateNext())
		hes.push_back(he);
	return hes;
}

template<typename V, typename E, typename P>
const std::vector<Ptr<V>> TEdge<V, E, P>::AdjVertices() {
	std::vector<Ptr<V>> vertices;
	for (auto e : AdjOutHEs())
		vertices.push_back(e->End());
	return vertices;
}
