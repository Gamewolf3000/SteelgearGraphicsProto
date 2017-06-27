cbuffer VSConstants : register(cb0)
{
	float4x4 WorldMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
}

struct GS_IN
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float2 Blend : BLENDCOORD;
	float3 Normal : NORMAL;
	float4 Lightpos : LIGHT;
};


struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
	float4 Old : OLDPOSITION;
	float2 Blend : BLENDCOORD;
	float4 Lightpos : LIGHT;
};

[instance(1)]
[maxvertexcount(3)]
void GS_main(triangle GS_IN input[3], uint InstanceID : SV_GSInstanceID, inout TriangleStream<GS_OUT> OutputStream)
{
	GS_OUT output = (GS_OUT)0;

	for (uint i = 0; i < 3; i += 1)
	{

		output.Pos = mul(input[i].Pos, WorldMatrix);
		output.Old = input[i].Pos;

		output.Old = mul(mul(output.Old, WorldMatrix), ViewMatrix);
		output.Old /= output.Old.w;

		if (InstanceID == 1)
		{
			output.Pos.xyz -= output.Normal.xyz;
			output.Old.xyz -= output.Normal.xyz;
		}

		output.Pos = mul(output.Pos, ViewMatrix);
		output.Pos = mul(output.Pos, ProjectionMatrix);

		output.Tex = input[i].Tex;
		output.Blend = input[i].Blend;
		output.Normal = mul(input[i].Normal, WorldMatrix);

		output.Lightpos = input[i].Lightpos;

		OutputStream.Append(output);
	}


	OutputStream.RestartStrip();

}