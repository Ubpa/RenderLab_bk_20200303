#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_T_POLYGON_H_
#define _CPPUTIL_BASIC_HEMESH_T_POLYGON_H_

#include <CppUtil/Basic/HEMesh/ForwardDecl.h>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		class TPolygon {
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
			const ptrc<HE> HalfEdge() const { return const_cast<TPolygon*>(this)->HalfEdge(); }

			void SetHalfEdge(ptr<HE> he) { halfEdge = he; }

			static bool IsBoundary(ptr<P> p) { return p == nullptr; }
			
			const std::vector<ptr<HE>> BoundaryHEs() { return HalfEdge()->NextLoop(); }
			size_t Degree() const { return static_cast<int>(const_cast<TPolygon*>(this)->BoundaryHEs().size()); }
			const std::vector<ptr<E>> BoundaryEdges();
			const std::vector<ptr<V>> BoundaryVertice();

			void Clear() { halfEdge.reset(); self.reset(); }

		public:
			ptr<HE> halfEdge;

		private:
			friend class HEMesh<V, void, void, void>;
			ptr<P> self;
		};
	}
}

#include <CppUtil/Basic/HEMesh/TPolygon.inl>

#endif // !_CPPUTIL_BASIC_HEMESH_T_POLYGON_H_
