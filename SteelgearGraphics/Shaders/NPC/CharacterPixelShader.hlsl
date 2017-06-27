struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Normal : NORMAL;
};

Texture2D txDiffuse : register(t0);
SamplerState sampAni : register(s0);

float4 PS_main(VS_OUT input) : SV_TARGET
{
	float4 texFloat = txDiffuse.Sample(sampAni, input.Tex);

	if (texFloat.w < 0.8)
		clip(-1);

	return texFloat;
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
}