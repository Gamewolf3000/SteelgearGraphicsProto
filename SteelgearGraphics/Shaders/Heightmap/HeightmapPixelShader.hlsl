cbuffer VSConstants : register(b13)
{
	float4 i;
}

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
	float4 Worldpos : WORLDPOS;
	float2 Blend : BLENDCOORD;
	float4 Lightpos : LIGHT;
};

Texture2D txDiffuse : register(t0);
Texture2D txDiffuse2 : register(t1);
Texture2D txDiffuse3 : register(t2);
Texture2D blendTx : register(t3);
Texture2D shadowTex : register(t13);
SamplerState sampAni : register(s0);
//SamplerState sampShadow  : register(s1);
SamplerComparisonState testShadow : register(s1);

float2 texOffset(int u, int v)
{
	float size = 4096.0f;
	return float2(u * 1.0f / (size), v * 1.0f / (size));
}

float4 PS_main(VS_OUT input) : SV_Target
{
	float3 texFloat = txDiffuse.Sample(sampAni, input.Tex).xyz;
	float3 texFloat2 = txDiffuse2.Sample(sampAni, input.Tex).xyz;
	float3 texFloat3 = txDiffuse3.Sample(sampAni, input.Tex).xyz;
	float3 blendTex = blendTx.Sample(sampAni, input.Blend).xyz;
	float3 blendedFloat3, returnValue;
	float temp1, temp2, temp3;
	float3 finalized[6];

	finalized[0] = finalized[3] = texFloat;


	// Determine how much of each texture should be used
	temp1 = blendTex.r / (blendTex.r + blendTex.g);
	temp2 = blendTex.g / (blendTex.r + blendTex.g);

	finalized[1] = blendedFloat3 = temp1*texFloat + temp2*texFloat2;


	// Determine how much of each texture should be used
	temp1 = blendTex.r / (blendTex.r + blendTex.g + blendTex.b);
	temp2 = blendTex.g / (blendTex.r + blendTex.g + blendTex.b);
	temp3 = blendTex.b / (blendTex.r + blendTex.g + blendTex.b);

	finalized[2] = temp1*texFloat + temp2*texFloat2 + temp3*texFloat3;

	returnValue = finalized[0] * i[0] + finalized[1] * i[1] + finalized[2] * i[2];
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

	float shadowMapBias = 0.0f;

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

	float shadowFactor = sum / 64.0;	return float4(returnValue * 0.2 + 0.8 * returnValue * shadowFactor, 1.0f);
}

//// Perspective divide to project the texture coords onto the shadow map
//input.Lightpos.xy /= input.Lightpos.w; // now in NDC

//// poslightH.xy is in [-1,1], but to sample the shadow map we
//// need [0,1], so scale by ½ and add ½
//float2 smTex = float2(0.5f*input.Lightpos.x + 0.5f, -0.5f*input.Lightpos.y + 0.5f);

//// Compute pixel depth for shadowing.
//float depth = input.Lightpos.z / input.Lightpos.w;
//// 2x2 percentage closest filter. POINT SAMPLING!
//float dx = 1.0f / 4096 * 2;//SMAP_SIZE; // size of one texture sample in the shadow map (width==height)
//float SHADOW_EPSILON = 0.00000001f;

//float s0 = (shadowTex.Sample(sampShadow, smTex).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
//float s1 = (shadowTex.Sample(sampShadow, smTex + float2(dx, 0.0f)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
//float s2 = (shadowTex.Sample(sampShadow, smTex + float2(0.0f, dx)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
//float s3 = (shadowTex.Sample(sampShadow, smTex + float2(dx, dx)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;

//// Transform shadow map UV coord to texel space
//float2 texelPos = smTex * 4096 * 2;// SMAP_SIZE;

//// Determine the lerp amount, if texelPos = (340.3, 200.1) then lerps = (0.3,0.1)
//float2 lerps = frac(texelPos);
//float shadowCoeff = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);
//float3 litColor = returnValue.rgb * shadowCoeff;