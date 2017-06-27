struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
	float4 Worldpos : WORLDPOS;
	float4 Lightpos : LIGHT;
};

Texture2D txDiffuse : register(t0);
Texture2D shadowTex : register(t13);
SamplerState sampAni : register(s0);
SamplerComparisonState testShadow : register(s1);

float2 texOffset(int u, int v)
{
	float size = 4096.0f;
	return float2(u * 1.0f / (size), v * 1.0f / (size));
}

float4 PS_main(VS_OUT input) : SV_Target
{
	float3 returnValue = float3(input.Tex.x, input.Tex.y, 0);


	//re-homogenize position after interpolation
	input.Lightpos.xyz /= input.Lightpos.w;

	//if position is not visible to the light - dont illuminate it
	//results in hard light frustum
	if (input.Lightpos.x < -1.0f || input.Lightpos.x > 1.0f ||
		input.Lightpos.y < -1.0f || input.Lightpos.y > 1.0f ||
		input.Lightpos.z < 0.0f || input.Lightpos.z > 1.0f) return float4(returnValue * 0.2, 1.0f);

	//transform clip space coords to texture space coords (-1:1 to 0:1)
	input.Lightpos.x = input.Lightpos.x / 2 + 0.5;
	input.Lightpos.y = input.Lightpos.y / -2 + 0.5;

	float shadowMapBias = 0.000035f;

	//apply shadow map bias
	input.Lightpos.z -= shadowMapBias;	//PCF sampling for shadow map
	float sum = 0;
	float x, y;

	//perform PCF filtering on a 8 x 8 texel neighborhood
	for (y = -3.5; y <= 3.5; y += 1.0)
	{
		for (x = -3.5; x <= 3.5; x += 1.0)
		{
			sum += shadowTex.SampleCmpLevelZero(testShadow, input.Lightpos.xy + texOffset(x, y), input.Lightpos.z);
		}
	}

	float shadowFactor = sum / 64.0;	return float4(returnValue * 0.2 + 0.8 * returnValue * shadowFactor, 1.0f);
	//return float4(input.Tex.x, input.Tex.y, 0, 1);
}