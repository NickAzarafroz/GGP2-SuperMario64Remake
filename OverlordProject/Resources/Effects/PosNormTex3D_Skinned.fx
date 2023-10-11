float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);

float4x4 gBones[70]; // New variable for bone transformations

Texture2D gDiffuseMap;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

struct VS_INPUT{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
    float4 blendIndices : BLENDINDICES;
    float4 blendWeights : BLENDWEIGHTS;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

BlendState NoBlending
{
	BlendEnable[0] = FALSE;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output;
    float4 transformedPosition = 0;
    float3 transformedNormal = 0;
	
	// loop through each bone
    for (int i = 0; i < 4; ++i)
    {
		// get the current bone index
        int boneIndex = int(input.blendIndices[i]);

		// make sure the bone index is valid
        if (boneIndex >= 0)
        {
			// calculate bone space position & normal
            float4 boneSpacePos = mul(float4(input.pos, 1.0f), gBones[boneIndex]);
            float3 boneSpaceNormal = mul(input.normal, (float3x3) gBones[boneIndex]);

			// append position & normal to transformedPosition & transformedNormal
            transformedPosition += input.blendWeights[i] * boneSpacePos;
            transformedNormal += input.blendWeights[i] * boneSpaceNormal;
        }
    }
	
	// ensure w-component of transformedPosition is 1
    transformedPosition.w = 1.0f;

	// transform transformedPosition to clipping space
    output.pos = mul(transformedPosition, gWorldViewProj);

	// transform transformedNormal to world space (rotation only)
    output.normal = normalize(mul(transformedNormal, (float3x3) gWorld));

	// copy texture coordinates
    output.texCoord = input.texCoord;

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{

	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	float3 color_rgb= diffuseColor.rgb;
	float color_a = diffuseColor.a;
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

	return float4( color_rgb , color_a );
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

