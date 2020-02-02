#include "api/app/renderdoc_app.h"

class Renderdoc
{
public:
	static void Init();

	static void BeginCapture();
	static void EndCapture();

private:
	static RENDERDOC_API_1_4_0* ms_renderdoc;
};

#define RENDERDOC_BEGIN_CAPTURE Renderdoc::BeginCapture()
#define RENDERDOC_END_CAPTURE Renderdoc::EndCapture()