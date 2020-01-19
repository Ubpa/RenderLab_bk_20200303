#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_THALFEDGE_INL_
#define _CPPUTIL_BASIC_HEMESH_THALFEDGE_INL_

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		const HEMesh_ptr<THalfEdge<V, E, P>, HEMesh<V>> THalfEdge<V, E, P>::Pre() {
			ptr<THalfEdge> he;
			for (he = Self(); he->Next()->Next() != Self(); he = he->Next())
				;// empty
			return he->Next();
		}

		template<typename V, typename E, typename P>
		const HEMesh_ptr<THalfEdge<V, E, P>, HEMesh<V>> THalfEdge<V, E, P>::FindFreeIncident(ptr<THalfEdge> begin, ptr<THalfEdge> end) {
			assert(begin->End() == end->End());

			for (auto he = begin; he != end; he = he->Next()->Pair()) {
				if (he->IsFree())
					return he;
			}

			return nullptr;
		}

		template<typename V, typename E, typename P>
		bool THalfEdge<V, E, P>::MakeAdjacent(ptr<THalfEdge> inHE, ptr<THalfEdge> outHE) {
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
		const std::vector<HEMesh_ptr<THalfEdge<V, E, P>, HEMesh<V>>> THalfEdge<V, E, P>::NextBetween(ptr<THalfEdge> begin, ptr<THalfEdge> end) {
			std::vector<ptr<THalfEdge<V, E, P>>> hes;
			auto he = begin;
			do {
				hes.push_back(he);
				he = he->Next();
			} while (he != end);
			return hes;
		}

		template<typename V, typename E, typename P>
		const std::vector<HEMesh_ptr<THalfEdge<V, E, P>, HEMesh<V>>> THalfEdge<V, E, P>::RotateNextBetween(ptr<THalfEdge> begin, ptr<THalfEdge> end) {
			std::vector<ptr<THalfEdge<V, E, P>>> hes;
			auto he = begin;
			do {
				hes.push_back(he);
				he = he->RotateNext();
			} while (he != end);
			return hes;
		}

		template<typename V, typename E, typename P>
		void THalfEdge<V, E, P>::Clear() {
			next = nullptr;
			pair = nullptr;
			origin = nullptr;
			edge = nullptr;
			polygon = nullptr;
		}
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_THALFEDGE_INL_
