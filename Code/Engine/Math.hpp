#pragma once

#include "Base.hpp"
#include <math.h>

namespace Easy2D
{

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//

	const float PI = 3.14159265359f;
	const float PI2 = PI * 2;
	const float HALF_PI = PI / 2;
	const float QUAD_PI = PI / 4;
	const float Deg2Rad = PI / 180;
	const float Rad2Deg = 180 / PI;

	//!
	template <typename T> const T& Min(const T& _a, const T& _b) { return _a < _b ? _a : _b; }
	//!
	template <typename T> const T& Min(const T& _a, const T& _b, const T& _c) { return _a < _b ? (_a < _c ? _a : _c) : (_b < _c ? _b : _c); }
	//!
	template <typename T> const T& Max(const T& _a, const T& _b) { return _a > _b ? _a : _b; }
	//!
	template <typename T> const T& Max(const T& _a, const T& _b, const T& _c) { return _a > _b ? (_a > _c ? _a : _c) : (_b > _c ? _b : _c); }
	//!
	template <typename T> const T Clamp(T _x, T _l, T _u) { return _x > _l ? (_x < _u ? _x : _u) : _l; }
	//!
	template <typename T> const T Clamp01(T _x) { return Clamp<T>(_x, (T)0, (T)1); }
	//!
	template <typename T> T Mix(const T& _a, const T& _b, float _t) { return _a + (_b - _a) * _t; }

	//----------------------------------------------------------------------------//
	// ---
	//----------------------------------------------------------------------------//

	struct IntVector2
	{
		union
		{
			struct
			{
				int x, y;
			};
			int v[2];
		};
	};
	struct Vector3
	{
		union
		{
			struct
			{
				float x, y, z;
			};
			float v[3];
		};
	};

	struct Vector4
	{
		union
		{
			struct
			{
				float x, y, z, w;
			};
			struct
			{
				float r, g, b, a;
			};
			float v[4];
		};
	};


	struct Color4ub
	{
		union
		{
			struct
			{
				uint8 r, g, b, a;
			};
			uint8 v[4];
			uint8 rgba;
		};
	};

	//----------------------------------------------------------------------------//
	// Vector2
	//----------------------------------------------------------------------------//

	struct Vector2
	{
		//!
		Vector2(void) = default;
		//!
		Vector2(float _s) : x(_s), y(_s) { }
		//!
		Vector2(float _x, float _y) : x(_x), y(_y) { }

		//!
		const float operator [] (unsigned  int _index) const { return (&x)[_index]; }
		//!
		float& operator [] (unsigned  int _index) { return (&x)[_index]; }
		//!
		const float* operator * (void) const { return &x; }
		//!
		float* operator * (void) { return &x; }

		//!
		Vector2 operator - (void) const { return Vector2(-x, -y); }
		//!
		Vector2 operator + (const Vector2& _rhs) const { return Vector2(x + _rhs.x, y + _rhs.y); }
		//!
		Vector2 operator - (const Vector2& _rhs) const { return Vector2(x - _rhs.x, y - _rhs.y); }
		//!
		Vector2 operator * (const Vector2& _rhs) const { return Vector2(x * _rhs.x, y * _rhs.y); }
		//!
		Vector2 operator / (const Vector2& _rhs) const { return Vector2(x / _rhs.x, y / _rhs.y); }
		//!
		Vector2 operator * (float _rhs) const { return Vector2(x * _rhs, y * _rhs); }
		//!
		Vector2 operator / (float _rhs) const { return Vector2(x / _rhs, y / _rhs); }
		//!
		Vector2& operator += (const Vector2& _rhs) { x += _rhs.x, y += _rhs.y;	return *this; }
		//!
		Vector2& operator -= (const Vector2& _rhs) { x -= _rhs.x, y -= _rhs.y;	return *this; }
		//!
		Vector2& operator *= (const Vector2& _rhs) { x *= _rhs.x, y *= _rhs.y;	return *this; }
		//!
		Vector2& operator /= (const Vector2& _rhs) { x /= _rhs.x, y /= _rhs.y;	return *this; }
		//!
		Vector2& operator *= (float _rhs) { x *= _rhs, y *= _rhs;	return *this; }
		//!
		Vector2& operator /= (float _rhs) { x /= _rhs, y /= _rhs;	return *this; }
		//!
		friend Vector2 operator / (float _lhs, const Vector2& _rhs) { return Vector2(_lhs / _rhs.x, _lhs / _rhs.y); }
		//!
		friend Vector2 operator * (float _lhs, const Vector2& _rhs) { return Vector2(_lhs * _rhs.x, _lhs * _rhs.y); }

		//!
		bool operator == (const Vector2& _rhs) const { return x == _rhs.x && y == _rhs.y; }
		//!
		bool operator != (const Vector2& _rhs) const { return x != _rhs.x || y != _rhs.y; }

		union
		{
			struct
			{
				float x, y;
			};
			float v[2];
		};
	};

	//----------------------------------------------------------------------------//
	// Transform
	//----------------------------------------------------------------------------//

	struct Transform
	{
		float a = 1, b = 0, c = 0, d = 1; // 2x2
		float e = 0, f = 0; // tx, ty

							//!
		Transform(void) = default;
		//!
		Transform(float _x, float _y, float _scale, float _angle);

		//!
		Transform Copy(void) const { return *this; }
		//!
		Transform Inverse(void) const;

		//!
		Transform operator * (const Transform& _rhs) const;
		//!
		friend Vector2 operator * (const Transform& _lhs, const Vector2& _rhs);

		//!
		float Angle(void) const;
		//!
		float Scale(void) const;
		//!
		const Vector2& Pos(void) const;

