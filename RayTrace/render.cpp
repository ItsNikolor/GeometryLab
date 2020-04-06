#include<vector>
#include<algorithm>
#include<thread>
#include <functional>
#include<unordered_map>


#include "render.h"
#include "Point.h"
#include "Lighting/Light.h"
#include "Materials/Material.h"
#include "Objects/Object.h"


using namespace std;

const uint32_t RED = 0x000000FF;
const uint32_t GREEN = 0x0000FF00;
const uint32_t BLUE = 0x00FF0000;


const uint32_t background = 0;
float ambient = 0.2f;
Point Canvas;
Point Origin;
int max_recursion;

Point rgb(uint32_t color) {
	return Point(color & 0x000000FF,
		(color & 0x0000FF00) >> 8,
		(color & 0x00FF0000) >> 16);
}

Point TraceRay(const Point& p, const Point& dir, vector<unique_ptr<Lightning>>& Lights, vector < unique_ptr<Object>>& Objects,
	int depth, unordered_map<Lightning*, Object*>& shadow_cash);
void worker(vector<uint32_t>& image, vector < unique_ptr<Object>>& Objects, vector<unique_ptr<Lightning>>& Lights,
	int w, int h, int num_worker, int threads_count);

Point board(const Point &a,const Point &b, const Point& Origin, int w, int h,
	const Point& first_color, const Point& second_color, const Point& p) {
	w *= 2;
	h *= 2;
	auto tmp = p - Origin;
	float x = tmp * a;
	float y = tmp * b;
	x = ceil(x);
	y = ceil(y);

	x = int(x) % w;
	if (x < 0) x += w;

	y = int(y) % h;
	if (y < 0)y += h;

	if ((x < w / 2 && y < h / 2) || (x >= w / 2 && y >= h / 2))
		return first_color;
	return second_color;
}

function<Point(Point)> set_board(const Point& a, const Point& b, const Point &Origin,int w,int h,
	const Point &first_color=Point(0,0,0),const Point &second_color=Point(255,255,255)) {
	return bind(board, a,b, Origin,  w, h,first_color,second_color,std::placeholders::_1);
}


void init(Point& Canvas, Point& Origin) {
	Canvas = Point(1, 1, 1);
	Origin = Point(0, -6, 3);
}

