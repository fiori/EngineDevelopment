TextureCube cube0;
SamplerState sampler0;

cbuffer CBuffer0
{
    matrix WVPMatrix; // 64 bytes
    float4 directional_light_vector; // 16 bytes
    float4 directional_light_color; // 16 bytes
    float4 ambient_light_color; // 16 bytes
}

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};


VOut ModelVS(float4 Pos : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
    VOut output;

    output.texcoord = Pos.xyz;
    output.position = mul(WVPMatrix, Pos);
    
    float diffuse_amount = dot(directional_light_color, normal);
    diffuse_amount = saturate(diffuse_amount);
    output.color = ambient_light_color + (directional_light_color * diffuse_amount);

    
    return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD) : SV_TARGET
{
    return color * cube0.Sample(sampler0, texcoord);
}