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

	//float2 tex = (input.tex * 8) + time * 0.002f;

	// Invert the light direction for calculations.
	lightDir = normalize(input.position3D - lightPosition);

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, -lightDir));

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	//adding ambient

	//specular light
	/*float3 cameraDir = normalize(input.position3D - cameraPosition);

	float cons = 1.0f;
	float3 is = float3(1.0, 1.0, 1.0);
	float3 reflectedLight = reflect(lightPosition, input.normal);
	float amount = pow(max(0.0f, dot(reflectedLight, cameraDir)), 1.0f);
	float4 specularLight = float4(cons * amount * is, 0.1f);*/

	color = ambientColor + (diffuseColor ); // + specularLight;
	color = saturate(color);

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);
	color = color * textureColor;

	return color;
}










//Texture2D shaderTexture : register(t0);
//SamplerState SampleType : register(s0);
//
//
//cbuffer LightBuffer : register(b0)
//{
//	float4 ambientColor;
//	float4 diffuseColor;
//	float3 lightPosition;
//	float padding;
//	float3 cameraPosition;
//	float time;
//};



//struct InputType
//{
//	float4 position : SV_POSITION;
//	float2 tex : TEXCOORD0;
//	float3 normal : NORMAL;
//	float3 position3D : TEXCOORD2;
//};
//
//float4 main(InputType input) : SV_TARGET
//{
//	float4	textureColor;
//	float3	lightDir;
//	float	lightIntensity;
//	float4	color;
//
//
//	float rt_w; // render target width
//	float rt_h; // render target height
//	float vx_offset = 0.5;
//
//	shaderTexture.GetDimensions(rt_w, rt_h);
//
//	float offset[3] = { 0.0, 50.3846153846, 53.2307692308 };
//	float weight[3] = { 0.4270270270, 0.3162162162, 0.0702702703 };
//
//	float3 tc = float3(1.0, 0.0, 0.0);
//	if (input.tex.x < (vx_offset - 0.01))
//	{
//		float2 uv = input.tex.xy;
//		tc = shaderTexture.Sample(SampleType, uv).rgb * weight[0];
//		for (int i = 1; i < 3; i++)
//		{
//			tc += shaderTexture.Sample(SampleType, uv + float2(offset[i] / rt_w, offset[i]) / rt_h).rgb \
//				* weight[i];
//			tc += shaderTexture.Sample(SampleType, uv - float2(offset[i] / rt_w, offset[i]) / rt_h).rgb \
//				* weight[i];
//		}
//	}
//	else if (input.tex.x >= (vx_offset + 0.01))
//	{
//		tc = shaderTexture.Sample(SampleType, input.tex.xy).rgb;
//	}
//	color = float4(tc, 1.0);
//
//	return color;
//}










//Texture2D shaderTexture : register(t0);
//SamplerState SampleType : register(s0);
//
//
//cbuffer LightBuffer : register(b0)
//{
//	float4 ambientColor;
//	float4 diffuseColor;
//	float3 lightPosition;
//	float padding;
//	float3 cameraPosition;
//	float time;
//};
//
//
//
//struct InputType
//{
//	float4 position : SV_POSITION;
//	float2 tex : TEXCOORD0;
//	float3 normal : NORMAL;
//	float3 position3D : TEXCOORD2;
//};
//
//float4 main(InputType input) : SV_TARGET
//{
//	float4	textureColor;
//	float3	lightDir;
//	float	lightIntensity;
//	float3	color;
//
//
//
//    float rt_w;
//
//	float rt_h;
//
//
//	shaderTexture.GetDimensions(rt_w, rt_h);
//
//
//	float radius = 200.0;
//    float angle = 0.8;
//	float2 center = float2(600.0, 200.0);
//
//	float2 uv = input.tex.xy;
//
//
//	float2 texSize = float2(rt_w, rt_h);
//	float2 tc = uv * texSize;
//	tc -= center;
//	float dist = length(tc);
//	if (dist < radius)
//	{
//		float percent = (radius - dist) / radius;
//		float theta = percent * percent * angle * 8.0;
//		float s = sin(theta);
//		float c = cos(theta);
//		tc = float2(dot(tc, float2(c, -s)), dot(tc, float2(s, c)));
//	}
//	tc += center;
//	color = shaderTexture.Sample(SampleType, tc / texSize).rgb;
//	return float4(color, 1.0);
//
//
//
//}