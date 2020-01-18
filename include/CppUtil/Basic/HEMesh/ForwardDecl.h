#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_FORWARDDECL_H_
#define _CPPUTIL_BASIC_HEMESH_FORWARDDECL_H_

#include <CppUtil/Basic/Ptr.h>

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

		template<typename T> // enable
		struct HEMesh_ID {
			struct hash {
				size_t operator()(const HEMesh_ID& id) const noexcept {
					return std::hash<int>()(id.val);
				}
			};
			explicit HEMesh_ID(int val = -1) : val(val) {}
			bool expired() const { return val == -1; }
			void reset() { val = -1; }
			bool operator==(const HEMesh_ID& id) const = default;
			bool operator<(const HEMesh_ID& id) const { return val < id.val; }
			operator HEMesh_ID<const T>() const { return HEMesh_ID<const T>(val); }
			operator HEMesh_ID<std::remove_const_t<T>>() const { return HEMesh_ID<std::remove_const_t<T>>(val); }
			int val;
		};

		template<typename T, typename HEMesh_t>
		class HEMesh_ptr {
		public:
			using element_type = T;
			using MeshT = HEMesh_t;

		private:
			using IDt = HEMesh_ID<std::remove_const_t<T>>;
		public:
			struct hash {
				size_t operator()(const HEMesh_ptr& p) const noexcept {
					return typename IDt::hash()(p.ID);
				}
			};
		public:
			HEMesh_ptr(Ptr<HEMesh_t> mesh = nullptr, IDt ID = IDt()) : mesh(mesh), ID(ID) {}
			HEMesh_ptr(std::nullptr_t) : HEMesh_ptr(nullptr, IDt(-1)) { }
			T* operator->() { return mesh.lock()->Get(ID); }
			T* const operator->() const { return const_cast<HEMesh_ptr*>(this)->operator->(); }
			bool expired() const { return ID.expired() || mesh.expired(); }
			void reset() { mesh.reset(); ID.reset(); }
			bool operator==(const HEMesh_ptr& p) const { return mesh.lock() == p.mesh.lock() && ID == p.ID; }
			bool operator<(const HEMesh_ptr& p) const { return mesh.lock() < p.mesh.lock() || (mesh.lock() == p.mesh.lock() && ID < p.ID); }
			bool operator!=(const HEMesh_ptr& p) const { return !(this->operator==(p)); }
			HEMesh_ptr& operator=(const HEMesh_ptr& p) {
				mesh = p.mesh.lock();
				ID = p.ID;
				return *this;
			}
			operator bool() const { return this->operator!=(nullptr); }
			operator HEMesh_ptr<const T, HEMesh_t>() const { return HEMesh_ptr<const T, HEMesh_t>(mesh.lock(), ID); }
		private:
			template<typename V, typename, typename, typename>
			friend class HEMesh;
			WPtr<HEMesh_t> mesh;
			IDt ID;
		};

		// use _ to avoid _Vector_alloc, base class of vector
		template<template<typename, typename ...> class ContainerT, typename ValT, typename HEMesh_t, typename ... Args>
		ContainerT<HEMesh_ptr<const ValT, HEMesh_t>> Const(const ContainerT<HEMesh_ptr<ValT, HEMesh_t>, Args...>& c) {
			return ContainerT<HEMesh_ptr<const ValT, HEMesh_t>>(c.begin(), c.end());
		}
	}
}

namespace std {
	template<typename T>
	struct hash<CppUtil::Basic::HEMesh_ID<T>> {
		size_t operator()(const CppUtil::Basic::HEMesh_ID<T>& id) const {
			return typename CppUtil::Basic::HEMesh_ID<T>::hash()(id);
		}
	};
	template<typename T, typename HEMesh_t>
	struct hash<CppUtil::Basic::HEMesh_ptr<T, HEMesh_t>> {
		size_t operator()(const CppUtil::Basic::HEMesh_ptr<T, HEMesh_t>& p) const {
			return typename CppUtil::Basic::HEMesh_ptr<T, HEMesh_t>::hash()(p);
		}
	};
}

#endif // !_CPPUTIL_BASIC_HEMESH_FORWARDDECL_H_
