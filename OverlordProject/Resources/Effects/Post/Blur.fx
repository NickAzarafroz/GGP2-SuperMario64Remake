//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
/// Create Rasterizer State (Backface culling) 


//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};

//Depth Stencil State
//-------------------
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
};

//Rasterizer State
//------------------
RasterizerState BackCulling
{
    CullMode = BACK;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
	// Set the Position
    output.Position = float4(input.Position, 1.0f);
	// Set the TexCoord
    output.TexCoord = input.TexCoord;
	
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
	// Step 1: find the dimensions of the texture (the texture has a method for that)
    int textureWidth, textureHeight, numMipLevels;
    gTexture.GetDimensions(0, textureWidth, textureHeight, numMipLevels);
    float2 textureSize = float2(textureWidth, textureHeight);
	// Step 2: calculate dx and dy (UV space for 1 pixel)
    float dx = 1.0f / textureSize.x;
    float dy = 1.0f / textureSize.y;
	// Step 3: Create a double for loop (5 iterations each)
	//		   Inside the loop, calculate the offset in each direction. Make sure not to take every pixel but move by 2 pixels each time
	//			Do a texture lookup using your previously calculated uv coordinates + the offset, and add to the final color
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int x = 0; x < 5; ++x)
    {
        for (int y = 0; y < 5; ++y)
        {
            // Calculate the offset in each direction
            float2 offset = float2(dx * x, dy * y);

            // Perform a texture lookup using the calculated UV coordinates + offset
            float4 texColor = gTexture.Sample(samPoint, input.TexCoord + offset);

            // Accumulate the color
            finalColor += texColor;
        }
    }
	// Step 4: Divide the final color by the number of passes (in this case 5*5)
    finalColor /= (5 * 5);
	// Step 5: return the final color
    return finalColor;
}


//TECHNIQUE
//---------
technique11 Blur
{
    pass P0
    {
		// Set states...
        SetRasterizerState(BackCulling);
        SetDepthStencilState(EnableDepth, 0);
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}