#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_FORWARDDECL_H_
#define _CPPUTIL_BASIC_HEMESH_FORWARDDECL_H_

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
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_FORWARDDECL_H_
