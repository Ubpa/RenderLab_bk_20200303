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

namespace CppUtil {
	namespace Basic {
		// nullptr Polygon is a boundary
		template<typename V,
			typename = std::enable_if_t<std::is_base_of_v<TVertex<V, typename V::E_t, typename V::P_t>, V>>,
			typename = std::enable_if_t<std::is_base_of_v<TEdge<V, typename V::E_t, typename V::P_t>, typename V::E_t>>,
			typename = std::enable_if_t<std::is_base_of_v<TPolygon<V, typename V::E_t, typename V::P_t>, typename V::P_t>>
		>
		class HEMesh : public HeapObj {
		public:
			using HE = typename V::HE;
			using E = typename V::E_t;
			using P = typename V::P_t;

		public:
			static const Ptr<HEMesh> New() { return Basic::New<HEMesh>(); }

		public:
			const std::vector<Ptr<V>> & Vertices() { return vertices.vec(); }
			const std::vector<Ptr<HE>> & HalfEdges() { return halfEdges.vec(); }
			const std::vector<Ptr<E>> & Edges() { return edges.vec(); }
			const std::vector<Ptr<P>> & Polygons() { return polygons.vec(); }

			const std::vector<PtrC<V>> Vertices() const { return Const(const_cast<HEMesh*>(this)->Vertices()); }
			const std::vector<PtrC<HE>> HalfEdges() const { return Const(const_cast<HEMesh*>(this)->HalfEdges()); }
			const std::vector<PtrC<E>> Edges() const { return Const(const_cast<HEMesh*>(this)->Edges()); }
			const std::vector<PtrC<P>> Polygons() const { return Const(const_cast<HEMesh*>(this)->Polygons()); }

			size_t NumVertices() const { return vertices.size(); }
			size_t NumEdges() const { return halfEdges.size(); }
			size_t NumPolygons() const { return polygons.size(); }
			size_t NumHalfEdges() const { return halfEdges.size(); }

			size_t Index(Ptr<V> v) const { return vertices.idx(v); }
			size_t Index(Ptr<E> e) const { return edges.idx(e); }
			size_t Index(Ptr<P> p) const { return polygons.idx(p); }

			bool HaveBoundary() const;
			const std::vector<std::vector<Ptr<HE>>> Boundaries();
			size_t NumBoundaries() const { return const_cast<HEMesh*>(this)->Boundaries().size(); }
			bool IsTriMesh() const;

			void Clear();
			void Reserve(size_t n);

			// -----------------
			// [basic mesh edit]
			// -----------------

			template<typename ...Args>
			const Ptr<V> AddVertex(Args&& ... args);
			// e's halfedge is form v0 to v1
			template<typename ...Args>
			const Ptr<E> AddEdge(Ptr<V> v0, Ptr<V> v1, Args&& ... args) { return _AddEdge(v0, v1, nullptr, std::forward<Args>(args)...); }
			// polygon's halfedge is heLoop[0]
			template<typename ...Args>
			const Ptr<P> AddPolygon(const std::vector<Ptr<HE>> heLoop, Args&& ... args);
			void RemovePolygon(Ptr<P> polygon);
			void RemoveEdge(Ptr<E> e) { RemoveEdge(e, true); }
			void RemoveVertex(Ptr<V> v);

			// --------------------
			// high-level mesh edit
			// --------------------

			// e remains in container
			// add 3 new edge
			// new e's halfedge is from new v to e->halfedge->end
			Ptr<V> SpiltEdge(Ptr<E> e);

			// counter-clock, remain e in container
			bool RotateEdge(Ptr<E> e);

			const Ptr<P> EraseVertex(Ptr<V> v);

			const Ptr<V> CollapseEdge(Ptr<E> e);


			// add a vertex in he.polygon
			// he.origin => new vertex => he.origin
			template<typename ...Args>
			const Ptr<V> AddPolygonVertex(Ptr<HE> he, Args&& ... args);
			// add a vertex in polygon
			// v => new vertex => v
			// he[new vertex => v].next is the first halfedge whose origin is v from p.he
			// p.he, p.he.next, ..., **he[v => ?]**, ...
			template<typename ...Args>
			const Ptr<V> AddPolygonVertex(Ptr<P> p, Ptr<V> v, Args&& ... args);

			// connect he0.origin and he1.origin in he0/he1.polygon
			// [require] he0.polygon == he1.polygon, he0.origin != he1.origin
			// [return] edge with halfedge form he0.origin to he1.origin
			const Ptr<E> ConnectVertex(Ptr<HE> he0, Ptr<HE> he1);
			// connect v0 and v1 in p
			// call ConnectVertex(p->HalfEdge(v0), p->HalfEdge(v1))
			const Ptr<E> ConnectVertex(Ptr<P> p, Ptr<V> v0, Ptr<V> v1) { return ConnectVertex(p->HalfEdge(v0), p->HalfEdge(v1)); }

		private:
			template<typename ...Args>
			const Ptr<E> _AddEdge(Ptr<V> v0, Ptr<V> v1, Ptr<E> e, Args&& ... args);
			void RemoveEdge(Ptr<E> e, bool needErase);

		protected:
			virtual ~HEMesh() = default;

		private:
			random_set<Ptr<V>> vertices;
			random_set<Ptr<HE>> halfEdges;
			random_set<Ptr<E>> edges;
			random_set<Ptr<P>> polygons;
		};

#include<CppUtil/Basic/HEMesh/HEMesh.inl>
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_
