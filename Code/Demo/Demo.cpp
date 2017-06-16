#include <Easy2D.hpp>

using namespace Easy2D;

struct SpriteDesc
{
	SharedPtr<Texture> texture;
	Vector2 size;
	Vector2 pivot;
	Rect tc;

	void Draw(float _x, float _y, float _angle, float _xScale, float _yScale, uint _mode)
	{
		Vertex* _quad = gEngine->AddBatch(PrimitiveType::Quads, 4, texture, _mode);

		float _x1 = (size.x * pivot.x) * -_xScale;
		float _y1 = (size.y * pivot.y) * -_yScale;
		float _x2 = (size.x * (1 - pivot.x)) * _xScale;
		float _y2 = (size.y * (1 - pivot.y)) * _yScale;

		if (_angle != 0)
		{
			float _s = sinf(_angle);
			float _c = cosf(_angle);
			float _xc1 = _x1 * _c;
			float _yc1 = _y1 * _c;
			float _xc2 = _x2 * _c;
			float _yc2 = _y2 * _c;
			float _xs1 = _x1 * _s;
			float _ys1 = _y1 * _s;
			float _xs2 = _x2 * _s;
			float _ys2 = _y2 * _s;

			_quad[0].pos.x = _xc1 - _ys1 + _x;
			_quad[0].pos.y = _xs1 + _yc1 + _y;
			_quad[1].pos.x = _xc2 - _ys1 + _x;
			_quad[1].pos.y = _xs2 + _yc1 + _y;
			_quad[2].pos.x = _xc2 - _ys2 + _x;
			_quad[2].pos.y = _xs2 + _yc2 + _y;
			_quad[3].pos.x = _xc1 - _ys2 + _x;
			_quad[3].pos.y = _xs1 + _yc2 + _y;
		}
		else
		{
			_quad[0].pos.x = _x1 + _x;
			_quad[0].pos.y = _y1 + _y;
			_quad[1].pos.x = _x2 + _x;
			_quad[1].pos.y = _y1 + _y;
			_quad[2].pos.x = _x2 + _x;
			_quad[2].pos.y = _y2 + _y;
			_quad[3].pos.x = _x1 + _x;
			_quad[3].pos.y = _y2 + _y;
		}
		
		// TODO: texcoord

		float _z = 0;
		Color4ub _color = {0xff, 0, 0, 1}; // TODO
		for (uint i = 0; i < 4; ++i)
		{
			_quad[i].pos.z = _z;
			_quad[i].color = _color;
			_quad[i].color.g = 64 * i; // temp
		}
	}

};

void main(void)
{
	Engine _engine;

	gEngine->SetVSync(false);

	SpriteDesc _sprite;
	_sprite.pivot = { .5f, .5f };
	_sprite.size = { 16, 16 };

	while (gEngine->IsOpened())
	{
		gEngine->BeginFrame();

		gEngine->Clear(FrameBufferType::All, { .7f ,.7f , .75f , 1 });

		gEngine->Begin2D({ 0, 0 }, 1);

		srand(0);
		for (uint i = 0; i < 100000; ++i)
		{
			_sprite.Draw(rand() % gEngine->WindowSize().x, rand() % gEngine->WindowSize().y, .5f, 1, 1, 0);
		}	

		gEngine->EndFrame();

		if (gEngine->UserRequireExit())
			gEngine->RequireExit();
	}
}