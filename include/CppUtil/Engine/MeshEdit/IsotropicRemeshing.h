#ifndef _CPPUTIL_ENGINE_MESHEDIT_ISOTROPICREMESHING_H_
#define _CPPUTIL_ENGINE_MESHEDIT_ISOTROPICREMESHING_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/HEMesh/HEMesh.h>
#include <CppUtil/Basic/UGM/UGM.h>

namespace CppUtil {
	namespace Engine {
		class TriMesh;

		class IsotropicRemeshing : public Basic::HeapObj {
		public:
			IsotropicRemeshing(Basic::Ptr<TriMesh> triMesh);

		public:
			static const Basic::Ptr<IsotropicRemeshing> New(Basic::Ptr<TriMesh> triMesh) {
				return Basic::New<IsotropicRemeshing>(triMesh);
			}

		public:
			bool Init(Basic::Ptr<TriMesh> triMesh);
			void Clear();
			bool Run(size_t n);

		private:
			bool Kernel(size_t n);

		private:
			class V;
			class E;
			class P;
			class V : public Basic::TVertex<V, E, P> {
			public:
				Vec3 pos;
				Vec3 newPos;
			};
			class E : public Basic::TEdge<V, E, P> {
			public:
				float Length() const { return (HalfEdge()->Origin()->pos - HalfEdge()->End()->pos).Norm(); }
				Vec3 Centroid() const { return (HalfEdge()->Origin()->pos + HalfEdge()->End()->pos) / 2.f; }
			};
			class P :public Basic::TPolygon<V, E, P> { };
		private:
			Basic::Ptr<TriMesh> triMesh;
			const Basic::Ptr<Basic::HEMesh<V>> heMesh;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_MESHEDIT_ISOTROPICREMESHING_H_
