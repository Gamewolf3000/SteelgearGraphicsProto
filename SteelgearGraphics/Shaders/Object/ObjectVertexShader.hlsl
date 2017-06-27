cbuffer VSConstants : register(b0)
{
	float4x4 WorldMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
}

cbuffer LightConstants : register(b1)
{
	float4x4 WorldLight;
	float4x4 ViewLight;
	float4x4 ProjectionLight;
}

cbuffer Position : register(b13)
{
	float4x4 PositionMatrix;
}


struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
	float4 Worldpos : WORLDPOS;
	float4 Lightpos : LIGHT;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = float4(input.Pos, 1);
	float4x4 world = mul(PositionMatrix, WorldMatrix);
	output.Worldpos = mul(output.Pos, world);
	output.Pos = mul(mul(mul(output.Pos, world), ViewMatrix), ProjectionMatrix);

	output.Lightpos = output.Worldpos;
	output.Lightpos = mul(mul(output.Lightpos, ViewLight), ProjectionLight);

	output.Tex = input.Tex;

	output.Normal = input.Normal;

	return output;
}