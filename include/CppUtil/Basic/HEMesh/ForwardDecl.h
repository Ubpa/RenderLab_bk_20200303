#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_FORWARDDECL_H_
#define _CPPUTIL_BASIC_HEMESH_FORWARDDECL_H_

#include <CppUtil/Basic/Ptr.h>
#include <assert.h>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		class THalfEdge;
		template<typename V, typename E, typename P>
		class TVertex;
		template<typename V, typename E, typename P>
		class TEdge;
		template<typename V, typename E, typename P>
		class TPolygon;

		template<typename V, typename, typename, typename>
		class _enable_HEMesh;
		template<typename V>
		class HEMesh;

		template <typename E, typename P>
		class EmptyV;
		template <typename V, typename E>
		class EmptyP;
		template <typename V, typename P>
		class EmptyE;

		template <typename V>
		class EmptyEP_E;
		template <typename V>
		class EmptyEP_P;

		class AllEmpty;

		template<typename T, typename HEMesh_t>
		class HEMesh_ptr {
		public:
			using element_type = T;
			using MeshT = HEMesh_t;
		public:
			size_t Hash() const { return std::hash<int>()(idx); }
		public:
			HEMesh_ptr(HEMesh_t * mesh = nullptr, int idx = -1) : mesh(mesh), idx(idx) {}
			T* operator->() const { return mesh->Get<std::remove_const_t<T>>(idx); }
			bool operator==(const HEMesh_ptr& p) const { assert(mesh == p.mesh); return idx == p.idx; }
			bool operator==(std::nullptr_t) const { return idx == -1; }
			bool operator<(const HEMesh_ptr& p) const { assert(mesh == p.mesh); return idx < p.idx; }
			bool operator!=(const HEMesh_ptr& p) const { assert(mesh == p.mesh); return idx != p.idx; }
			bool operator!=(std::nullptr_t) const { return idx != -1; }
			HEMesh_ptr& operator=(const HEMesh_ptr& p) {
				mesh = p.mesh;
				idx = p.idx;
				return *this;
			}
			HEMesh_ptr& operator=(std::nullptr_t) { mesh = nullptr; idx = -1; return *this; }
			operator bool() const { return idx != -1; }
			operator HEMesh_ptr<const T, HEMesh_t>() const { return HEMesh_ptr<const T, HEMesh_t>(mesh, idx); }

		private:
			template<typename V>
			friend class HEMesh;
			HEMesh_t * mesh;
			int idx;
		};

		// use _ to avoid _Vector_alloc, base class of vector
		template<template<typename, typename ...> class ContainerT, typename ValT, typename HEMesh_t, typename ... Args>
		ContainerT<HEMesh_ptr<const ValT, HEMesh_t>> Const(const ContainerT<HEMesh_ptr<ValT, HEMesh_t>, Args...>& c) {
			return ContainerT<HEMesh_ptr<const ValT, HEMesh_t>>(c.begin(), c.end());
		}
	}
}

namespace std {
	template<typename T, typename HEMesh_t>
	struct hash<CppUtil::Basic::HEMesh_ptr<T, HEMesh_t>> {
		size_t operator()(const CppUtil::Basic::HEMesh_ptr<T, HEMesh_t>& p) const {
			return p.Hash();
		}
	};
}

#endif // !_CPPUTIL_BASIC_HEMESH_FORWARDDECL_H_
