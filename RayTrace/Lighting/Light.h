#pragma once
#include<vector>
#include"../Point.h"
class Lightning {
protected:
	Lightning(const float intensity) :intensity(intensity) {};
public:
	float intensity;
	virtual Point getDirection(const Point &p) = 0;
	virtual double getDistance(const Point& p) = 0;
	virtual float getT(const Point& p,const Point& dir) = 0;
};

class Point_light : public Lightning {
	Point position;
	float radius = 0;
public:
	Point_light(const float intensity, const Point &position,float radius=0) :Lightning(intensity),
		position(position),radius(radius) { };
	Point getDirection(const Point &p) override {
		Point dir = position-p;
		return dir / dir.len();
	};
	double getDistance(const Point& p) override {
		return (p - position).len();
	}
	float getT(const Point& p,const Point &dir) {
		if(!radius)
			return position.findT(p, dir);
		return (position - p).len() - radius;
	}
};

class Direct_light : public Lightning {
	Point direction;
public:
	Direct_light(const float intensity, const Point &direction) : Lightning(intensity), direction(direction/direction.len()) {};
	Point getDirection(const Point &p) override {
		return direction;
	}
	double getDistance(const Point& p) override {
		return std::numeric_limits<double>::max();
	}
	float getT(const Point& p, const Point& dir) {
		return std::numeric_limits<float>::max();
	}
};

class Ambient_light : public Lightning {
public:
	Ambient_light(const float intensity) :Lightning(intensity) {};
	Point getDirection(const Point &p) override {
		return Point(0, 0, 0);
	}
	double getDistance(const Point& p) override {
		return 0;
	}
	float getT(const Point& p, const Point& dir) {
		return 0;
	}

};