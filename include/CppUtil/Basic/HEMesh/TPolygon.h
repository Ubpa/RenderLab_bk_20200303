#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_T_POLYGON_H_
#define _CPPUTIL_BASIC_HEMESH_T_POLYGON_H_

#include <CppUtil/Basic/HEMesh/ForwardDecl.h>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		class TPolygon {
		private:
			// internal use
			using HE = THalfEdge<V, E, P>;
			template<typename T>
			using ptr = HEMesh_ptr<T, HEMesh<V>>;
			template<typename T>
			using ptrc = ptr<const T>;

		public:
			// external use
			using Ptr = ptr<P>;
			using PtrC = ptrc<P>;

		public:
			const ptr<HE> HalfEdge() { return halfEdge; }
			const ptrc<HE> HalfEdge() const { return const_cast<TPolygon*>(this)->HalfEdge(); }

			void SetHalfEdge(ptr<HE> he) { halfEdge = he; }

			static bool IsBoundary(ptr<P> p) { return p == nullptr; }
			size_t Degree() const { return static_cast<int>(const_cast<TPolygon*>(this)->BoundaryHEs().size()); }

			const std::vector<ptr<HE>> BoundaryHEs() { return HalfEdge()->NextLoop(); }
			const std::vector<ptr<E>> BoundaryEdges();
			const std::vector<ptr<V>> BoundaryVertice();

			void Clear() { halfEdge = nullptr; }

		private:
			ptr<HE> halfEdge;
		};
	}
}

#include <CppUtil/Basic/HEMesh/TPolygon.inl>

#endif // !_CPPUTIL_BASIC_HEMESH_T_POLYGON_H_
