#include "Math.hpp"

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// Transform
	//----------------------------------------------------------------------------//

	const Transform Transform::Identity;

	//----------------------------------------------------------------------------//
	Transform::Transform(float _x, float _y, float _scale, float _angle)
	{
		float _c = cosf(_angle);
		float _s = sinf(_angle);

		a = _c * _scale;
		b = -_s * _scale;
		c = _s * _scale;
		d = _c * _scale;
		e = _x;
		f = _y;
	}
	//----------------------------------------------------------------------------//
	Transform Transform::Inverse(void) const
	{
		float _dt = 1 / (a * d - b * c);
		Transform _r;

		_r.a = d * _dt;
		_r.b = -b * _dt;
		_r.c = -c * _dt;
		_r.d = a * _dt;
		_r.e = (c * f - d * e) * _dt;
		_r.f = -(a * f - b * e) * _dt;

		return _r;
	}
	//----------------------------------------------------------------------------//
	Transform Transform::operator * (const Transform& _rhs) const
	{
		Transform _r;
		_r.a = a * _rhs.a + c * _rhs.b;
		_r.b = b * _rhs.a + d * _rhs.b;
		_r.c = a * _rhs.c + c * _rhs.d;
		_r.d = b * _rhs.c + d * _rhs.d;
		_r.e = a * _rhs.e + c * _rhs.f + e;
		_r.f = b * _rhs.e + d * _rhs.f + f;
		return _r;
	}
	//----------------------------------------------------------------------------//
	Vector2 operator * (const Transform& _lhs, const Vector2& _rhs)
	{
		return{ _rhs.x * _lhs.a + _rhs.y * _lhs.c + _lhs.e, _rhs.x * _lhs.b + _rhs.y * _lhs.d + _lhs.f };
	}
	//----------------------------------------------------------------------------//
	float Transform::Angle(void) const
	{
		float _scaleX = 1 / sqrtf(a * a + b * b);
		float _angle = atan2f(c * _scaleX, d * _scaleX);

		return fmodf(_angle + PI2, PI2);
	}
	//----------------------------------------------------------------------------//
	float Transform::Scale(void) const
	{
		return sqrtf(a * a + b * b); // x
	}
	//----------------------------------------------------------------------------//
	const Vector2& Transform::Pos(void) const
	{
		return *reinterpret_cast<const Vector2*>(&e);
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	// Rect
	//----------------------------------------------------------------------------//

	const Rect Rect::Zero(0, 0, 0, 0);
	const Rect Rect::Identity(0, 0, 1, 1);

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//
}
