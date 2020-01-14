#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_T_EDGE_H_
#define _CPPUTIL_BASIC_HEMESH_T_EDGE_H_

#include <CppUtil/Basic/HeapObj.h>
#include <vector>
#include <set>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		class THalfEdge;

		template<typename V, typename E, typename P>
		class TEdge : public HeapObj {
		public:
			const Ptr<THalfEdge<V, E, P>> HalfEdge() { return halfEdge.lock(); }
			const PtrC<THalfEdge<V, E, P>> HalfEdge() const { return const_cast<TEdge*>(this)->HalfEdge(); }
			
			bool IsBoundary() const { return HalfEdge()->IsBoundary() || HalfEdge()->Pair()->IsBoundary(); }
			bool IsFree() const { return HalfEdge()->IsFree() && HalfEdge()->Pair()->IsFree(); }

			void SetHalfEdge(Ptr<THalfEdge<V, E, P>> he) { halfEdge = he; }

			// clockwise
			// + [he.RotateNext, he.RotateNext.RotateNext, ..., he)
			// + [he.next, he.next.RotateNext, ..., he.pair)
			const std::vector<Ptr<THalfEdge<V, E, P>>> AdjOutHEs();
			const std::set<Ptr<V>> AdjVertices();
			const std::vector<Ptr<E>> AdjEdges();
			void Clear() { halfEdge.reset(); }

		private:
			WPtr<THalfEdge<V, E, P>> halfEdge;
		};

#include <CppUtil/Basic/HEMesh/TEdge.inl>
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_T_EDGE_H_
