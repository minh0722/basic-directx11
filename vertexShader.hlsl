
struct VertexInput
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

struct PixelOutput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

PixelOutput main(VertexInput inputVertex)
{
	PixelOutput output;

	output.pos = inputVertex.pos;
	output.color = inputVertex.color;

	return output;
}