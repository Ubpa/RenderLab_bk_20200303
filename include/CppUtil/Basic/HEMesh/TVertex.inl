#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_TVERTEX_INL_
#define _CPPUTIL_BASIC_HEMESH_TVERTEX_INL_

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		const std::vector<Ptr<THalfEdge<V, E, P>>> TVertex<V, E, P>::AdjOutHEs() {
			std::vector<Ptr<HE>> hes;
			if (IsIsolated())
				return hes;

			auto he = HalfEdge();
			do {
				hes.push_back(he);
				he = he->RotateNext();
			} while (he != HalfEdge());
			return hes;
		}

		template<typename V, typename E, typename P>
		const std::vector<Ptr<E>> TVertex<V, E, P>::AdjEdges() {
			std::vector<Ptr<E>> edges;
			for (auto he : AdjOutHEs())
				edges.push_back(he->Edge());
			return edges;
		}

		template<typename V, typename E, typename P>
		const Ptr<THalfEdge<V, E, P>> TVertex<V, E, P>::FindFreeIncident() {
			for (auto outHE : AdjOutHEs()) {
				auto inHE = outHE->Pair();
				if (inHE->IsFree())
					return inHE;
			}
			return nullptr;
		}

		template<typename V, typename E, typename P>
		const Ptr<THalfEdge<V, E, P>> TVertex<V, E, P>::FindHalfEdge(Ptr<V> v0, Ptr<V> v1) {
			for (auto he : v0->AdjOutHEs()) {
				if (he->End() == v1)
					return he;
			}
			return nullptr;
		}

		template<typename V, typename E, typename P>
		const std::vector<Ptr<V>> TVertex<V, E, P>::AdjVertices() {
			std::vector<Ptr<V>> adjVs;
			for (auto he : AdjOutHEs())
				adjVs.push_back(CastTo<V>(he->End()));
			return adjVs;
		}

		template<typename V, typename E, typename P>
		const Ptr<E> TVertex<V, E, P>::EdgeBetween(Ptr<V> v0, Ptr<V> v1) {
			for (auto outHE : v0->AdjOutHEs()) {
				if (outHE->End() == v1)
					return outHE->Edge();
			}

			return nullptr;
		}

		template<typename V, typename E, typename P>
		bool TVertex<V, E, P>::IsBoundary() const {
			for (auto he : AdjOutHEs()) {
				if (he->IsBoundary())
					return true;
			}
			return false;
		}
	}
}

#endif// !_CPPUTIL_BASIC_HEMESH_TVERTEX_INL_
