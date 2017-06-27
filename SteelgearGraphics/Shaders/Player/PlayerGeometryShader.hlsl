cbuffer GSConstants : register(b0)
{
	float4x4 WorldMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
}

cbuffer GSLightConstants : register(b1)
{
	float4x4 LightWorldMatrix;
	float4x4 LightViewMatrix;
	float4x4 LightProjectionMatrix;
}

cbuffer camera : register(b2)
{
	float4 camPos;
}

cbuffer movement : register(b13)
{
	float4x4 movementMatrix;
}


struct GS_IN
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float4 Normal : NORMAL;
};


struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Normal : NORMAL;
};

float4 position[4] =
{
	float4(-6.0f/9.0f, -1.0, 0.0f, 1.0f),
	float4(-6.0f / 9.0f, 1.0, 0.0f, 1.0f),
	float4(6.0f / 9.0f, 1.0, 0.0f, 1.0f),
	float4(6.0f / 9.0f, -1.0, 0.0f, 1.0f)
};

float2 uv[4] =
{
	float2(0, 1),
	float2(0, 0),
	float2(1, 0),
	float2(1, 1)
};


[maxvertexcount(4)]
void GS_main(point GS_IN input[1], inout TriangleStream<GS_OUT> OutputStream)
{
	GS_OUT output = (GS_OUT)0;

	//input[0].Pos = mul(input[0].Pos, ViewMatrix);

	//for (int i = 0; i < 4; i++)
	//{
	//	output.Pos = mul(input[0].Pos + position[i], ProjectionMatrix);
	//	output.Tex = uv[i];
	//	output.Normal = input[0].Normal;
	//	OutputStream.Append(output);
	//}

	input[0].Pos = mul(input[0].Pos, movementMatrix);
	float3 camToParticle = normalize(input[0].Pos.xyz - camPos.xyz);
	float4x4 wvp = mul(mul(WorldMatrix, ViewMatrix), ProjectionMatrix);
	float3 rightVec = normalize(cross(float3(0, 1, 0), camToParticle));
	float3 upVec = cross(rightVec, camToParticle);

	float size = (600.0f / 900.0f);
	float size2 = (900.0f / 900.0f);

	//output.Pos.xz = input[0].Pos.xz - rightVec.xz*size;
	//output.Pos.y = input[0].Pos.y - size;
	output.Pos.w = 1;
	output.Pos.xz = input[0].Pos.xz - rightVec.xz*size + upVec.xz*size;
	output.Pos.y = input[0].Pos.y + upVec.y*size2;
	output.Tex = float2( 0, 1 );
	output.Normal = input[0].Normal;

	output.Pos = mul(output.Pos, wvp);

	OutputStream.Append(output);

	//output.Pos.xz = input[0].Pos.xz - rightVec.xz*size;
	//output.Pos.y = input[0].Pos.y + size;
	output.Pos.w = 1;
	output.Pos.xz = input[0].Pos.xz - rightVec.xz*size - upVec.xz*size;
	output.Pos.y = input[0].Pos.y - upVec.y*size2;
	output.Tex = float2(0, 0);
	output.Normal = input[0].Normal;

	output.Pos = mul(output.Pos, wvp);

	OutputStream.Append(output);

	//output.Pos.xz = input[0].Pos.xz + rightVec.xz*size;
	//output.Pos.y = input[0].Pos.y - size;
	output.Pos.w = 1;
	output.Pos.xz = input[0].Pos.xz + rightVec.xz*size + upVec.xz*size;
	output.Pos.y = input[0].Pos.y + upVec.y*size2;
	output.Tex = float2(1, 1);
	output.Normal = input[0].Normal;

	output.Pos = mul(output.Pos, wvp);

	OutputStream.Append(output);

	//output.Pos.xz = input[0].Pos.xz + rightVec.xz*size;
	//output.Pos.y = input[0].Pos.y + size;
	output.Pos.w = 1;
	output.Pos.xz = input[0].Pos.xz + rightVec.xz*size - upVec.xz*size;
	output.Pos.y = input[0].Pos.y - upVec.y*size2;
	output.Tex = float2(1, 0);
	output.Normal = input[0].Normal;

	output.Pos = mul(output.Pos, wvp);

	OutputStream.Append(output);

}