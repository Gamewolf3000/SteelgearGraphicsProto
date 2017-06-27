#define NROFLIGHTS 256

static const float PI = 3.14159265359f;

cbuffer CameraData  : register(b1)
{
	float4 CameraPos;
	float4 ViewVec;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 WorldPos : WORLDPOS;
	float2 Tex : TEXCOORD;
	float3 LocalPos : LOCALPOS;
};

TextureCube envMap : register(t5);

SamplerState standardSamp : register(s0);

float4 sampleMap(float3x3 side, float3 eyedir, float3 base_ray)
{
	float specularity = 1.0f;

	float3 ray = mul(base_ray, side);
	float lambert = max(0.0, dot(ray, eyedir));
	float term = pow(lambert, specularity)*base_ray.z;
	return float4(envMap.SampleLevel(standardSamp, ray, 3.0).rgb*term, term);
}

float4 PS_main(VS_OUT input) : SV_TARGET
{

	float size = 256.0f;
	float step = 1 / size;
	float3 sum = 0;
	float3 Irr_map_direction = normalize(input.WorldPos).rgb;
	float3 env_map_direction = float3(0, 0, 0);
	float3 env_map_value;

	//Front and back-------------------------------------------------------------------------------------------
	for (int i = -size/2; i < size/2; i++)
	{
		env_map_direction = float3(i / size, 0, 1.0f);
		for (int j = -size/2; j < size/2; j++)
		{
			env_map_direction.y = j / size;

			env_map_value = envMap.Sample(standardSamp, normalize(env_map_direction)).xyz;

			sum += max(0, dot(env_map_direction, Irr_map_direction)) * env_map_value;
		}
	}

	for (int i = -size / 2; i < size / 2; i++)
	{
		env_map_direction = float3(i / size, 0, -1.0f);
		for (int j = -size / 2; j < size / 2; j++)
		{
			env_map_direction.y = j / size;

			env_map_value = envMap.Sample(standardSamp, normalize(env_map_direction)).xyz;

			sum += max(0, dot(env_map_direction, Irr_map_direction)) * env_map_value;
		}
	}

	//Top and Bottom-------------------------------------------------------------------------------------------
	for (int i = -size / 2; i < size / 2; i++)
	{
		env_map_direction = float3(i / size, 1.0f, 0);
		for (int j = -size / 2; j < size / 2; j++)
		{
			env_map_direction.z = j / size;

			env_map_value = envMap.Sample(standardSamp, normalize(env_map_direction)).xyz;

			sum += max(0, dot(env_map_direction, Irr_map_direction)) * env_map_value;
		}
	}

	for (int i = -size / 2; i < size / 2; i++)
	{
		env_map_direction = float3(i / size, -1.0f, 0);
		for (int j = -size / 2; j < size / 2; j++)
		{
			env_map_direction.z = j / size;

			env_map_value = envMap.Sample(standardSamp, normalize(env_map_direction)).xyz;

			sum += max(0, dot(env_map_direction, Irr_map_direction)) * env_map_value;
		}
	}

	//Left and Right-------------------------------------------------------------------------------------------
	for (int i = -size / 2; i < size / 2; i++)
	{
		env_map_direction = float3(1.0f, i / size, 0);
		for (int j = -size / 2; j < size / 2; j++)
		{
			env_map_direction.z = j / size;

			env_map_value = envMap.Sample(standardSamp, normalize(env_map_direction)).xyz;

			sum += max(0, dot(env_map_direction, Irr_map_direction)) * env_map_value;
		}
	}

	for (int i = -size / 2; i < size / 2; i++)
	{
		env_map_direction = float3(-1.0f, i / size, 0);
		for (int j = -size / 2; j < size / 2; j++)
		{
			env_map_direction.z = j / size;

			env_map_value = envMap.Sample(standardSamp, normalize(env_map_direction)).xyz;

			sum += max(0, dot(env_map_direction, Irr_map_direction)) * env_map_value;
		}
	}

	return float4(0.59f, 0.56f, 0.51f, 1.0f);

	//for (int i = -size; i < size; i++)
	//{
	//	env_map_direction = float3(i / size, 0, 0);
	//	for (int j = -size; j < size; j++)
	//	{
	//		env_map_direction = float3(env_map_direction.r, j / size, 0);
	//		for (int k = -size; k < size; k++)
	//		{
	//			env_map_direction = float3(env_map_direction.r, env_map_direction.g, k / size);
	//			env_map_value = envMap.Sample(standardSamp, normalize(env_map_direction));
	//		}
	//	}
	//}

	////-----------------------------------------------------------------------------------------------------------------------------------
	////return float4(envMap.SampleLevel(standardSamp, normalize(float4(0.0f, 0.0f, 0.0f, input.WorldPos.w)), 3.0f).rgb , 1.0f);
	//float4 iradiance_map_direction = normalize(input.WorldPos);
	//float4 envMap_direction = 0;
	//float4 I = 0;
	//float4 sum = float4(0.0f, 0.0f, 0.0f, 1.0f);
	//const int size = 1;
	//const int totSize = 256;

	//float3x3 zRot;
	//float3x3 upRot;
	//float rot;
	//float3 new_up;

	//float3 base_direction = float3(1.0f, 0.0f, 0.0f);
	//float3 base_up = float3(0.0f, 1.0f, 0.0f);
	//for (int z = 0; z < size; z += 1)
	//{
	//	/*zRot = float3x3(1.0f, 0.0f, 0.0f,
	//		0.0f, cos(2 * PI * (0 / (size * 4))), -sin(2 * PI * (0 / (size * 4))),
	//		0.0f, sin(2 * PI * (0 / (size * 4))), cos(2 * PI * (0 / (size * 4)))
	//		);*/
	//	rot = 2 * PI * z / (totSize);
	//	zRot = float3x3 (cos(rot), -sin(rot), 0.0f,
	//					 sin(rot), cos(rot), 0.0f,
	//					 0.0f, 0.0f, 1.0f);

	//	//return float4(zRot[0].x, zRot[0].y, zRot[0].z, 1.0f);

	//	for (int y = 0; y < size; y += 1)
	//	{
	//		new_up = normalize(mul(base_up, zRot));
	//		rot = 2 * PI * y / (totSize);

	//		upRot = float3x3 (cos(rot) + new_up.x*new_up.x * (1 - cos(rot)), new_up.x * new_up.y * (1 - cos(rot)) - new_up.z * sin(rot), new_up.x * new_up.z * (1 - cos(rot)) + new_up.y * sin(rot),
	//						  new_up.x * new_up.y * (1 - cos(rot)) + new_up.z * sin(rot), cos(rot) + new_up.y*new_up.y * (1 - cos(rot)), new_up.z * new_up.y * (1 - cos(rot)) - new_up.x * sin(rot),
	//						  new_up.x * new_up.z * (1 - cos(rot)) - new_up.y * sin(rot), new_up.z * new_up.y * (1 - cos(rot)) + new_up.x * sin(rot), cos(rot) + new_up.z*new_up.z * (1 - cos(rot))
	//						 );

	//		envMap_direction = normalize(float4(mul(mul(base_direction, zRot), upRot), 0.0f));
	//		I = envMap.Sample(standardSamp, envMap_direction);

	//		//return float4(dot(base_direction, iradiance_map_direction).xxx, 1.0f);

	//		//if (dot(base_direction, iradiance_map_direction) >= 0.7f)
	//		//	return float4(1.0f, 1.0f, 1.0f, 1.0f);

	//		//sum += max(0, dot(envMap_direction, iradiance_map_direction));
	//		//return float4(dot(envMap_direction, iradiance_map_direction).xxx, 1.0f);

	//	}
	//}

	////for (float x = 0; x <= 1.0f; x += 1.0f / size)
	////{
	////	for (float y = 0; y <= 1.0f; y += 1.0f / size)
	////	{
	////		[unroll(size)] for (float z = 0; z <= 1.0f; z += 1.0f / size)
	////		{
	////			envMap_direction = normalize(float4(x, y, z, input.WorldPos.w));
	////			I = envMap.SampleLevel(standardSamp, envMap_direction, 0.0f);

	////			sum += max(0, dot(envMap_direction, iradiance_map_direction)) * I;
	////			//return sum;
	////		}
	////	}
	////}

	//return sum;

	////-------------------------------------------------------------------------------------------------------
	//return float4(envMap.SampleLevel(standardSamp, normalize(input.WorldPos), 3.0f).rgb , 1.0f);

	/*const float size = 64.0;
	const float start = ((0.5 / size) - 0.5)*2.0;
	const float end = -start;
	const float incr = 2.0 / size;

	const float4x4 inv_proj = float4x4(float4(1.00000012, 0.0f, 0.0f, 0.0f), float4(0.0f, 1.00000012, 0.0f, 0.0f), float4(0.0f, -0.0f, 0.0f, 1.0f), float4(0.0f, 0.0f, -9.99900055, 10.0f));

	const float3 x = float3(1.0, 0.0, 0.0);
	const float3 y = float3(0.0, 1.0, 0.0);
	const float3 z = float3(0.0, 0.0, 1.0);

	const float3x3 front = float3x3(x, y, z);
	const float3x3 back = float3x3(x, y, -z);
	const float3x3 right = float3x3(z, y, x);
	const float3x3 left = float3x3(z, y, -x);
	const float3x3 top = float3x3(x, z, y);
	const float3x3 bottom = float3x3(x, z, -y);

	float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 eyedir = normalize(input.WorldPos - CameraPos).xyz;
	float3 ray;

	for (float xi = start; xi <= end; xi += incr)
	{
		for (float yi = start; yi <= end; yi += incr)
		{
			ray = normalize((mul(float4(xi, yi, 0.0, 1.0), inv_proj)).xyz);

			result += sampleMap(front, eyedir, ray);
			result += sampleMap(back, eyedir, ray);
			result += sampleMap(top, eyedir, ray);
			result += sampleMap(bottom, eyedir, ray);
			result += sampleMap(left, eyedir, ray);
			result += sampleMap(right, eyedir, ray);
		}
	}

	return float4(result.xyz / result.w, 1.0f);

	return float4(-1.0f, 0.0f, 0.0f, 1.0f);*/
}