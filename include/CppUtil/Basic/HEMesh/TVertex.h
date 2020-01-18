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
			const ptr<THalfEdge<V,E,P>> HalfEdge() { return halfEdge; }
			const ptrc<THalfEdge<V, E, P>> HalfEdge() const { return const_cast<TVertex*>(this)->HalfEdge(); }

			void SetHalfEdge(ptr<THalfEdge<V, E, P>> he) { halfEdge = he; }

			bool IsIsolated() const { return halfEdge.expired(); }

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

			void Clear() { halfEdge.reset(); self.reset(); }

		protected:
			virtual ~TVertex() = default;

		public:
			ptr<HE> halfEdge;

		private:
			friend class HEMesh<V, void, void, void>;
			ptr<V> self;
		};
	}
}

#include <CppUtil/Basic/HEMesh/TVertex.inl>

#endif // !_CPPUTIL_BASIC_HEMESH_T_VERETX_H_
