#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_
#define _CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_

#include <CppUtil/Basic/HEMesh/THalfEdge.h>
#include <CppUtil/Basic/HEMesh/TVertex.h>
#include <CppUtil/Basic/HEMesh/TEdge.h>
#include <CppUtil/Basic/HEMesh/TPolygon.h>

#include <CppUtil/Basic/random_set.h>

#include <assert.h>
#include <set>
#include <string>
#include <algorithm>
#include <iterator>

namespace CppUtil {
	namespace Basic {
		template <typename E, typename P>
		class EmptyV : public TVertex<EmptyV<E, P>, E, P> {};
		template <typename V, typename E>
		class EmptyP : public TPolygon<V, E, EmptyP<V, E>> {};
		template <typename V, typename P>
		class EmptyE : public TEdge<V, EmptyE<V, P>, P> {};

		template <typename V>
		class EmptyEP_E : public TEdge<V, EmptyEP_E<V>, EmptyEP_P<V>> {};
		template <typename V>
		class EmptyEP_P : public TPolygon<V, EmptyEP_E<V>, EmptyEP_P<V>> {};

		class AllEmpty : public TVertex<AllEmpty, EmptyEP_E<AllEmpty>, EmptyEP_P<AllEmpty>> {};

		// nullptr Polygon is a boundary
		template<typename V = AllEmpty,
			typename = std::enable_if_t<std::is_base_of_v<TVertex<V, typename V::E_t, typename V::P_t>, V>>,
			typename = std::enable_if_t<std::is_base_of_v<TEdge<V, typename V::E_t, typename V::P_t>, typename V::E_t>>,
			typename = std::enable_if_t<std::is_base_of_v<TPolygon<V, typename V::E_t, typename V::P_t>, typename V::P_t>>
		>
		class HEMesh : public HeapObj {
		private:
			using HE = typename V::HE;
			using E = typename V::E_t;
			using P = typename V::P_t;

		public:
			HEMesh() = default;
			HEMesh(const std::vector<std::vector<size_t>>& polygons) { Init(polygons); }
			HEMesh(const std::vector<size_t>& polygons, size_t sides) { Init(polygons, sides); }

		public:
			static const Ptr<HEMesh> New() { return Basic::New<HEMesh>(); }
			static const Ptr<HEMesh> New(const std::vector<std::vector<size_t>>& polygons) { return Basic::New<HEMesh>(polygons); }
			static const Ptr<HEMesh> New(const std::vector<size_t>& polygons, size_t sides) { return Basic::New<HEMesh>(polygons, sides); }

		public:
			const std::vector<Ptr<V>> & Vertices() { return vertices.vec(); }
			const std::vector<Ptr<HE>> & HalfEdges() { return halfEdges.vec(); }
			const std::vector<Ptr<E>> & Edges() { return edges.vec(); }
			const std::vector<Ptr<P>> & Polygons() { return polygons.vec(); }
			const std::vector<std::vector<Ptr<HE>>> Boundaries();

			size_t NumVertices() const { return vertices.size(); }
			size_t NumEdges() const { return edges.size(); }
			size_t NumPolygons() const { return polygons.size(); }
			size_t NumHalfEdges() const { return halfEdges.size(); }
			size_t NumBoundaries() const { return const_cast<HEMesh*>(this)->Boundaries().size(); }

			size_t Index(Ptr<V> v) const { return vertices.idx(v); }
			size_t Index(Ptr<E> e) const { return edges.idx(e); }
			size_t Index(Ptr<P> p) const { return polygons.idx(p); }
			const std::vector<size_t> Indices(Ptr<P> p) const;

			bool IsValid() const;
			bool IsTriMesh() const;
			// vertices empty => halfedges, edges and polygons empty
			bool IsEmpty() const { return vertices.empty(); }
			bool HaveIsolatedVertices() const;
			bool HaveBoundary() const;

			// min is 0
			bool Init(const std::vector<std::vector<size_t>> & polygons);
			bool Init(const std::vector<size_t> & polygons, size_t sides);
			void Clear();
			void Reserve(size_t n);
			const std::vector<std::vector<size_t>> Export() const;

			// -----------------
			//  basic mesh edit
			// -----------------

			template<typename ...Args>
			const Ptr<V> AddVertex(Args&& ... args) { return NewVertex(std::forward<Args>(args)...); }
			// e's halfedge is form v0 to v1
			template<typename ...Args>
			const Ptr<E> AddEdge(Ptr<V> v0, Ptr<V> v1, Args&& ... args);
			// polygon's halfedge is heLoop[0]
			template<typename ...Args>
			const Ptr<P> AddPolygon(const std::vector<Ptr<HE>> heLoop, Args&& ... args);
			void RemovePolygon(Ptr<P> polygon);
			void RemoveEdge(Ptr<E> e);
			void RemoveVertex(Ptr<V> v);

			// ----------------------
			//  high-level mesh edit
			// ----------------------

			// edge's halfedge : v0=>v1
			// nweV's halfedge : newV => v1
			template<typename ...Args>
			const Ptr<V> AddEdgeVertex(Ptr<E> e, Args&& ... args);

			// connect he0.origin and he1.origin in he0/he1.polygon
			// [require] he0.polygon == he1.polygon, he0.origin != he1.origin
			// [return] edge with halfedge form he0.origin to he1.origin
			template<typename ...Args>
			const Ptr<E> ConnectVertex(Ptr<HE> he0, Ptr<HE> he1, Args&& ... args);

			// delete e
			template<typename ...Args>
			const Ptr<V> SpiltEdge(Ptr<E> e, Args&& ... args);

			// counter-clock, remain e in container, won't break iteration
			bool RotateEdge(Ptr<E> e);

			// RemoveVertex and AddPolygon
			const Ptr<P> EraseVertex(Ptr<V> v);

			template<typename ...Args>
			const Ptr<V> CollapseEdge(Ptr<E> e, Args&& ... args);

		private:
			// new and insert
			const Ptr<HE> NewHalfEdge();
			template<typename ...Args>
			const Ptr<V> NewVertex(Args&& ... args);
			template<typename ...Args>
			const Ptr<E> NewEdge(Args&& ... args);
			template<typename ...Args>
			const Ptr<P> NewPolygon(Args&& ... args);

			// clear and erase
			void DeleteHalfEdge(Ptr<HE> he) { he->Clear(); halfEdges.erase(he); }
			void DeleteVertex(Ptr<V> v) { v->Clear(); vertices.erase(v); }
			void DeleteEdge(Ptr<E> e) { e->Clear(); edges.erase(e); }
			void DeletePolygon(Ptr<P> p) { p->Clear(); polygons.erase(p); }
		protected:
			virtual ~HEMesh() = default;

		private:
			random_set<Ptr<HE>> halfEdges;
			random_set<Ptr<V>> vertices;
			random_set<Ptr<E>> edges;
			random_set<Ptr<P>> polygons;
		};
	}
}

#include<CppUtil/Basic/HEMesh/HEMesh.inl>

#endif // !_CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_
