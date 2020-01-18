#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_
#define _CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_

#include <CppUtil/Basic/HEMesh/THalfEdge.h>
#include <CppUtil/Basic/HEMesh/TVertex.h>
#include <CppUtil/Basic/HEMesh/TEdge.h>
#include <CppUtil/Basic/HEMesh/TPolygon.h>

#include <CppUtil/Basic/random_set.h>
#include <CppUtil/Basic/vec_pool.h>
#include <CppUtil/Basic/HeapObj.h>

#include <set>
#include <string>
#include <algorithm>
#include <iterator>
#include <unordered_map>

#include <assert.h>

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
		public:
			using HE = typename V::HE_t;
			using E = typename V::E_t;
			using P = typename V::P_t;
			template<typename T>
			using ptr = HEMesh_ptr<T, HEMesh>;
			template<typename T>
			using ptrc = ptr<const T>;

		private:
			template<typename T>
			using ID = HEMesh_ID<T>;

		public:
			HEMesh() = default;
			HEMesh(const std::vector<std::vector<size_t>>& polygons) { Init(polygons); }
			HEMesh(const std::vector<size_t>& polygons, size_t sides) { Init(polygons, sides); }

		public:
			static const Ptr<HEMesh> New() { return Basic::New<HEMesh>(); }
			static const Ptr<HEMesh> New(const std::vector<std::vector<size_t>>& polygons) { return Basic::New<HEMesh>(polygons); }
			static const Ptr<HEMesh> New(const std::vector<size_t>& polygons, size_t sides) { return Basic::New<HEMesh>(polygons, sides); }

		public:
			const std::vector<ptr<V>> & Vertices() { return vertices.vec(); }
			const std::vector<ptr<HE>> & HalfEdges() { return halfEdges.vec(); }
			const std::vector<ptr<E>> & Edges() { return edges.vec(); }
			const std::vector<ptr<P>> & Polygons() { return polygons.vec(); }
			const std::vector<std::vector<ptr<HE>>> Boundaries();

			size_t NumVertices() const { return vertices.size(); }
			size_t NumEdges() const { return edges.size(); }
			size_t NumPolygons() const { return polygons.size(); }
			size_t NumHalfEdges() const { return halfEdges.size(); }
			size_t NumBoundaries() const { return const_cast<HEMesh*>(this)->Boundaries().size(); }

			size_t Index(ptr<V> v) const { return vertices.idx(v); }
			size_t Index(ptr<E> e) const { return edges.idx(e); }
			size_t Index(ptr<P> p) const { return polygons.idx(p); }
			const std::vector<size_t> Indices(ptr<P> p) const;

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
			const ptr<V> AddVertex(Args&& ... args) { return New<V>(std::forward<Args>(args)...); }
			// e's halfedge is form v0 to v1
			template<typename ...Args>
			const ptr<E> AddEdge(ptr<V> v0, ptr<V> v1, Args&& ... args);
			// polygon's halfedge is heLoop[0]
			template<typename ...Args>
			const ptr<P> AddPolygon(const std::vector<ptr<HE>> heLoop, Args&& ... args);
			void RemovePolygon(ptr<P> polygon);
			void RemoveEdge(ptr<E> e);
			void RemoveVertex(ptr<V> v);

			// ----------------------
			//  high-level mesh edit
			// ----------------------

			// edge's halfedge : v0=>v1
			// nweV's halfedge : newV => v1
			template<typename ...Args>
			const ptr<V> AddEdgeVertex(ptr<E> e, Args&& ... args);

			// connect he0.origin and he1.origin in he0/he1.polygon
			// [require] he0.polygon == he1.polygon, he0.origin != he1.origin
			// [return] edge with halfedge form he0.origin to he1.origin
			template<typename ...Args>
			const ptr<E> ConnectVertex(ptr<HE> he0, ptr<HE> he1, Args&& ... args);

			// delete e
			template<typename ...Args>
			const ptr<V> SpiltEdge(ptr<E> e, Args&& ... args);

			// counter-clock, remain e in container, won't break iteration
			bool RotateEdge(ptr<E> e);

			// RemoveVertex and AddPolygon
			const ptr<P> EraseVertex(ptr<V> v);

			template<typename ...Args>
			const ptr<V> CollapseEdge(ptr<E> e, Args&& ... args);

		private:
			// new and insert
			template<typename T, typename ... Args>
			const ptr<T> New(Args&& ... args) {
				auto idx = traits<T>::pool(this).request(std::forward<Args>(args)...);
				auto& elem = traits<T>::pool(this).at(idx);
				ID<T> id(traits<T>::nextID(this)++);
				traits<T>::table(this)[id] = idx;
				elem.self = ptr<T>(This<HEMesh>(), id);
				traits<T>::set(this).insert(elem.self);
				return elem.self;
			}

			// clear and erase
			template<typename T>
			void Delete(ptr<T> elem) {
				elem->Clear();
				auto target = traits<T>::table(this).find(elem.ID);
				size_t idx = target->second;
				traits<T>::table(this).erase(target);
				traits<T>::pool(this).recycle(idx);
				traits<T>::set(this).erase(elem);
			}

			template<typename T, typename HEMesh_t>
			friend class HEMesh_ptr;
			template<typename T>
			T* const Get(ID<T> ID) {
				auto target = traits<T>::table(this).find(ID);
				if (target == traits<T>::table(this).end())
					return nullptr;
				else
					return &traits<T>::pool(this)[target->second];
			}
		protected:
			virtual ~HEMesh() = default;

		private:
			random_set<ptr<HE>> halfEdges;
			random_set<ptr<V>> vertices;
			random_set<ptr<E>> edges;
			random_set<ptr<P>> polygons;

			vec_pool<HE> poolHE;
			vec_pool<V> poolV;
			vec_pool<E> poolE;
			vec_pool<P> poolP;

			std::unordered_map<ID<HE>, size_t> tableHE;
			std::unordered_map<ID<V>, size_t> tableV;
			std::unordered_map<ID<E>, size_t> tableE;
			std::unordered_map<ID<P>, size_t> tableP;

			int nextID_HE = 0;
			int nextID_V = 0;
			int nextID_E = 0;
			int nextID_P = 0;

			template<typename T>
			struct traits;
			template<typename T>
			friend struct traits;
			template<>
			struct traits<HE> {
				static auto& pool(HEMesh* mesh) { return mesh->poolHE; }
				static auto& table(HEMesh* mesh) { return mesh->tableHE; }
				static auto& set(HEMesh* mesh) { return mesh->halfEdges; }
				static auto& nextID(HEMesh* mesh) { return mesh->nextID_HE; }
			};
			template<>
			struct traits<V> {
				static auto& pool(HEMesh* mesh) { return mesh->poolV; }
				static auto& table(HEMesh* mesh) { return mesh->tableV; }
				static auto& set(HEMesh* mesh) { return mesh->vertices; }
				static auto& nextID(HEMesh* mesh) { return mesh->nextID_V; }
			};
			template<>
			struct traits<E> {
				static auto& pool(HEMesh* mesh) { return mesh->poolE; }
				static auto& table(HEMesh* mesh) { return mesh->tableE; }
				static auto& set(HEMesh* mesh) { return mesh->edges; }
				static auto& nextID(HEMesh* mesh) { return mesh->nextID_E; }
			};
			template<>
			struct traits<P> {
				static auto& pool(HEMesh* mesh) { return mesh->poolP; }
				static auto& table(HEMesh* mesh) { return mesh->tableP; }
				static auto& set(HEMesh* mesh) { return mesh->polygons; }
				static auto& nextID(HEMesh* mesh) { return mesh->nextID_P; }
			};
		};
	}
}

#include<CppUtil/Basic/HEMesh/HEMesh.inl>

#endif // !_CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_
