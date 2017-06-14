#include <Easy2D.hpp>

using namespace Easy2D;

void main(void)
{
	Engine _engine;

	gEngine->SetVSync(false);

	while (gEngine->IsOpened())
	{
		gEngine->BeginFrame();

		gEngine->Clear(FrameBufferType::All, { .7f ,.7f , .75f , 1 });

		gEngine->EndFrame();

		if (gEngine->UserRequireExit())
			gEngine->RequireExit();
	}
}