cbuffer WorldMatrix : register(b0)
{
	float4x4 WorldMatrix;
}

cbuffer CameraMatrices : register(b1)
{
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
}

cbuffer CameraPosition : register(b2)
{
	float4 camPos;
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
	float4 WorldPos : WORLDPOS;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;

	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
	float3 LocalPos : LOCALPOS;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.LocalPos = input.Pos;
	output.WorldPos = output.Pos = float4(input.Pos, 1);
	output.Pos = mul(mul(mul(output.Pos, WorldMatrix), ViewMatrix), ProjectionMatrix);
	output.WorldPos = mul(output.WorldPos, WorldMatrix);
	//output.Pos = mul(mul(output.Pos, ViewMatrix), ProjectionMatrix);

	output.Tex = input.Tex;

	output.Normal = normalize(mul(input.Normal, WorldMatrix));
	output.Tangent = normalize(mul(input.Tangent, WorldMatrix));
	output.Bitangent = normalize(mul(input.Bitangent, WorldMatrix));

	return output;
}