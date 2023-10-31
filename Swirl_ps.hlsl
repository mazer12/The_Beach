Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(b0)
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightPosition;
	float padding;
	float3 cameraPosition;
	float time;
};



struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 position3D : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
	float4	textureColor;
	float3	lightDir;
	float	lightIntensity;
	float3	color;



	float rt_w;

	float rt_h;


	shaderTexture.GetDimensions(rt_w, rt_h);


	float radius = 200.0;
	float angle = 1.5;
	float2 center = float2(600.0, 200.0);

	float2 uv = input.tex.xy;


	float2 texSize = float2(rt_w, rt_h);
	float2 tc = uv * texSize;
	tc -= center;
	float dist = length(tc);
	if (dist < radius)
	{
		float percent = (radius - dist) / radius;
		float theta = percent * percent * angle * 8.0;
		float s = sin(theta);
		float c = cos(theta);
		tc = float2(dot(tc, float2(c, -s)), dot(tc, float2(s, c)));
	}
	tc += center;
	color = shaderTexture.Sample(SampleType, tc / texSize).rgb;
	return float4(color, 1.0);



}