void render(vector<uint32_t>& image, size_t w, size_t h, int threads,int scene) {
	if (scene == 1) max_recursion = 32;
	else max_recursion = 6;
	init(Canvas, Origin);

	vector<unique_ptr<Lightning>> Lights;

	Material matt(0.5, 0.5, 0, 0, 0, 0, 0);
	Material shiny_matt(0.33, 0.33, 0.33, 4, 0, 0, 0);
	Material specular(0.33, 0.33, 0.33, 500, 0, 0, 0);
	Material mirror(1, 0, 0, 0, 1, 0, 0);
	Material nothing(1, 0, 0, 0, 0, 1, 1);
	Material like_water(0.33, 0.33, 0.33, 100, 0.3, 0.8, 1.33);
	Material shiny1(0.33, 0.33, 0.33, 500, 0.2, 0, 0);
	Material shiny2(0.33, 0.33, 0.33, 500, 0.3, 0, 0);
	Material shiny3(0.33, 0.33, 0.33, 10, 0.4, 0, 0);
	Material just_refr(1, 1, 1, 0, 0, 1, 1.33);
	Material glass(1, 1, 1, 0, 0, 1, 1.52);
	Material light(7, 0, 0, 0, 0, 0, 0);

	vector < unique_ptr<Object>> Objects;

	std::function<std::function<Point(Point t)>(Point color)> set_color = [](Point color) {return [color](Point t) {return color; }; };

	if (scene == 1) {
		Point a(1, 0, 0), b(0, 1, 0), c(0, 0, 1);

		Objects.emplace_back(new Circle(Point(0, 5, 7.8), 0.2, a, c, &light, set_color(Point(255, 255, 255))));
		Lights.emplace_back(new Point_light(0.8, Point(0, 5, 7.8),0.1));

		Objects.emplace_back(new Circle(Point(-2.25, 5, 1.5), 0.2, a, c, &light, set_color(Point(255, 255, 255))));
		Lights.emplace_back(new Point_light(0.8, Point(-2.25, 5, 1.5),0.1));

		Objects.emplace_back(new Circle(Point(2.25, 5, 1.5), 0.2, a, c, &light, set_color(Point(255, 255, 255))));
		Lights.emplace_back(new Point_light(0.8, Point(2.25, 5, 1.5),0.1));

		Objects.emplace_back(new Plane(a, b, Point(0, 0, 0), &shiny2, set_board(a, b, Point(0, 0, 0), 2, 2)));

		Objects.emplace_back(new Plane(Point(1, 0, 0), Point(0, 0, 1), 5, 8, 0.5, 0.5, Point(0, 5, 4), &matt, set_color(Point(0, 255, 255))));
		Objects.emplace_back(new Plane(Point(1, 0, 0), Point(0, 0, 1), 4, 7, Point(0, 5, 4), &mirror, set_color(Point(0, 255, 255))));

		Objects.emplace_back(new Plane(Point(1, 0, 0), Point(0, 0, 1), Point(0, -6, 4), &mirror, set_color(Point(0, 255, 255))));

		Objects.emplace_back(new Plane(Point(0, 1, 0), Point(0, 0, 1), Point(5, 0, 0), &shiny_matt, set_color(Point(0, 255, 255))));
		Objects.emplace_back(new Plane(Point(0, 1, 0), Point(0, 0, 1), Point(-5, 0, 0), &shiny_matt, set_color(Point(0, 255, 255))));


		Objects.emplace_back(new Sphere(Point(1.3, 4.3, 0.7), 0.7, &shiny3, set_color(Point(255, 255, 0))));
		auto pi = 3.1415926353;
		Objects.emplace_back(new Ellipse(Point(-1., 1.5, 0.8), 2.5, 0.8, 0.8,-pi/4,0, &shiny3, set_color(Point(0, 255, 0))));
	}
	else {
		Point v1(0, 1, 0), v2(0, 0, 1), v3(1, 0, 0);

		Objects.emplace_back(new Plane(v1, v2, 0.5, 0.5, Point(2, 0, 3), &light, set_color(Point(255, 255, 255))));
		Lights.emplace_back(new Point_light(0.8, Point(1.95, 0, 3), 0.25));

		Objects.emplace_back(new Plane(v1, v2, 0.5, 0.5, Point(2, 3, 5), &light, set_color(Point(255, 255, 255))));
		Lights.emplace_back(new Point_light(0.8, Point(1.95, 3, 5), 0.25));

		Objects.emplace_back(new Plane(v3, v2, 0.5, 0.5, Point(0, 7, 5), &light, set_color(Point(255, 255, 255))));
		Lights.emplace_back(new Point_light(0.8, Point(0, 7, 5), 0.25));

		Objects.emplace_back(new Plane(v3, v2, 0.5, 0.5, Point(-3, 7, 4), &light, set_color(Point(255, 255, 255))));
		Lights.emplace_back(new Point_light(0.8, Point(-3, 7, 4), 0.25));

		Objects.emplace_back(new Plane(v3, v1, 0.5, 0.5, Point(-4, 3, 7), &light, set_color(Point(255, 255, 255))));
		Lights.emplace_back(new Point_light(0.8, Point(-4, 3, 7), 0.25));

		Point a(1, 0, 0), b(0, 1, 0);

		Objects.emplace_back(new Plane(a, b, Point(0, 0, 0), &shiny2, set_board(a, b, Point(0, 0, 0), 1, 1, Point(178, 200, 100))));
		Objects.emplace_back(new Plane(a, b, Point(0, 0, 7), &shiny_matt, set_color(Point(178, 200, 100))));
		Objects.emplace_back(new Plane(v1, v2, Point(2, 0, 0), &shiny1, set_color(Point(128, 170, 53))));
		Objects.emplace_back(new Plane(Point(1, 0, 0), Point(0, 0, 1), Point(0, 7, 0), &shiny_matt, set_color(Point(128, 170, 53))));

		Objects.emplace_back(new Sphere(Point(0.5, 4, 1), 1, &shiny2, set_color(Point(70, 25, 80))));
		Objects.emplace_back(new Sphere(Point(-1, 0, 1), 1, &glass, set_color(Point(70, 25, 80))));
	}

	
	vector<thread> t;
	for (int i = 0; i < threads;i++) {
		t.emplace_back(worker, ref(image), ref(Objects), ref(Lights), w, h, i, threads);
		//worker(image,Objects,Lights,w,h,i,threads);
	}

	for (auto& i : t)
		i.join();
	
}

