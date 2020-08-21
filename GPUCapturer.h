
enum CaptureType
{
    Renderdoc,
    Pix
};

class GPUCapturer
{
public:
    static void Init(CaptureType type = Renderdoc);

    static void BeginCapture();
    static void EndCapture();
    static void ShowOverlay();
    static void HideOverlay();
};

#define GPU_BEGIN_CAPTURE GPUCapturer::BeginCapture()
#define GPU_END_CAPTURE GPUCapturer::EndCapture()