#include <Easy2D.hpp>

using namespace Easy2D;

void main(void)
{
	Engine _engine;

	gEngine->SetVSync(false);

	{
		const char* _str = R"(
{
	"atlas" : "font",
	"symbols" : {
		"a" : [0, 0, 10, 10],
		"b" : [0, 10, 10, 20]
	},
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
		printf("Root size: %d\n", _js.Size());
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