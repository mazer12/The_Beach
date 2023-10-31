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
	float4	color;


	float rt_w; // render target width
	float rt_h; // render target height
	float vx_offset = 0.5;

	shaderTexture.GetDimensions(rt_w, rt_h);

	float offset[3] = { 0.0, 5.3846153846, 8.2307692308 };
	float weight[3] = { 0.4270270270, 0.3162162162, 0.0702702703 };

	float3 tc = float3(1.0, 0.0, 0.0);
	if (input.tex.x < (vx_offset - 0.01))
	{
		float2 uv = input.tex.xy;
		tc = shaderTexture.Sample(SampleType, uv).rgb * weight[0];
		for (int i = 1; i < 3; i++)
		{
			tc += shaderTexture.Sample(SampleType, uv + float2(offset[i] / rt_w, offset[i]) / rt_h).rgb \
				* weight[i];
			tc += shaderTexture.Sample(SampleType, uv - float2(offset[i] / rt_w, offset[i]) / rt_h).rgb \
				* weight[i];
		}
	}
	else if (input.tex.x >= (vx_offset + 0.01))
	{
		tc = shaderTexture.Sample(SampleType, input.tex.xy).rgb;
	}
	color = float4(tc, 1.0);

	return color;
}
