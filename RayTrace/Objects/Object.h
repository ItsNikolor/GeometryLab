#pragma once
#include"../Point.h"
#include"../Materials/Material.h"
#include <functional>

class Object {
public:
	const Material *material;
	std::function<Point(Point t)> texture;
	Object(const Material* material, std::function<Point(Point t)> texture) :material(material),texture(texture) {};
	virtual Point getNormal(const Point &p,const Point &dir) = 0;
	virtual bool rayIntersect(const Point& dir, const Point &Origin, float &ans) = 0;
};

class Sphere : public Object {
protected:
	float radius;
	float radius_2;
	Point center;
	bool sphereIntersect(const Point& dir, const Point& k, float& ans);
public:
	Sphere(const Point& center, float radius, const Material* material, std::function<Point(Point t)> texture) :
		Object(material, texture), radius(radius), center(center), radius_2(radius* radius) {};
	virtual Point getNormal(const Point &p,const Point &dir) override;
	virtual bool rayIntersect(const Point& dir, const Point& Origin, float& ans) override;
};

class Ellipse : public Sphere {
	float A, B, C;
	float cosz, sinz;
	float cosy, siny;
public:
	Ellipse(const Point& center, float A, float B, float C,float az,float ay, const Material* material, std::function<Point(Point t)> texture) :
		Sphere(center, 1, material, texture), A(A), B(B), C(C),cosy(cos(ay)),siny(sin(ay)),cosz(cos(az)),sinz(sin(az)) {}
	Point getNormal(const Point& p, const Point& dir) override;
	bool rayIntersect(const Point& dir, const Point& Origin, float& ans) override;
};

class Plane : public Object {
protected:
	Point normal;
	Point center;
	float t_max = std::numeric_limits<float>::max();
	float m_max = std::numeric_limits<float>::max();
	float t_min = -1;
	float m_min = -1;
public:
	Point a, b;

	Plane(const Point& a, const Point& b, const Point& center,
		const Material* material, std::function<Point(Point t)> texture) :
		Object(material, texture), center(center), a(a / a.len()), b(b / b.len()) {
		normal = a.dot(b);
		normal = normal / normal.len();
	};
	Plane(const Point& a, const Point& b, float w, float h,
		const Point& center, const Material* material, std::function<Point(Point t)> texture) :
		Object(material, texture), center(center), a(a / a.len()), b(b / b.len()),
		t_max(w / 2), m_max(h / 2) {
		normal = a.dot(b);
		normal = normal / normal.len();
	};
	Plane(const Point& a, const Point& b, float w, float h, float w2, float h2,
		const Point& center, const Material* material, std::function<Point(Point t)> texture) :
		Object(material, texture), center(center), a(a / a.len()), b(b / b.len()),
		t_max(w / 2), m_max(h / 2), t_min(w / 2 - w2), m_min(h / 2 - h2) {
		normal = a.dot(b);
		normal = normal / normal.len();

	};
	Point getNormal(const Point& p, const Point& dir) override;
	bool rayIntersect(const Point& dir, const Point& Origin, float& ans) override;
};

class Circle : public Plane {
protected:
	float radius;
public:
	Circle(const Point& center, float radius, const Point& a, const Point& b, const Material* material, std::function<Point(Point t)> texture) :
		Plane(a, b, center, material, texture), radius(radius) {};
	bool rayIntersect(const Point& dir, const Point& Origin, float& ans) override;
};