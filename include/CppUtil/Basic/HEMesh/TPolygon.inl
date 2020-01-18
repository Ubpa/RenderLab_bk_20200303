#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_TPOLYGON_INL_
#define _CPPUTIL_BASIC_HEMESH_TPOLYGON_INL_

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		const std::vector<typename TPolygon<V, E, P>::ptr<E>> TPolygon<V, E, P>::BoundaryEdges() {
			std::vector<ptr<E>> edges;
			for (auto he : BoundaryHEs())
				edges.push_back(he->Edge());
			return edges;
		}

		template<typename V, typename E, typename P>
		const std::vector<typename TPolygon<V, E, P>::ptr<V>> TPolygon<V, E, P>::BoundaryVertice() {
			std::vector<ptr<V>> vertice;
			for (auto he : BoundaryHEs())
				vertice.push_back(he->Origin());
			return vertice;
		}
	}
}

#endif// !_CPPUTIL_BASIC_HEMESH_TPOLYGON_INL_
