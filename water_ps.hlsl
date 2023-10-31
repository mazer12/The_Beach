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

	float2 tex = (input.tex * 8.0f) + time * 0.002f;

	// Invert the light direction for calculations.
	lightDir = normalize(input.position3D - lightPosition);

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, -lightDir));

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	//adding ambient

	float3 cameraDir = normalize(input.position3D - cameraPosition);
	

	float cons = 1.0;
	float3 is = float3(1.0, 1.0, 1.0);
	float3 reflectedLight = reflect(lightDir, input.normal);
	float amount = pow(max(0.0f, dot(reflectedLight, cameraPosition)), 1.0);
	float4 specularLight = float4(cons * amount * is, 1.0);

	color = ambientColor  + (diffuseColor * lightIntensity) +specularLight;
	color = saturate(color);

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, tex);
	color = color * textureColor ;

	return color;
}