#pragma once
#include"../Point.h"
#include <cstdint>
#include<algorithm>


struct Material {
	Material(float Ka, float Kd, float Ks, float specular, float reflect, float transparency, float refraction) :
		Ka(Ka), Kd(Kd), Ks(Ks), specular(specular), reflect(reflect), transparency(transparency), refraction(refraction) {};

	float Ka, Kd, Ks;
	float specular;
	float reflect;
	float transparency;
	float refraction;
	float computeLightning(const Point& normal, const Point& light_direction, const Point& dir) const {
		float ans = std::max(float(0), normal * light_direction) * Kd;
		if (specular) {
			auto R = normal * (normal * light_direction) * 2 - light_direction;
			auto tmp = R * dir;
			if (tmp > 0)
				ans += pow(tmp / R.len(), specular) * Ks;
		}
		return ans;
	};
};

