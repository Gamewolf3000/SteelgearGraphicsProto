cbuffer VSConstants : register(b0)
{
	float4x4 WorldMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
}

cbuffer movement : register(b13)
{
	float4x4 movementMatrix;
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
	float4 Normal : NORMAL;

};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	float4x4 wvp = mul(mul(WorldMatrix, ViewMatrix), ProjectionMatrix);

	output.Pos = float4(input.Pos, 1);
	output.Pos = mul(mul(output.Pos, movementMatrix), wvp);

	output.Tex = input.Tex;

	output.Normal = float4(input.Normal, 0);

	return output;
}