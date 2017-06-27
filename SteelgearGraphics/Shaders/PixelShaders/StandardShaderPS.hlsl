#define NROFLIGHTS 256

static const float PI = 3.14159265359f;

cbuffer LightData  : register(b0)
{
	float4 LightPos[NROFLIGHTS];
	float4 LightColour[NROFLIGHTS];
	float4 LightRangeType[NROFLIGHTS];
}

cbuffer CameraData  : register(b1)
{
	float4 CameraPos;
	float4 ViewVec;
}

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 WorldPos : WORLDPOS;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;

	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
	float3 LocalPos : LocalPos;
};

Texture2D diffuse : register(t0);
Texture2D roughness : register(t1);
Texture2D metallic : register(t2);
Texture2D normalMap : register(t3);
Texture2D displacementMap : register(t4);
TextureCube reflection : register(t5);
TextureCube irradiance : register(t6);

SamplerState standardSamp : register(s0);


float3 F_Schlick(float3 f0, float f90, float u)
{
	return f0 + (f90 - f0) * pow(1.0f - u, 5.0f);
}

float Fr_DisneyDiffuse(float NdotV, float NdotL, float LdotH, float linearRoughness)
{
	float energyBias = lerp(0, 0.5, linearRoughness);
	float energyFactor = lerp(1.0, 1.0 / 1.51, linearRoughness);
	float fd90 = energyBias + 2.0 * LdotH * LdotH * linearRoughness;
	float3 f0 = float3 (1.0f, 1.0f, 1.0f);

	float lightScatter = F_Schlick(f0, fd90, NdotL).r;
	float viewScatter = F_Schlick(f0, fd90, NdotV).r;

	return lightScatter * viewScatter * energyFactor;
}

float V_SmithGGXCorrelated(float NdotV, float NdotL, float alphaG) {

	// Original formulation of G_SmithGGX Correlated
	// lambda_v = (-1 + sqrt ( alphaG2 * (1 - NdotL2 ) / NdotL2 + 1)) * 0.5f;
	// lambda_l = (-1 + sqrt ( alphaG2 * (1 - NdotV2 ) / NdotV2 + 1)) * 0.5f;
	// G_SmithGGXCorrelated = 1 / (1 + lambda_v + lambda_l );
	// V_SmithGGXCorrelated = G_SmithGGXCorrelated / (4.0 f * NdotL * NdotV );

	float alphaG2 = alphaG * alphaG;
	//Caution: the "NdotL*" and "NdotV *" are explicitely inversed, this is not a mistake
	float Lambda_GGXV = NdotL * sqrt((-NdotV * alphaG2 + NdotV) * NdotV + alphaG2);
	float Lambda_GGXL = NdotV * sqrt((-NdotL * alphaG2 + NdotL) * NdotL + alphaG2);

	return 0.5f / (Lambda_GGXV + Lambda_GGXL);
}

float D_GGX(float NdotH, float m)
{
	// Divide by PI is apply later
	float m2 = m * m;
	float f = (NdotH * m2 - NdotH) * NdotH + 1.0f;

	return m2 / (f * f);
}


float CalcFr(float3 f0, float f90, float LdotH, float NdotV, float NdotL, float roughness, float NdotH, inout float3 DFV)
{
	float3 F = F_Schlick(f0, f90, LdotH);
	float Vis = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
	float D = D_GGX(NdotH, roughness);
	float Fr = D * F * Vis / PI;	DFV.x = D;	DFV.y = F;	DFV.z = Vis;	return Fr;
}

float3 CalcBumpedNormal(VS_OUT indata)
{
	float3 Normal = normalize(indata.Normal);
	float3 Tangent = normalize(indata.Tangent);
	Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
	float3 Bitangent = normalize(cross(Tangent, Normal));
	float3 BumpMapNormal = normalMap.Sample(standardSamp, indata.Tex).xyz;  //texture(gNormalMap, TexCoord0).xyz;
	//return BumpMapNormal;
	BumpMapNormal = normalize(2.0 * BumpMapNormal - float3(1.0, 1.0, 1.0));
	float3 NewNormal;
	float3x3 TBN = float3x3(Tangent, Bitangent, Normal);
	NewNormal = mul(BumpMapNormal,TBN);
	NewNormal = normalize(NewNormal);
	return NewNormal;
}

float3 ReadNormalMap(float3 normal, float3 tangent, float2 uv)
{
	normal = normalize(normal);
	tangent = normalize(tangent);
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	float3 bitangent = cross(tangent, normal);
	float3 mapNormal = normalMap.Sample(standardSamp, uv).xyz;
	mapNormal = 2.0f * mapNormal - 1.0f;
	float3x3 tbn = float3x3(tangent, bitangent, normal);
	return normalize(mul(mapNormal, tbn)).xyz;
}

