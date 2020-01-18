#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_T_EDGE_H_
#define _CPPUTIL_BASIC_HEMESH_T_EDGE_H_

#include <CppUtil/Basic/HEMesh/ForwardDecl.h>
#include <vector>
#include <set>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P = EmptyP<V, E>>
		class TEdge {
		private:
			using HE = THalfEdge<V, E, P>;

		public:
			using V_t = V;
			using E_t = E;
			using P_t = P;
			using HE_t = HE;
			using HEMesh_t = HEMesh<V, void, void, void>;
			template<typename T>
			using ptr = HEMesh_ptr<T, HEMesh_t>;
			template<typename T>
			using ptrc = ptr<const T>;

		public:
			const ptr<HE> HalfEdge() { return halfEdge; }
			const ptrc<HE> HalfEdge() const { return const_cast<TEdge*>(this)->HalfEdge(); }

			bool IsBoundary() const { return HalfEdge()->IsBoundary() || HalfEdge()->Pair()->IsBoundary(); }
			bool IsFree() const { return HalfEdge()->IsFree() && HalfEdge()->Pair()->IsFree(); }

			void SetHalfEdge(ptr<HE> he) { halfEdge = he; }

			// clockwise
			// + [he.RotateNext, he.RotateNext.RotateNext, ..., he)
			// + [he.next, he.next.RotateNext, ..., he.pair)
			const std::vector<ptr<HE>> OutHEs();
			const std::set<ptr<V>> AdjVertices();
			const std::vector<ptr<E>> AdjEdges();
			void Clear() { halfEdge.reset(); self.reset(); }

		public:
			ptr<HE> halfEdge;

		private:
			friend class HEMesh<V, void, void, void>;
			ptr<E> self;
		};
	}
}

#include <CppUtil/Basic/HEMesh/TEdge.inl>

#endif // !_CPPUTIL_BASIC_HEMESH_T_EDGE_H_
