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

// IN/OUT STRUCTS
// --------------
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

// VERTEX SHADER
// -------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = float4(input.Position, 1.0f);
    output.TexCoord = input.TexCoord;
    return output;
}

// PIXEL SHADER
// ------------
float4 PS(PS_INPUT input) : SV_Target
{
    // Sample the pixel color from the input texture
    float4 pixelColor = gTexture.Sample(samPoint, input.TexCoord);

    // Define bloom parameters
    float bloomThreshold = 0.2f; // Adjust this value to control the brightness threshold for bloom
    float bloomIntensity = 2.0f; // Adjust this value to control the intensity of the bloom effect
    float bloomBlurAmount = 5.0f; // Adjust this value to control the amount of blur for the bloom effect

    // Apply bloom effect to bright areas
    float4 bloomColor = max(pixelColor - bloomThreshold, 0.0f);
    bloomColor = pow(bloomColor * bloomIntensity, bloomBlurAmount);

    // Combine the bloom effect with the original pixel color
    float4 finalColor = pixelColor + bloomColor;

    return finalColor;
}

// TECHNIQUE
// ---------
technique11 Bloom
{
    pass P0
    {
        // Set states...
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}
