
struct VS_INPUT
{
    float2 pos : POSITION;
    float2 uv0 : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv0 : TEXCOORD0;
};

cbuffer baseCBuffer : register (b0)
{
    matrix transform;
    float2 screensize;
    float2 padding;
}

Texture2D baseTexture;
SamplerState baseSampler;

VS_OUTPUT main_vs(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(transform, float4(input.pos, 0, 1));
    output.pos.xy /= screensize;
    output.pos.y = 1 - output.pos.y;
    output.pos.xy = output.pos.xy * 2 - 1;
    output.uv0 = input.uv0;

    return output;
}

float4 main_ps(VS_OUTPUT input) : SV_Target
{
    return baseTexture.Sample(baseSampler, input.uv0);
}
