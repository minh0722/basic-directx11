cbuffer ConstBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
};

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

    output.pos = mul(world, inputVertex.pos);
    output.pos = mul(view, output.pos);
    output.pos = mul(proj, output.pos);

	output.color = inputVertex.color;

	return output;
}