#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_T_VERETX_H_
#define _CPPUTIL_BASIC_HEMESH_T_VERETX_H_

#include <CppUtil/Basic/HEMesh/ForwardDecl.h>

#include <vector>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E = EmptyEP_E<V>,
			typename P = std::conditional<std::is_same<E, EmptyEP_E<V>>::value, EmptyEP_P<V>, EmptyP<V,E>>::type>
		class TVertex {
		private:
			// internal use
			using HE = THalfEdge<V, E, P>;
			template<typename T>
			using ptr = HEMesh_ptr<T, HEMesh<V>>;
			template<typename T>
			using ptrc = ptr<const T>;

		public:
			// external use
			using Ptr = ptr<V>;
			using PtrC = ptrc<V>;

		private:
			// just for _enable_HEMesh
			friend class _enable_HEMesh<V, void, void, void>;
			using _E = E;
			using _P = P;

		public:
			const ptr<HE> HalfEdge() { return halfEdge; }
			const ptrc<HE> HalfEdge() const { return const_cast<TVertex*>(this)->HalfEdge(); }

			void SetHalfEdge(ptr<HE> he) { halfEdge = he; }

			bool IsIsolated() const { return halfEdge == nullptr; }

			const std::vector<ptr<HE>> OutHEs() { return IsIsolated() ? std::vector<ptr<HE>>() : HalfEdge()->RotateNextLoop(); }
			const std::vector<ptrc<HE>> OutHEs() const { return Const(const_cast<TVertex*>(this)->OutHEs()); }

			const std::vector<ptr<E>> AdjEdges();
			const std::vector<ptrc<HE>> AdjEdges() const { return Const(const_cast<TVertex*>(this)->AdjEdges()); }

			const ptr<HE> FindFreeIncident();
			const ptrc<HE> FindFreeIncident() const { return const_cast<TVertex*>(this)->FindFreeOutHE(); }

			static const ptr<HE> FindHalfEdge(ptr<V> v0, ptr<V> v1);

			const std::vector<ptr<V>> AdjVertices();
			const std::vector<ptrc<V>> AdjVertices() const { return Const<std::vector, V>(const_cast<TVertex*>(this)->AdjVertices()); }

			static const ptr<E> EdgeBetween(ptr<V> v0, ptr<V> v1);
			const ptr<E> EdgeWith(ptr<V> v) { return EdgeBetween(self, v); }

			static bool IsConnected(ptr<V> v0, ptr<V> v1) { return EdgeBetween(v0, v1) != nullptr; }
			bool IsConnectedWith(ptr<V> v) const { return IsConnected(self, v); }
			bool IsBoundary() const;

			size_t Degree() const { return OutHEs().size(); }

			void Clear() { halfEdge = nullptr; self = nullptr; }

		private:
			friend class HEMesh<V>; // for self
			ptr<V> self;

			ptr<HE> halfEdge;
		};
	}
}

#include <CppUtil/Basic/HEMesh/TVertex.inl>

#endif // !_CPPUTIL_BASIC_HEMESH_T_VERETX_H_
