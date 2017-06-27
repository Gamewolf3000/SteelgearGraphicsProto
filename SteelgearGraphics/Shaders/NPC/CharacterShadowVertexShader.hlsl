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
};

Texture2D txDiffuse : register(t0);
SamplerState sampAni : register(s0);

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)// : SV_POSITION
{
	//VS_OUT output = (VS_OUT)0;
	//float4x4 wvp = mul(mul(WorldLight, ViewLight), ProjectionLight);

	//output.Pos = float4(input.Pos, 1);
	//output.Pos = mul(mul(output.Pos, movementMatrix), wvp);

	//output.Tex = input.Tex;

	//return output;

	VS_OUT output = (VS_OUT)0;
	float4x4 wvp = mul(mul(WorldLight, ViewLight), ProjectionLight);
	float4 texFloat = txDiffuse.SampleLevel(sampAni, input.Tex, 0);

	output.Pos = float4(input.Pos, 1);
	output.Pos = mul(mul(output.Pos, movementMatrix), wvp);
	//output.Pos = mul(texFloat.w, output.Pos);

	output.Tex = input.Tex;

	return output;
}