float3 FresnelSchlickWithRoughness(float3 SpecularColor, float3 E, float3 N, float Gloss)
{
	return SpecularColor + (max(Gloss, SpecularColor) - SpecularColor) * pow(1 - saturate(dot(E, N)), 5);
}
float3 SpecularEnvmap(float3 E, float3 N, float3 R, float3 SpecularColor, float Gloss)
{
	//float3 Envcolor = texCUBElod(EnvironmentTexture, float4(R, EnvMapMipmapScaleBias.x * Gloss + EnvMapMipmapScaleBias.y)).rgb;
	float3 Envcolor = reflection.Sample(standardSamp, R).rgb;
	return FresnelSchlickWithRoughness(SpecularColor, E, N, Gloss) * Envcolor.rgb; // * EnvMapScaleAndModulate; // EnvMapScaleAndModulate is used to decompress range
}





// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float radicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float2 Hammersley(uint i, uint N)
{
	return float2(float(i) / float(N), radicalInverse_VdC(i));
}

// http://graphicrants.blogspot.com.au/2013/08/specular-brdf-reference.html
float GGX(float nDotV, float a)
{
	float aa = a*a;
	float oneMinusAa = 1 - aa;
	float nDotV2 = 2 * nDotV;
	float root = aa + oneMinusAa * nDotV * nDotV;
	return nDotV2 / (nDotV + sqrt(root));
}

// http://graphicrants.blogspot.com.au/2013/08/specular-brdf-reference.html
float G_Smith(float a, float nDotV, float nDotL)
{
	return GGX(nDotL, a) * GGX(nDotV, a);
}

float3 ImportanceSampleGGX(float2 Xi, float Roughness, float3 N)
{
	float a = Roughness * Roughness;
	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt((1 - Xi.y) / (1 + (a*a - 1) * Xi.y));
	float SinTheta = sqrt(1 - CosTheta * CosTheta);
	float3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;
	float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, N));
	float3 TangentY = cross(N, TangentX);
	// Tangent to world space
	return TangentX * H.x + TangentY * H.y + N * H.z;
}

float3 PrefilterEnvMap(float Roughness, float3 R)
{
	float3 N = R;
	float3 V = R;
	float3 PrefilteredColor = 0;
	float TotalWeight = 0; // this was missing in original notes?
	const uint NumSamples = 1;
	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 H = ImportanceSampleGGX(Xi, Roughness, N);
		float3 L = 2 * dot(V, H) * H - V;
		float NoL = saturate(dot(N, L));
		if (NoL > 0)
		{
			PrefilteredColor += reflection.SampleLevel(standardSamp, L, 0).rgb * NoL;
			TotalWeight += NoL;
		}
	}
	return PrefilteredColor / TotalWeight;
}

