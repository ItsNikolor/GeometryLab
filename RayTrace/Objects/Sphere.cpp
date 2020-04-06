#include"Object.h"
#include<cmath>

Point Sphere::getNormal(const Point &p,const Point &dir) {
	Point normal = p - center;
	normal = normal / normal.len();
	if (normal * dir > 0)
		return -normal;
	return normal;
}
bool Sphere::rayIntersect(const Point& dir, const Point& Origin, float& ans) {
	auto k = Origin - center;
	return sphereIntersect(dir, k, ans);
}
bool Sphere::sphereIntersect(const Point& dir, const Point& k, float& ans) {
	auto b = dir * k;
	auto c = k * k - radius_2;
	auto a = dir * dir;
	auto D = b * b - a * c;
	if (D < 0) return false;
	D = sqrt(D);
	ans = (-b - D) / a;
	if (ans < 0)
		ans = (-b + D) / a;
	return ans > 0;
}

Point Ellipse::getNormal(const Point& p, const Point& dir) {
	auto new_p = p, new_center = center;

	new_p.rotationz(cosz, sinz);
	new_center.rotationz(cosz, sinz);

	new_p.rotationy(cosy, siny);
	new_center.rotationy(cosy, siny);
	
	Point normal = Point((new_p.first - new_center.first) / A / A, (new_p.second - new_center.second) / B / B, (new_p.third - new_center.third) / C / C);

	normal.rotationy(cosy, -siny);
	normal.rotationz(cosz, -sinz);

	normal = normal / normal.len();
	if (normal * dir > 0)
		return -normal;
	return normal;
}
bool Ellipse::rayIntersect(const Point& dir, const Point& Origin, float& ans) {
	Point new_dir = dir, new_Origin = Origin, new_center = center;
	auto k = new_Origin - new_center;

	new_dir.rotationz(cosz, sinz);
	k.rotationz(cosz, sinz);

	new_dir.rotationy(cosy, siny);
	k.rotationy(cosy, siny);

	new_dir = new_dir / Point(A, B, C);
	k = k / Point(A, B, C);
	

	return sphereIntersect(new_dir, k, ans);
}