		static const Transform Identity;
	};

	//----------------------------------------------------------------------------//
	// Rect
	//----------------------------------------------------------------------------//

	struct Rect
	{
		//!
		Rect() = default;
		//!
		Rect(float _left, float _top, float _right, float _bottom) : left(_left), top(_top), right(_right), bottom(_bottom) { }

		//!
		float Width(void) const { return right - left; }
		//!
		float Height(void) const { return bottom - top; }
		//!
		const Vector2 Size(void) const { return Vector2(Width(), Height()); }
		//!
		Rect& SetSize(const Vector2& _size) { return SetSize(_size.x, _size.y); }
		//!
		Rect& SetSize(float _width, float _height) { right = left + _width, bottom = top + _height; return *this; }

		//!
		float AbsWidth(void) const { return fabsf(right - left); }
		//!
		float AbsHeight(void) const { return fabsf(bottom - top); }
		//!
		Vector2 AbsSize(void) const { return Vector2(AbsWidth(), AbsHeight()); }

		//!
		const Vector2& Origin(void) const { return *reinterpret_cast<const Vector2*>(&left); }
		//!
		Rect& SetOrigin(const Vector2& _pos) { return SetOrigin(_pos.x, _pos.y); }
		//!
		Rect& SetOrigin(float _left, float _top)
		{
			Vector2 _size = Size();
			left = _left, top = _top;
			SetSize(_size);
			return *this;
		}

		//!
		Rect& FromSizePivot(const Vector2& _size, const Vector2& _pivot)
		{
			left = -_pivot.x;
			right = _size.x - _pivot.x;
			top = -_pivot.y;
			bottom = _size.y - _pivot.y;
			return *this;
		}

		//!
		const Vector2& Position(void) const { return *reinterpret_cast<const Vector2*>(&left); }
		//!
		Vector2& Position(void) { return *reinterpret_cast<Vector2*>(&left); }

		//!
		const Vector2& Position2(void) const { return *reinterpret_cast<const Vector2*>(&right); }
		//!
		Vector2& Position2(void) { return *reinterpret_cast<Vector2*>(&right); }

		//!
		Vector2 HalfSize(void) const { return Size() * .5f; }
		//!
		Vector2 Center(void) const { return Position() + HalfSize(); }

		//!
		bool IsValid(void) const { return left <= right && top <= bottom; }

		//!
		Rect operator + (const Vector2& _rhs) const { return Rect(left + _rhs.x, top + _rhs.y, right + _rhs.x, bottom + _rhs.y); }
		//!
		friend Rect operator + (const Vector2& _lhs, const Rect& _rhs) { return _rhs + _lhs; }
		//!
		Rect operator - (const Vector2& _rhs) const { return Rect(left - _rhs.x, top - _rhs.y, right - _rhs.x, bottom - _rhs.y); }
		//!
		friend Rect operator - (const Vector2& _lhs, const Rect& _rhs) { return _rhs - _lhs; }
		//!
		Rect operator * (const Vector2& _scale) const { return Rect(left * _scale.x, top * _scale.y, right * _scale.x, bottom * _scale.y); }
		//!
		friend Rect operator * (const Vector2& _lhs, const Rect& _rhs) { return _rhs * _lhs; }

		//!
		Rect operator + (const Rect& _rhs) const
		{
			return Rect(
				Min(left, _rhs.left),
				Min(top, _rhs.top),
				Max(right, _rhs.right),
				Max(bottom, _rhs.bottom));
		}

		//!
		Rect operator * (const Rect& _rhs) { return Rect(left * _rhs.left, top * _rhs.top, right * _rhs.right, bottom * _rhs.bottom); }

		//!
		bool operator == (const Rect& _rhs) const { return left == _rhs.left &&  top == _rhs.top && right == _rhs.right && bottom == _rhs.bottom; }
		//!
		bool operator != (const Rect& _rhs) const { return !(*this == _rhs); }

		//!
		float Proximity(const Rect& _other) const
		{
			float _x = left + right - (_other.left + _other.right);
			float _y = top + bottom - (_other.top + _other.bottom);
			return fabsf(_x) + fabsf(_y);
		}
		//!
		int Select(const Rect& _a, const Rect& _b) const { return Proximity(_a) < Proximity(_b) ? 0 : 1; }

		//!
		bool Contains(const Vector2& _test) const
		{
			return _test.x >= left && _test.x <= right && _test.y >= top && _test.y <= bottom;
		}
		//!
		bool Contains(const Rect& _test) const
		{
			return _test.left >= left && _test.top >= top && _test.right <= right && _test.bottom <= bottom;
		}
		//!
		bool Intersects(const Rect& _test) const
		{
			return !((_test.left < left && _test.top < top) || (_test.right > right && _test.bottom > bottom));
		}

		float left = 0, top = 0, right = 0, bottom = 0;

		static const Rect Zero;
		static const Rect Identity;
	};

	//----------------------------------------------------------------------------//
	// Line
	//----------------------------------------------------------------------------//

	struct Line
	{
		Vector2 start;
		Vector2 end;
	};

	//----------------------------------------------------------------------------//
	// Ray
	//----------------------------------------------------------------------------//

	struct Ray
	{
		Vector2 origin;
		Vector2 dir;
	};

	//----------------------------------------------------------------------------//
	// Bounds
	//----------------------------------------------------------------------------//

	//! OOBB
	struct Bounds
	{
		Vector2 center;
		Vector2 extends;
	};

	//----------------------------------------------------------------------------//
	// Circle
	//----------------------------------------------------------------------------//

	struct Circle
	{
		Vector2 center;
		float radius;
	};

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//

}
