#ifndef _CPPUTIL_ENGINE_DIRECTIONAL_LIGHT_H_
#define _CPPUTIL_ENGINE_DIRECTIONAL_LIGHT_H_

#include <CppUtil/Engine/Light.h>

namespace CppUtil {
	namespace Engine {
		// 局部坐标系中，朝下 -y
		class DirectionalLight : public Light {
		public:
			DirectionalLight(const RGBf &color = RGBf(1), float intensity = 1.0f)
				: color(color), intensity(intensity) { }

		public:
			static const Basic::Ptr<DirectionalLight> New(const RGBf &color = RGBf(1), float intensity = 1.0f) {
				return Basic::New<DirectionalLight>(color, intensity);
			}

		protected:
			virtual ~DirectionalLight() = default;

		public:
			// 采样 L 函数
			// !!! p，wi 处于光源的坐标空间中
			// @arg0  in，以 p 点来计算 distToLight 和 PD
			// @arg1 out，wi 指向光源，为单位向量
			// @arg2 out，p 点到光源采样点的距离
			// @arg3 out，概率密度 probability density
			virtual const RGBf Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const override;

			// 概率密度函数
			// !!! p，wi 处于灯的坐标空间中
			virtual float PDF(const Point3 & p, const Normalf & wi) const override { return 0; }

			virtual bool IsDelta() const override { return true; }

		public:
			RGBf color;
			float intensity;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_DIRECTIONAL_LIGHT_H_
