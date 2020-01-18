#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_T_HALFEDGE_H_
#define _CPPUTIL_BASIC_HEMESH_T_HALFEDGE_H_

#include <CppUtil/Basic/HEMesh/ForwardDecl.h>
#include <vector>
#include <assert.h>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		class THalfEdge {
		public:
			using V_t = V;
			using E_t = E;
			using P_t = P;
			using HE_t = THalfEdge;
			using HEMesh_t = HEMesh<V, void, void, void>;
			template<typename T>
			using ptr = HEMesh_ptr<T, HEMesh_t>;
			template<typename T>
			using ptrc = ptr<const T>;

		public:
			const ptr<THalfEdge> & Next() { return next; }
			const ptr<THalfEdge> Pair() { return pair; }
			const ptr<V> Origin() { return origin; }
			const ptr<E> Edge() { return edge; }
			const ptr<P> Polygon() { return polygon; }
			const ptr<V> End() { return pair->Origin(); }
			const ptr<THalfEdge> Pre();
			const ptr<THalfEdge> RotateNext() { return Pair()->Next(); }
			const ptr<THalfEdge> RotatePre() { return Pre()->Pair(); }

			const ptrc<THalfEdge> Next() const { return const_cast<THalfEdge*>(this)->Next(); }
			const ptrc<THalfEdge> Pair() const { return const_cast<THalfEdge*>(this)->Pair(); }
			const ptrc<V> Origin() const { return const_cast<THalfEdge*>(this)->Origin(); }
			const ptrc<E> Edge() const { return const_cast<THalfEdge*>(this)->Edge(); }
			const ptrc<P> Polygon() const { return const_cast<THalfEdge*>(this)->Polygon(); }
			const ptrc<V> End() const { return const_cast<THalfEdge*>(this)->End(); }
			const ptrc<THalfEdge> Pre() const { return const_cast<THalfEdge*>(this)->Pre(); }
			const ptrc<THalfEdge> RotateNext() const { return const_cast<THalfEdge*>(this)->RotateNext(); }
			const ptrc<THalfEdge> RotatePre() const { return const_cast<THalfEdge*>(this)->RotatePre(); }

			void SetNext(ptr<THalfEdge> he) { next = he; }
			void SetPair(ptr<THalfEdge> he) { pair = he; }
			void SetOrigin(ptr<V> v) { origin = v; }
			void SetEdge(ptr<E> e) { edge = e; }
			void SetPolygon(ptr<P> p) { polygon = p; }

			bool IsFree() const { return polygon.expired(); }
			bool IsBoundary() const { return polygon.expired(); }
			static const ptr<THalfEdge> FindFreeIncident(ptr<THalfEdge> begin, ptr<THalfEdge> end);
			static bool MakeAdjacent(ptr<THalfEdge> inHE, ptr<THalfEdge> outHE);

			// [begin, end), if begin == end, return a loop
			static const std::vector<ptr<THalfEdge>> NextBetween(ptr<THalfEdge> begin, ptr<THalfEdge> end);
			// [begin, end), if begin == end, return a loop
			static const std::vector<ptr<THalfEdge>> RotateNextBetween(ptr<THalfEdge> begin, ptr<THalfEdge> end);
			// [this, end), NextBetween(this, end);
			const std::vector<ptr<THalfEdge>> NextTo(ptr<THalfEdge> end) { return NextBetween(self, end); }
			// [this, end), RotateNextBetween(this, end);
			const std::vector<ptr<THalfEdge>> RotateNextTo(ptr<THalfEdge> end) { return RotateNextBetween(self, end); }
			// NextBetween(this, this), a loop from this to this
			const std::vector<ptr<THalfEdge>> NextLoop() { return NextBetween(self, self); }
			// RotateNextBetween(this, this), a loop from this to this
			const std::vector<ptr<THalfEdge>> RotateNextLoop() { return RotateNextBetween(self, self); }

			void Clear();

		public:
			ptr<THalfEdge> next;
			ptr<THalfEdge> pair;
			
			ptr<V> origin;
			ptr<E> edge;
			ptr<P> polygon;

		private:
			friend class HEMesh<V, void, void, void>;
			ptr<THalfEdge> self;
		};
	}
}

#include <CppUtil/Basic/HEMesh/THalfEdge.inl>

#endif // !_CPPUTIL_BASIC_HEMESH_T_HALFEDGE_H_
