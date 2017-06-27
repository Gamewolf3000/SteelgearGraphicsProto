cbuffer WorldMatrix : register(b0)
{
	float4x4 WorldMatrix;
}

cbuffer CameraMatrices : register(b1)
{
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
}

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;

	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = float4(input.Pos, 1);
	//output.Pos = mul(mul(mul(output.Pos, WorldMatrix), ViewMatrix), ProjectionMatrix);
	output.Tex = input.Tex;

	return output;
}