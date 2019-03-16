#pragma once

#include <cmath>

namespace up
{

struct Vec2
{
	Vec2() noexcept :
		x(0.0f),
		y(0.0f),
		_length(-1.0f)
	{}

	Vec2(float x, float y) noexcept :
		x(x),
		y(y),
		_length(-1.0)
	{}

	~Vec2() noexcept = default;

	float length() const
	{
		if (_length < 0.0f)
			_length = sqrt(x*x + y*y);
		return _length;
	}

	float length2() const
	{
		return x*x + y*y;
	}

	void normalize()
	{
		float l = length();

		x /= l;
		y /= l;

		_length = 1.0f;
	}

	Vec2 getNormal() const
	{
		return Vec2(-y, x);
	}

	void operator+=(const Vec2& v)
	{
		x += v.x;
		y += v.y;
	}

	float x, y;

private:
	mutable float _length;
};

inline Vec2 operator+(const Vec2& v1,const Vec2& v2)
{
	return Vec2(v1.x + v2.x, v1.y + v2.y);
}

inline Vec2 operator-(const Vec2& v1, const Vec2& v2)
{
	return Vec2(v1.x - v2.x, v1.y - v2.y);
}

inline Vec2 operator+(const Vec2& v)
{
	return Vec2(-v.x, -v.y);
}

inline Vec2 operator*(float f, const Vec2& v)
{
	return Vec2(f*v.x, f*v.y);
}

inline Vec2 operator*(const Vec2& v, float f)
{
	return Vec2(f*v.x, f*v.y);
}

struct Segment
{
	Segment(const Vec2& p1, const Vec2& p2) :
		p1(p1),
		p2(p2)
	{
		x = p2.x - p1.x;
		y = p2.y - p1.y;
	}

	Vec2 p1, p2;
	float x, y;
};

struct Intersection
{
	Intersection(const Segment& s1, const Segment& s2) :
		cross(false)
	{
		float s1_x(s1.x), s1_y(s1.y), s2_x(s2.x), s2_y(s2.y);
		float p0_x(s1.p1.x), p0_y(s1.p1.y), p2_x(s2.p1.x), p2_y(s2.p1.y);

		float s, t;
		s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
		t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

		if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
		{
			// Intersection detected
			point.x = p0_x + (t * s1_x);
			point.y = p0_y + (t * s1_y);

			cross = true;
		}
	}

	Intersection(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec2& p4) :
		Intersection(Segment(p1, p2), Segment(p3, p4))
	{}

	bool cross;
	Vec2 point;
};

}
