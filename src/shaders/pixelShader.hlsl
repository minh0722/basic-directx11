struct InputPixel
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

float4 main(InputPixel inputPixel) : SV_TARGET
{
	return inputPixel.color;
}