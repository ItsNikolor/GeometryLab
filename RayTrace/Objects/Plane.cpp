#include"Object.h"
#include<cmath>

Point Plane::getNormal(const Point& p, const Point& dir) {
	if (normal * dir > 0)
		return -normal;
	return normal;
}

bool Plane::rayIntersect(const Point& dir, const Point& Origin, float& ans) {
	Point c = center - Origin;
	auto tmp = normal * c;
	if (tmp)
		ans = tmp / (normal * dir);
	else
		return false;
	if (ans <= 0) return false;

	if (t_max == std::numeric_limits<float>::max()) return true;
	Point p = Origin + dir * ans;
	p = p - center;
	float t = a * p;
	float m = b * p;
	return std::abs(t) < t_max && std::abs(m) < m_max && (std::abs(t) > t_min || std::abs(m) > m_min);
}

bool Circle::rayIntersect(const Point& dir, const Point& Origin, float& ans) {
	if (Plane::rayIntersect(dir, Origin, ans)) {
		Point p = Origin + dir * ans;
		return radius >= (p - center).len();
	}
	return false;
}