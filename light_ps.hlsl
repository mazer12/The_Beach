// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

Texture2D shaderTexture : register(t0);
Texture2D shaderTexture1 : register(t1);
SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(b0)
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightPosition;
	float  padding;
	float3 cameraPosition;
	float time;
};


struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 tex1 : TEXCOORD1;
	float3 normal : NORMAL;
	float3 position3D : TEXCOORD2;
	float3 worldPos : TEXCOORD3;
};

float4 main(InputType input) : SV_TARGET
{
	float4	textureColor;
	float4	textureColor1;
	float3	lightDir;
	float	lightIntensity;
	float4	color;
	float3  dir;


	

	// Invert the light direction for calculations.
	lightDir = normalize(input.position3D - lightPosition);

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, -cameraPosition));

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	//adding ambient

	color = ambientColor + (diffuseColor * lightIntensity); 
	color = saturate(color);

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);
	textureColor1 = shaderTexture1.Sample(SampleType, input.tex1);

	color = color * (textureColor*0.4 + textureColor1*0.6);

	return color;
}

