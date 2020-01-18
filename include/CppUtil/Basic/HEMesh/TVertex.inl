#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_TVERTEX_INL_
#define _CPPUTIL_BASIC_HEMESH_TVERTEX_INL_

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		const std::vector<typename TVertex<V, E, P>::ptr<E>> TVertex<V, E, P>::AdjEdges() {
			std::vector<ptr<E>> edges;
			for (auto he : OutHEs())
				edges.push_back(he->Edge());
			return edges;
		}

		template<typename V, typename E, typename P>
		const typename TVertex<V, E, P>::ptr<THalfEdge<V, E, P>> TVertex<V, E, P>::FindFreeIncident() {
			for (auto outHE : OutHEs()) {
				auto inHE = outHE->Pair();
				if (inHE->IsFree())
					return inHE;
			}
			return nullptr;
		}

		template<typename V, typename E, typename P>
		const typename TVertex<V, E, P>::ptr<THalfEdge<V, E, P>> TVertex<V, E, P>::FindHalfEdge(ptr<V> v0, ptr<V> v1) {
			for (auto he : v0->OutHEs()) {
				if (he->End() == v1)
					return he;
			}
			return nullptr;
		}

		template<typename V, typename E, typename P>
		const std::vector<typename TVertex<V, E, P>::ptr<V>> TVertex<V, E, P>::AdjVertices() {
			std::vector<ptr<V>> adjVs;
			for (auto he : OutHEs())
				adjVs.push_back(he->End());
			return adjVs;
		}

		template<typename V, typename E, typename P>
		const typename TVertex<V, E, P>::ptr<E> TVertex<V, E, P>::EdgeBetween(ptr<V> v0, ptr<V> v1) {
			for (auto outHE : v0->OutHEs()) {
				if (outHE->End() == v1)
					return outHE->Edge();
			}

			return nullptr;
		}

		template<typename V, typename E, typename P>
		bool TVertex<V, E, P>::IsBoundary() const {
			for (auto he : OutHEs()) {
				if (he->IsBoundary())
					return true;
			}
			return false;
		}
	}
}

#endif// !_CPPUTIL_BASIC_HEMESH_TVERTEX_INL_