void worker(vector<uint32_t>& image, vector < unique_ptr<Object>>& Objects, vector<unique_ptr<Lightning>>& Lights,
	int w, int h, int num_worker, int threads_count) {

	unordered_map<Lightning*, Object*> shadow_cash;
	for (auto& l : Lights)
		shadow_cash[l.get()] = Objects.front().get();

	int j_begin = h * num_worker / threads_count;
	int j_end = h * (num_worker + 1) / threads_count;

	float y = Canvas.second;
	vector<Point> cash(w + 1);
	{
		float z = (Canvas.third * h - 1) / 2.0f - j_begin;
		z += 1.f;
		z /= h;
		for (size_t i = 0; i < w + 1; i++) {
			float x = (-Canvas.first * w + 1) / 2.0f + i;
			x -= 1.f;
			x /= w;

			Point dir(x, Canvas.second, z), p = dir + Origin;
			dir = dir / dir.len();

			cash[i] = TraceRay(p, dir, Lights, Objects, 0, shadow_cash);
		}
	}

	for (int j = j_begin; j < j_end; j++) {
		float z = (Canvas.third * h - 1) / 2.0f - j;
		z /= h;
		Point pred = cash[1];
		{
			Point s = cash[0] + cash[1];
			float x = (-Canvas.first * w + 1) / 2.0f + 0.f;
			x /= w;

			Point dir(x, Canvas.second, z), p = dir + Origin;
			dir = dir / dir.len();

			s = s + TraceRay(p, dir, Lights, Objects, 0, shadow_cash);


			dir = Point(x - 1.f / w, y, z - 1.f / h), p = dir + Origin;
			dir = dir / dir.len();

			cash[0] = TraceRay(p, dir, Lights, Objects, 0, shadow_cash);
			s = s + cash[0];


			dir = Point(x + 1.f / w, y, z - 1.f / h), p = dir + Origin;
			dir = dir / dir.len();

			cash[1] = TraceRay(p, dir, Lights, Objects, 0, shadow_cash);
			s = s + cash[1];

			image[0 + (h - 1 - j) * w] = (s / 5).getColor();
		}

		for (size_t i = 1; i < w; i++) {
			float x = (-Canvas.first * w + 1) / 2.0f + i;
			x /= w;

			Point s = cash[i + 1] + cash[i] + pred;
			pred = cash[i + 1];
			Point dir(x, Canvas.second, z), p = dir + Origin;
			dir = dir / dir.len();

			s = s + TraceRay(p, dir, Lights, Objects, 0, shadow_cash);



			dir = Point(x + 1.f / w, y, z - 1.f / h), p = dir + Origin;
			dir = dir / dir.len();

			cash[i + 1] = TraceRay(p, dir, Lights, Objects, 0, shadow_cash);
			s = s + cash[i + 1];

			image[i + (h - 1 - j) * w] = (s / 5).getColor();
		}
	}
}
Point find_refr(const Point& dir, const Point& normal, float ref1, float ref2) {
	auto v1 = dir / dir.len() * ref1;
	auto tmp = v1 * normal;
	auto v2 = v1 + normal * (sqrt((ref2 * ref2 - ref1 * ref1) / (tmp * tmp) + 1) - 1) * (v1 * normal);
	v2 = v2 / v2.len();
	return v2;
}



Point TraceRay(const Point& p, const Point& dir, vector<unique_ptr<Lightning>>& Lights, vector < unique_ptr<Object>>& Objects,
	int depth, unordered_map<Lightning*, Object*> &shadow_cash) {

	Point closest_point;
	Object* closest_obj = nullptr;
	float closest_t = std::numeric_limits<float>::max();
	for (auto& o : Objects) {
		float t;
		if (o->rayIntersect(dir, p, t)) {
			if (t < closest_t) {
				closest_obj = o.get();
				closest_t = t;
			}
		}
	}

	if (closest_obj == nullptr)
		return rgb(background);
	else {
		closest_point = p + dir * closest_t;
		float sum_light = ambient * closest_obj->material->Ka;
		Point normal = closest_obj->getNormal(closest_point, dir);
		float eps = 0.001;

		for (auto& l : Lights) {
			Point light_dir = l->getDirection(closest_point);
			auto eps_closest_point = closest_point + light_dir * eps;;
			float t_max = l->getT(eps_closest_point, light_dir);
			float t;

			if (shadow_cash[l.get()]->rayIntersect(light_dir, eps_closest_point, t) &&
				(t < t_max))
				t_max = -1;
			else {
				for (auto& o : Objects) {
					if (o->rayIntersect(light_dir, eps_closest_point, t))
						if (t < t_max) {
							shadow_cash[l.get()] = o.get();
							t_max = -1;
							break;
						}
				}
			}
			
			if (t_max != -1)
				sum_light += closest_obj->material->computeLightning(normal, light_dir, -dir) * l->intensity;
		}
		auto local_color = closest_obj->texture(closest_point) * std::min(sum_light, 1.f);

		auto reflect = closest_obj->material->reflect;
		if (reflect && depth < max_recursion) {
			auto d = -dir;
			auto R = normal * (normal * d) * 2 - d;
			R = R / R.len();
			local_color = local_color * (1 - reflect) + TraceRay(closest_point + R * eps, R, Lights, Objects, depth + 1, shadow_cash) * reflect;
		}
		auto transparency = closest_obj->material->transparency;
		auto refraction = closest_obj->material->refraction;
		if (transparency) {
			auto new_dir = find_refr(dir, normal, 1, refraction);
			float t;
			closest_obj->rayIntersect(new_dir, closest_point + new_dir * eps, t);
			closest_point = closest_point + new_dir * (eps + t);
			normal = closest_obj->getNormal(closest_point, new_dir);
			new_dir = find_refr(new_dir, normal, refraction, 1);

			local_color = local_color * (1 - transparency) + TraceRay(closest_point + new_dir * eps, new_dir, Lights, Objects, depth , shadow_cash) * transparency;
		}
		return local_color;
	}
}