float2 IntegrateBRDF(float Roughness, float NoV)
{
	float3 V;
	V.x = sqrt(1.0f - NoV * NoV); // sin
	V.y = 0;
	V.z = NoV; // cos
	float A = 0;
	float B = 0;
	float3 N = float3(0.0f, 0.0f, 1.0f); // this was missing, but maybe we should send in N?
	const uint NumSamples = 1024;
	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 H = ImportanceSampleGGX(Xi, Roughness, N);
		float3 L = 2 * dot(V, H) * H - V;
		float NoL = saturate(L.z);
		float NoH = saturate(H.z);
		float VoH = saturate(dot(V, H));
		if (NoL > 0)
		{
			float G = G_Smith(Roughness, NoV, NoL);
			float G_Vis = G * VoH / (NoH * NoV);
			float Fc = pow(1 - VoH, 5);
			A += (1 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}
	return float2(A, B) / NumSamples;
}

float3 ApproximateSpecularIBL(float3 SpecularColor, float Roughness, float3 N, float3 V)
{
	float NoV = saturate(dot(N, V));
	float3 R = 2 * dot(V, N) * N - V;
	float3 PrefilteredColor = PrefilterEnvMap(Roughness, R);
	float2 EnvBRDF = IntegrateBRDF(Roughness, NoV);
	return PrefilteredColor * (SpecularColor * EnvBRDF.x + EnvBRDF.y);
}



float G_SmithMarmo(float a2, float Cos)
{
	return (2.0 * Cos) / (Cos + sqrt(a2 + (1.0 - a2) * (Cos * Cos)));
}

float3 ImportanceSampleGGXMarmo(float2 Xi, float Roughness)
{
	float a = Roughness * Roughness;
	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt((1 - Xi.y) / (1 + (a*a - 1) * Xi.y));
	float SinTheta = sqrt(1 - CosTheta * CosTheta);

	return float3(cos(Phi) * SinTheta, sin(Phi) * SinTheta, CosTheta);

	//float3 H;
	//H.x = SinTheta * cos(Phi);
	//H.y = SinTheta * sin(Phi);
	//H.z = CosTheta;
	//float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
	//float3 TangentX = normalize(cross(UpVector, N));
	//float3 TangentY = cross(N, TangentX);
	//// Tangent to world space
	//return TangentX * H.x + TangentY * H.y + N * H.z;
}

float3 ReflectionMarmo(float roughness, float3 normal, float3 view, float metalness /*instead of reflectivity? check marmo metallic shader*/, float3 fresnel /*they seem to be using the (1.0f, 1.0f, 1.0f) version, try both*/)
{
	//float roughness = 1.0 - s.gloss;

	float a = max(roughness * roughness, 5e-4);
	float a2 = a*a;
	float k = a * 0.5;

	float3 basisX = normalize(cross(normal, float3(0.0, 1.0, saturate(normal.y*10000.0 - 9999.0))));
	float3 basisY = cross(basisX, normal);
	float3 basisZ = normal;

	//vec3 reflectivity = s.reflectivity, fresn = s.fresnel;
	float3 reflectivity = 0.16f * metalness * metalness;

	float3 spec = float3(0.0, 0.0, 0.0);
	
	const int numSamples = 20;
	for (int i = 0; i<numSamples; i++)
	{
		float3 dir = ImportanceSampleGGXMarmo(Hammersley(i, numSamples), roughness);
		float3 H = dir.x*basisX + dir.y*basisY + dir.z*basisZ;
		float3 L = (2.0 * dot(view, H)) * H - view;

		float NdotH = saturate(dot(normal, H));
		float NdotL = saturate(dot(normal, L));
		float NdotV = saturate(dot(normal, view));
		float VdotH = saturate(dot(view, H));

		float d = (NdotH * a2 - NdotH) * NdotH + 1.0;
		float pdf = (NdotH * a2) / (4.0 * PI * d*d * VdotH);

		float lod = (0.5 * log2((256.0*256.0) / float(numSamples)) + 1.5*(1.0f - roughness)*(1.0f - roughness)) - 0.5*log2(pdf);
		//vec3 sampleCol = textureCubeLod(tReflectionCubeMap, L, lod).xyz;
		float3 sampleCol = reflection.SampleLevel(standardSamp, L, lod).rgb;

		float G = (G_SmithMarmo(a2, NdotL) * G_SmithMarmo(a2, max(1e-8, NdotV)));
		G *= VdotH / (NdotH * max(1e-8, NdotV));
		//float3 F = fresnel(VdotH, reflectivity, fresn);
		float3 F = F_Schlick( fresnel, reflectivity, VdotH);

		//spec += sampleCol * F * (G * (1.0 / float(numSamples)));
		spec += sampleCol * F * G * VdotH / (NdotH * NdotV);
	}

	//return spec;
	return spec / float(numSamples);
}

// Lys constants
static const float k0 = 0.00098, k1 = 0.9921, fUserMaxSPow = 0.2425;
static const float g_fMaxT = (exp2(-10.0 / sqrt(fUserMaxSPow)) - k0) / k1;
static const int nMipOffset = 0;

// Lys function, copy/pasted from https://www.knaldtech.com/docs/doku.php?id=specular_lys
float GetSpecPowToMip(float fSpecPow, int nMips)
{
    // This line was added because ShaderTool destroys mipmaps.
    fSpecPow = 1 - pow(1 - fSpecPow, 8);
    // Default curve - Inverse of Toolbag 2 curve with adjusted constants.
    float fSmulMaxT = ( exp2(-10.0/sqrt( fSpecPow )) - k0)/k1;
    return float(nMips-1-nMipOffset)*(1.0 - clamp( fSmulMaxT/g_fMaxT, 0.0, 1.0 ));
}


//http://shdr.bkcore.com/#1/hVZLc9s2EP4rGJ0km5IoP5o0inOoO+klcTPjTnJoOhmYBCVkSIAFSEl2Jv+9u3iQAElND6bF3Q+73z7BH7NcZm3FRKNnb/6e1YplXHMpyJ7v9jUpSkmb7Vf1VbSCF1JVVkIaXrFIfGDZFVFMy7Jt4LjRHah65mKHumtSfJKan1E9gAVaGgX+rdeZFLqhwAlfCbFPI3T+C3AlWPlJHpkid+R6lW7HIF2zrC2p+q1kLAdUunp1ewYGhgCwWZ3T30tWGAs3HmGJFq3ItOft4LRpFW0Ymdv30+KHPcGL+Ym8AyfpAjIFIIG/t4HyLTjolWmndIKTef3p3fUu/2DNF8gWyx9LWbPHf1XufOeHe6m9fycSIDKxpuTSApwXr7H/1+Sq829P7r3OSffOyhp+LYNQHNt9zNba+Eu17NGVJeSYOB9Ktru9YHpAGl1NBGnDi0hisQDc2QF28/7lkuw93AAv7kgtj3OIFCLoYAk2woJcdJWPA8MTqEl85yziSA+S56SiXMwX+OoCWa+ZOHAlBc5a2NZmBEqYtuZ3jr2M7/PlJrlKbhfbELheV9BWilOgoCAFjeJMjyw9Uc3uZSk7U5sE+v41PK5jczZftHriQMj0dhrXe7eXuvmAxKz2agsUjorWNUxTzoui1YwcebMnXByY0iA1YaDbRqqxLzzjJxY5oT3NK+gFmJY/FRPLB/pMJw76QebNszn6a8SzL6+h+UukrFhDy5JnE+fcDnlswPOucd08yDjsw5oqRqjIiTBbir8wAtuL06dyIvsWA6Y68NxtN5/8oNI9xgs9yNg6cHYMWiJN0mQz7AioNsTW8SGwi0klgXAmqxoFGS0xdbh49TixpZt42XQUEkdrqllcxtwJi0s8z6kDWLj33SG/GefOTjiKPQrHa2MGMpAn0cafOgijPChoxCduIWs/cONUOPRj56Mx7BN8+p8EY8uHkZfBwnI6jBd/Jv18RIgLTzfguR01nmJFCWPH8g99fy2d8Gxto5WJJR1ZOVNddyTa5nYl9gt8sGfPse85+GvaJAlWdXR9L3CR2zsrlgduvIEu251wioxRTFAxaw9pgLelqRc0Rbq6hWe/EQcdUXABU5/h2h1VJguXMQ5x6tlYzeVdsLOh/bDoI4QlHgH9ZjsDdW3sUSOLfvEHRocQm4kxwD535bf3iu6Cu+Zmbo4m5hPH3omz5NwnpaANXGBPLax+k6a6+zwcaoT7Ouy+NeEa9NKPFKCnWHcDGzBn5Wdo3Gk13J7focvBWaef/h49+wUr4lse7nhsn4fx8g9ZQia78bPNcYNRA37AF4Ammz4lPqHow3Mwi0OvTs8vW1eMUDMIEAzW+NkG9fgnmRlvszf2v17r9oUKwb5hdVbf9eznfw==

//constants

static const float fresnelPower = 3.0;
static const float specularBleed = 0.75;
static const float specPow = 1.5;
static const float specCoef = 0.45;

//float saturate(float x) {
//	if (x > 1.0) return 1.0;
//	if (x < 0.0) return 0.0;
//	return x;
//}

float GetWantedSlopeSqrd(float dvCos) {
	float dnCos = 1.0 + dvCos;
	dnCos = dnCos / 2.0;
	float h = dnCos;
	h = 1.0 / h - 1.0;
	return h;
}

float TrueSpecular(float dvCos, float roughness) {
	float h = GetWantedSlopeSqrd(dvCos);
	float spec = roughness / (roughness + h);
	spec *= pow(2.0 - roughness, 1.5) * specCoef;
	return pow(spec, specPow);
}

float3 Cook_TorranceSpecular(float3 n, float3 v, float3 l, float3 h, float roughness, float3 f0)
{
	float NdotV = max(dot(n, v), 0.0f);
	float NdotL = max(dot(n, l), 0.0f);
	float LdotH = max(dot(l, h), 0.0f);
	float NdotH = max(dot(n, h), 0.0f);
	float VdotH = max(dot(v, h), 0.0f);

	// F
	float3 F = F_Schlick(f0, 1.0f, VdotH);

	// D
	//float roughness2 = roughness * roughness;
	//float d1 = 1.0f / (4.0f * roughness2 * pow(NdotH, 4.0f));
	//float d2 = (NdotH * NdotH - 1.0f) / (roughness2 * NdotH * NdotH);
	//float D = (d1 * exp(d2)) / NdotL;
	float D = D_GGX(NdotH, roughness);

	// G
	float g1 = (2.0f * (NdotH * NdotV)) / VdotH;
	float g2 = (2.0f * (NdotH * NdotL)) / VdotH;
	float G = min(1.0f, min(g1, g2));

	return (F * D * G) / (4 * NdotV * NdotL);
}

//http://orsvarn.com/post/pbr-shader-source/
float4 PS_main(VS_OUT input) : SV_Target
{
	
	//float3 n = CalcBumpedNormal(input);
	////n = normalize(input.normal.xyz);
	//float3 l = normalize(LightPos[0].xyz - input.WorldPos.xyz);
	//float3 v = normalize(CameraPos.xyz - input.WorldPos.xyz);
	////v = normalize(camDirection);
	//float3 h = normalize(v + l);
	//float NdotV = saturate(dot(n, v));
	//float NdotL = saturate(dot(n, l));
	//float LdotH = saturate(dot(l, h));
	//float NdotH = saturate(dot(n, h));

	//float3 radiance = reflection.Sample(standardSamp, -reflect(v,n));
	//float3 irr = irradiance.Sample(standardSamp, input.WorldPos.xyz);
	//float4 albedo = diffuse.Sample(standardSamp, input.Tex);
	//float rough = roughness.Sample(standardSamp, input.Tex).r;
	//float metal = metallic.Sample(standardSamp, input.Tex).r;
	////float ao = ambientocclusionmap.Sample(samp, input.tex).r;
	////float ior = 1.5f; //Plastic 1.46, Diamond 2.417

	////albedo = float4(0.5f, 0.5f, 0.5f, 1.0f) /** radience.Sample(samp, vr)*/;
	////roughness = 0.03f;
	////metallic = 1.0f;
	////f0 = 0.8f;
	////diffuseColor = albedo.rgb;
	////specularColor = albedo.rgb;

	//float3 diffuseColor = lerp(albedo.rgb, 0.0f, metal);
	//float3 specularColor = lerp(0.04, albedo.rgb, metal);
	//float lightIntensity = max(1.0f - (length(LightPos[0].xyz - input.WorldPos.xyz) / LightRangeType[0].x), 0.0f);

	//// Gamma correction
	//radiance = pow(radiance, 2.2f);
	//irr = pow(irr, 2.2f);
	//diffuseColor = pow(albedo, 2.2f);
	//rough = pow(rough, 2.2f);
	//metal= pow(metal, 2.2f);

	//// Diffuse
	//float3 lambertD = (NdotL * diffuseColor * LightColour[0].rgb * lightIntensity) / PI;

	//// Ambient
	//lambertD += irr * diffuseColor;

	//// Ambient specular
	//float3 fresnel = F_Schlick(specularColor, 1.0f, NdotV);

	//// Specular
	//float3 Cook_TorranceS = float3(0.0f, 0.0f, 0.0f);
	//if (NdotL > 0.0f)
	//	Cook_TorranceS = Cook_TorranceSpecular(n, v, l, h, rough, specularColor) * specularColor * LightColour[0].rgb * lightIntensity;

	//// Composit
	//float3 output = float3(0.0f, 0.0f, 0.0f);
	//output = lerp(lambertD, radiance, fresnel);
	//output += Cook_TorranceS;
	//output = pow(output, 1.0f / 2.2f);

	//return float4(output, 1.0f);

	////----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//environment

	//float3 lightDir = normalize(LightPos[0].xyz - input.WorldPos.xyz);

	////materian propetries

	//float3 baseColor = diffuse.Sample(standardSamp, input.Tex);

	//float ambient = 0.00;
	//float ghostLight = 0.02; //wrapped diffuse with inversed light vector

	//float diffPower = 0.78; //simulate Oren-Nayar

	//float specularity = 0.9;
	//float roughnessT = abs(roughness.Sample(standardSamp, input.Tex).r);
	//float metallicT = metallic.Sample(standardSamp, input.Tex).r;
	//float fresnelStrength = 1.0;

	////radiance = pow(radiance, 2.2f);
	////irr = pow(irr, 2.2f);
	//baseColor = pow(baseColor, 2.2f);
	//roughnessT = pow(roughnessT, 2.2f);
	//metallicT = pow(metallicT, 2.2f);

	////prepare and normalize variables

	//float3 normal = CalcBumpedNormal(input);
	////lightDir = normalize(lightDir);
	//float3 camToPixelRaw = CameraPos - input.WorldPos;
	//float3 viewDir = normalize(camToPixelRaw);

	////float testDot = dot(input.Normal, viewDir);
	////return float4(testDot, testDot, testDot, 1.0f);

	////basic variables for more comples calculations

	//float lCos = dot(lightDir, normal);

	//float fresnel = dot(normal, viewDir);

	//float specFresnel = saturate(fresnel);
	//specFresnel = pow(1.0 - specFresnel, fresnelPower);
	//specFresnel *= fresnelStrength;

	//specularity = 1.0 - (1.0 - specularity) * (1.0 - specFresnel);

	////more complex calculations

	//float diff = saturate(lCos);
	//diff = pow(diff, diffPower);
	//diff *= 1.0 - specularity;

	//float3 reflectedLightDir = -reflect(lightDir, normal);
	//float spec = dot(reflectedLightDir, viewDir);

	//spec = TrueSpecular(spec, roughnessT);
	//spec *= specularity;

	//float specBleed = (lCos + specularBleed) / (1.0 + specularBleed);
	//specBleed = saturate(specBleed);
	//spec *= specBleed;

	//float ghost = (1.0 - lCos) * 0.5 * ghostLight;

	////// Ambient cubemap light
	//float4 cubemapSampleAmbient = irradiance.Sample(standardSamp, normal);
	////float3 lightColour = LightColour[0].rgb;
	////float lightIntensity = max(1.0f - (length(LightPos[0].xyz - input.WorldPos.xyz) / LightRangeType[0].x), 0.0f);
	////return float4(cubemapSampleAmbient.rgb, 1.0);
	////diff += cubemapSampleAmbient.rgb * baseColor.rgb;
	//float4 specularColour = float4(lerp(0.04f.rrr, baseColor.rgb, metallicT), 1.0f);

	////final color

	//float3 color = float3(0,0,0);
	//color += baseColor * (diff);
	//color += spec * baseColor * metallicT;
	//color += spec * (1.0 - metallicT) * specularColour;
	//color += ambient * baseColor;
	//color += ghost * baseColor;
	//color = pow(color, 0.4545f);

	//return float4(color, 1.0);

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//return float4(irradiance.SampleLevel(standardSamp, normalize(input.LocalPos), 0.0f).rgb , 1.0f);

	float3 camToPixelRaw = CameraPos - input.WorldPos;
	float3 camToPixel = normalize(camToPixelRaw);

	//float testDot = dot(input.Normal, camToPixel);
	//return float4(testDot, testDot, testDot, 1.0f);

	float3 normal = CalcBumpedNormal(input);
	float3 camNormalReflect = reflect(camToPixel, normal);

	int mipLevels, width, height;
	reflection.GetDimensions(0, width, height, mipLevels);
	float4 baseColourSample = diffuse.Sample(standardSamp, input.Tex);

	float roughnessSample = abs(roughness.Sample(standardSamp, input.Tex).r);
	float metallicSample = metallic.Sample(standardSamp, input.Tex).r;

	float4 cubemapSampleAmbient = irradiance.Sample(standardSamp, normal);
	float4 cubemapSampleSpec = reflection.SampleLevel(standardSamp, -camNormalReflect, GetSpecPowToMip(roughnessSample, mipLevels));
	roughnessSample = pow(roughnessSample, 0.4);

	// Creating specular colour and intensity, this needs to be done before gamma correction
	float4 specularColour = float4(lerp(0.04f.rrr, baseColourSample.rgb, metallicSample), 1.0f);
	baseColourSample.rgb = lerp(baseColourSample.rgb, 0.0f.rrr, metallicSample);

	// Gamma correct textures
	baseColourSample = pow(abs(baseColourSample), 2.2);
	roughnessSample = pow(abs(roughnessSample), 2.2);
	cubemapSampleAmbient = pow(abs(cubemapSampleAmbient), 2.2);
	cubemapSampleSpec = pow(abs(cubemapSampleSpec), 2.2);

	// Material settings
	float roughPow2 = roughnessSample*roughnessSample;
	float roughPow22 = roughPow2*roughPow2;
	float roughPow2Half = roughPow2*0.5;

	float4 diffuseLight = 0;
	float4 specularLight = 0;

	for (int i = 0; i < 1; i++)
	{
		//float3 lightDir = normalize(LightPos[i].xyz - input.WorldPos.xyz);
		float3 lightDir = normalize(LightPos[i].xyz - input.WorldPos.xyz);
		float3 lightColour = LightColour[i].rgb;
		float lightIntensity = max(1.0f - (length(LightPos[i].xyz - input.WorldPos.xyz) / LightRangeType[0].x), 0.0f);

		// DIFFUSE
		diffuseLight += float4((saturate(dot(-lightDir, normal))*(1 / PI)) * lightColour*lightIntensity*baseColourSample.rgb, 1.0f);

		// SPECULAR
		float3 halfVector = normalize(camToPixel + lightDir);
		float halfVecDotNorm = dot(halfVector, normal);
		float normDotLight = max(dot(normal, -lightDir), 0);
		float normDotCam = max(dot(normal, -camToPixel), 0);

		// Fresnel term
		float4 schlickFresnel = specularColour + (1 - specularColour) * (pow(1 - dot(camToPixel, halfVector), 5) / (6 - 5 * (1 - roughnessSample)));

		// Distribution term
		float denominator = halfVecDotNorm*halfVecDotNorm*(roughPow22 - 1) + 1;
		float ggxDistribution = roughPow22 / (PI*denominator*denominator);

		// Geometry term
		float schlickGgxGeometry = (normDotCam / (normDotCam*(1 - roughPow2Half) + roughPow2Half));
		//float ggxGeometry = (2*normDotCam) / (normDotCam + sqrt(roughPow2 +
		//       (1 - roughPow2)*pow(normDotCam, 2))); // ggxG without schlick approximation

		// Add the spec from this light
		specularLight += float4(((schlickFresnel*ggxDistribution*schlickGgxGeometry) / 4 * normDotLight*normDotCam).rrr * lightColour*specularColour.rgb*lightIntensity, 1.0f);
	}

	// Ambient cubemap light
	diffuseLight.rgb += cubemapSampleAmbient.rgb * baseColourSample.rgb;

	// Specular cubemap light
	float normDotCam = max(dot(lerp(input.Normal, normal, max(dot(input.Normal, -camToPixel), 0)), -camToPixel), 0);
	normDotCam = dot(normal, camToPixel);
	//return float4(normDotCam.rrr, 1.0);
	float4 schlickFresnel = saturate(specularColour + (1 - specularColour) * pow(1 - normDotCam, 5));
	//return float4(schlickFresnel.w, schlickFresnel.w, schlickFresnel.w, 1.0f);
	//return float4(schlickFresnel.x, schlickFresnel.x, schlickFresnel.x, 1.0f);

	// Composite
	float4 output = 0;
	output = lerp(diffuseLight, cubemapSampleSpec, schlickFresnel);
	output += specularLight;

	output = pow(abs(output), .4545);
	output.a = 1;
	//return float4(normDotCam, normDotCam, normDotCam, 1.0f);
	//return schlickFresnel;
	return output;
	//return float4(output.x, output.y, output.z, 1.0f);

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	////return float4(reflection.SampleLevel(standardSamp, float3(input.Tex.x, input.Tex.y, 1.0f), 0.0f).rgb , 1.0f);
	////float4 texFloat = diffuse.Sample(standardSamp, input.Tex);
	////return texFloat;

	////uint nr = 2415919104;
	////for (int p = 0; p < 1024; p++)
	////{
	////	for (int i = 0; i < nr; i++)
	////	{
	////		counter = i;
	////	}
	////}

	////---------------------------------------------------------------------------------------------------------------------------------

	//float EPSILON = 0.00001f; // Vet inte riktigt vad det här värdet borde vara, men hittade detta värde i orginal skriften och tror den ska funka
	////float3 f0 = (1.0f, 1.0f, 1.0f); // tagen ur Fr_DisneyDiffuse, ska den vara samma i båda beräkningarna?
	//float3 LightVec = LightPos[0].xyz - input.WorldPos.xyz; // Ändra detta så alla ljus räknas in och inte bara första (Detta är min L vektor)
	//float dist = length(LightVec);
	//float LightPower = max(1.0f - (dist / LightRangeType[0].x), 0.0f); // Lägga till så att ljus bara räknas med om man är mindre än 90 graders vinkel från det?
	//float3 calculatedNormal = /*input.Normal;*/ CalcBumpedNormal(input); // ändra till = input.Normal; för att skippa normal mappingen
	//LightVec = normalize(LightVec); //Borde den normaliseras?

	//LightVec = float3(0.0f, 1.0f, 0.0f); // top down hardcoded directional light
	//LightPower = 1.0f;

	//float3 V = normalize(CameraPos.xyz - input.WorldPos.xyz); // borde den inte normaliseras? Ska denna användas?
	////float3 V = normalize(ViewVec.xyz); // Är detta eller ovanstående rätt?
	//float NdotV = abs(dot(calculatedNormal, V)) + EPSILON; // avoid artifact (varför bara här, och varför är den annorlunda jämfört med de andra dot produkterna?)
	//float3 H = normalize(V + LightVec);
	//float LdotH = saturate(dot(LightVec, H));
	//float NdotH = saturate(dot(calculatedNormal, H));
	//float NdotL = saturate(dot(calculatedNormal, LightVec));

	////return float4(V.xyz, 1.0f);

	//float3 baseColor = diffuse.Sample(standardSamp, input.Tex).rgb;
	//float metalness = metallic.Sample(standardSamp, input.Tex).r;

	////roughness same for both diffuse and specular, as in Frostbite
	//float linearRoughness = saturate(roughness.Sample(standardSamp, input.Tex).r + EPSILON);
	//float roughness = pow(linearRoughness, 2);

	////if (metalness < 0.00001 && metalness > -0.00001 && linearRoughness < 1.00001 && linearRoughness > 0.99999)
	////	return float4(1.0f, 0.0f, 0.0f, 1.0f);

	//float3 diffuseColor = lerp(baseColor.rgb, 0.0f.rrr, metalness.r);
	////float3 diffuseColor = (1 - metalness.r) * baseColor; // Hittade denna versionen i orginal skriften
	//float3 f0 = lerp(0.03f.rrr, baseColor.rgb, metalness.r);
	//float3 specularColor = lerp(f0, baseColor.rgb, metalness.r);
	////float3 specularColor_f0 = lerp(0.04f.rrr, baseColor.rgb, metalness.r);

	//float3 ref = ReflectionMarmo(roughness, calculatedNormal, V, metalness, f0/*float3(1.0f, 1.0f, 1.0f)*/);

	////return float4(ref, 1.0f);

	////Calculate fd (diffuse)

	//float fd = Fr_DisneyDiffuse(NdotV, NdotL, LdotH, linearRoughness) / PI;

	//float3 diffuse = fd.xxx * LightColour[0].xyz * LightPower * diffuseColor;

	////Calculate the fr (specular)

	//float3 DFV = float3(0.0f, 0.0f, 0.0f);

	//float f90 = 0.16f * metalness * metalness;
	//float fr = CalcFr(f0, f90, LdotH, NdotV, NdotL, roughness, NdotH, DFV);
	////float3 se = SpecularEnvmap(CameraPos, calculatedNormal, reflectionVector, specularColor, 0.15f);
	////return float4(se, 1.0f);

	////float3 incident = -V;
	////float3 reflectionVector = reflect(incident, input.Normal);
	////float3 Envcolor = irradiance.Sample(standardSamp, reflectionVector).rgb;
	////float3 Envcolor = reflection.SampleLevel(standardSamp, reflectionVector, 0).rgb;
	////return float4(Envcolor, 1.0f);
	////float3 R = 2 * dot(V, calculatedNormal) * calculatedNormal - V;
	////return float4(PrefilterEnvMap(roughness, R), 1.0f);
	////float3 specular = ApproximateSpecularIBL(specularColor, linearRoughness, calculatedNormal, V) * LightColour[0].xyz * LightPower;
	//float3 specular = fr.xxx * LightColour[0].xyz * LightPower;// *specularColor;
	////return float4(specular, 1.0f);
	////specular += ref;

	////------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	////diffuse.rgb += irradiance.Sample(standardSamp, calculatedNormal) * diffuseColor.rgb;

	////float camToPixel = -V;
	////float normDotCam = max(dot(lerp(input.Normal, calculatedNormal, max(dot(input.Normal, -camToPixel), 0)),
	////	-camToPixel), 0);
	////float4 schlickFresnel = (saturate(specularColor + (1 - specularColor)*pow(1 - normDotCam, 5)), 1.0f);

	////float4 output = 0;
	////float3 incident = -V;
	////float3 reflectionVector = reflect(incident, input.Normal);

	////output = lerp(float4(diffuse, 1.0f), reflection.Sample(standardSamp, reflectionVector), schlickFresnel);
	////output.xyz += specular;

	////output = pow(abs(output), .4545);
	////output.a = 1;
	////return output;

	////------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//float4 finalColor = float4(saturate(diffuse), 1);
	//finalColor.rgb += saturate(specular);
	//return finalColor;
	////finalColor.rgb = float3(0.0f, 0.0f, 0.0f);

	////--------------------------------------------------------------------------------------------------

	//float a = max(roughness * roughness, 5e-4);
	//float a2 = a * a;
	//float k = a * 0.5f;

	//float3 basisX = normalize(cross(calculatedNormal, float3(0.0, 1.0, saturate(calculatedNormal.y*10000.0 - 9999.0))));
	//float3 basisY = cross(basisX, calculatedNormal);
	//float3 basisZ = calculatedNormal;

	//float3 reflectivity = metalness; // maybe this should actually have it's own value in some way? exists in metalness shader in marmoset
	//float3 fresnel = f0; //test float3(1.0f, 1.0f, 1.0f); seems to be what they use in marmoset? unless they change it somewhere between the metalness shader and reflection shader

	////-------------------------------------------------------------------------------------------------

	////return float4(diffuse + specular, 1.0f);
	//return float4(finalColor.rgb, 1.0f);

	////return float4(diffuse + specular, 1); // vad ska det egentligen vara på w?


	
}