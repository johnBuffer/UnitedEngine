#pragma once

#include <cmath>

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

	float length() const
	{
		if (_length < 0.0f)
			_length = sqrt(x*x + y*y);
		return _length;
	}

	float normalize()
	{
		x /= length();
		y /= length();
	}

	float x, y;

private:
	mutable float _length;
};

Vec2 operator+(const Vec2& v1,const Vec2& v2)
{
	return Vec2(v1.x + v2.x, v1.y + v2.y);
}

Vec2 operator-(const Vec2& v1, const Vec2& v2)
{
	return Vec2(v1.x - v2.x, v1.y - v2.y);
}

Vec2 operator+(const Vec2& v)
{
	return Vec2(-v.x, -v.y);
}

Vec2 operator*(float f, const Vec2& v)
{
	return Vec2(f*v.x, f*v.y);
}

Vec2 operator*(const Vec2& v, float f)
{
	return Vec2(f*v.x, f*v.y);
}

