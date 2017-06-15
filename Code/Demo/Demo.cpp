#include <Easy2D.hpp>

using namespace Easy2D;

void main(void)
{
	Engine _engine;

	gEngine->SetVSync(false);

	{
		const char* _str = R"(
{
	"atlas" : "font\n",
	"symbols" : {
		"a" : [0, 0, 777, 10],
		"b" : [0, 10, 10, 20],
		"c" : [0, 10, 20, 50, 60, 70, 80, .1e-1, .5]
	},
	// comment
	"commands" : {
		"flipX" : false,
		"flipY" : false
	}		
}
		)";

		String _err;
		Json _js;
		if (!_js.Parse(_str, &_err))
			printf("%s\n", _err.c_str());
		printf("%s\n", _js.Print().c_str());

		int _val = _js["symbols"]["a"][2];
		printf("[symbols][a][2] = %d\n", _val);

		_js["symbols"]["a"][2] = 99;
		printf("[symbols][a][2] = %d\n", _js["symbols"]["a"][2].AsInt());
	}


	while (gEngine->IsOpened())
	{
		gEngine->BeginFrame();

		gEngine->Clear(FrameBufferType::All, { .7f ,.7f , .75f , 1 });

		gEngine->EndFrame();

		if (gEngine->UserRequireExit())
			gEngine->RequireExit();
	}
}