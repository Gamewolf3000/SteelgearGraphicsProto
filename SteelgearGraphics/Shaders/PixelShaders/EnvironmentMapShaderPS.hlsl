
struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

Texture2D diffuse : register(t0);

SamplerState standardSamp : register(s0);

float4 PS_main(VS_OUT input) : SV_Target
{
	float4 texFloat = diffuse.Sample(standardSamp, input.Tex);
	return texFloat;
}