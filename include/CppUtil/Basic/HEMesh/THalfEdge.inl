template<typename V, typename E, typename P>
const Ptr<THalfEdge<V,E,P>> THalfEdge<V, E, P>::Pre() {
	Ptr<THalfEdge> he;
	for (he = This<THalfEdge>(); he->Next()->Next() != This<THalfEdge>(); he = he->Next())
		;// empty
	return he->Next();
}

template<typename V, typename E, typename P>
const Ptr<THalfEdge<V,E,P>> THalfEdge<V, E, P>::FindFreeIncident(Ptr<THalfEdge> begin, Ptr<THalfEdge> end) {
	assert(begin->End() == end->End());

	for (auto he = begin; he != end; he = he->Next()->Pair()) {
		if (he->IsFree())
			return he;
	}

	return nullptr;
}

template<typename V, typename E, typename P>
bool THalfEdge<V, E, P>::MakeAdjacent(Ptr<THalfEdge> inHE, Ptr<THalfEdge> outHE) {
	assert(inHE->End() == outHE->Origin());

	if (inHE->Next() == outHE)
		return true;

	auto freeIn = FindFreeIncident(outHE->Pair(), inHE);
	if (freeIn == nullptr)
		return false;

	auto inNext = inHE->Next();
	auto outPre = outHE->Pre();
	auto freeInNext = freeIn->Next();

	inHE->SetNext(outHE);
	freeIn->SetNext(inNext);
	outPre->SetNext(freeInNext);

	return true;
}

template<typename V, typename E, typename P>
const std::vector<Ptr<THalfEdge<V,E,P>>> THalfEdge<V, E, P>::NextBetween(Ptr<THalfEdge> begin, Ptr<THalfEdge> end) {
	std::vector<Ptr<THalfEdge<V, E, P>>> hes;
	auto he = begin;
	do {
		hes.push_back(he);
		he = he->Next();
	} while (he != end);
	return hes;
}

template<typename V, typename E, typename P>
const std::vector<Ptr<THalfEdge<V, E, P>>> THalfEdge<V, E, P>::RotateNextBetween(Ptr<THalfEdge> begin, Ptr<THalfEdge> end) {
	std::vector<Ptr<THalfEdge<V, E, P>>> hes;
	auto he = begin;
	do {
		hes.push_back(he);
		he = he->RotateNext();
	} while (he != end);
	return hes;
}

template<typename V, typename E, typename P>
const Ptr<THalfEdge<V, E, P>> THalfEdge<V, E, P>::NextAt(Ptr<V> origin) {
	auto he = This<THalfEdge>();
	while (he->Origin() != origin)
		he = he->Next();
	return he;
}

template<typename V, typename E, typename P>
void THalfEdge<V, E, P>::Clear() {
	next.reset();
	pair.reset();
	origin.reset();
	edge.reset();
	polygon.reset();
}
