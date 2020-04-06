#pragma once
#include<cmath>
#include<limits>
#include <cstdint>

struct Point {
	float first, second, third;
	Point(const float a=0, const float b=0, const float c=0) :first(a), second(b), third(c) {};
	/*Point& operator=(const Point& a) {
		first = a.first;
		second = a.second;
		third = a.third;
		return *this;
	}*/

	Point operator+(const Point &a) const {
		return Point(first + a.first, second + a.second, third + a.third);
	};
	Point operator-(const Point& a) const {
		return Point(first - a.first, second - a.second, third - a.third);
	};
	Point operator/(const float &a) const {
		return Point(first / a, second / a, third / a);
	}
	Point operator*(const float &a) const {
		return Point(first * a, second * a, third * a);
	}
	double len() const {
		return sqrt(double(first) * first + double(second) * second + double(third) * third);
	}
	float operator*(const Point& a) const {
		return first * a.first + second * a.second + third * a.third;
	}
	static Point maxPoint() {
		float constexpr m = std::numeric_limits<float>::max();
		return Point(m, m, m);
	}
	Point dot(const Point& a) const {
		return Point(second * a.third - third * a.second,
			-first * a.third + third * a.first,
			first * a.second - second * a.first);
	}
	bool operator==(const Point& a) const {
		return first == a.first && second == a.second && third == a.third;
	}
	bool operator!=(const Point& a) const {
		return !this->operator==(a);
	}
	uint32_t getColor() const {
		int a = static_cast<unsigned int>(first);
		int b = static_cast<unsigned int>(second) << 8;
		int c = static_cast<unsigned int>(third) << 16;
		return a + b + c;
	}
	Point operator-() const {
		return Point(-first, -second, -third);
	}
	float findT(const Point& Origin, const Point& dir) {
		if (dir.first)
			return (first - Origin.first) / dir.first;
		else if (dir.second)
			return (second - Origin.second) / dir.second;
		else
			return (third - Origin.third) / dir.third;
	}
	void rotationz(float cos,float sin) {
		float x = first * cos + second * sin;
		float y = -first * sin + second * cos;
		float z = third;
		
		*this=Point(x, y, z);
	}
	void rotationy(float cos, float sin) {
		float x = first*cos - third * sin;
		float y = second;
		float z = first * sin + third * cos;

		*this = Point(x, y, z);
	}
	Point operator/(const Point& p) const {
		return Point(first / p.first, second / p.second, third / p.third);
	}
};

