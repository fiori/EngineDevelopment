TextureCube cube0;
SamplerState sampler0;

cbuffer CBuffer0
{
    matrix WVPMatrix; // 64 bytes
    matrix worldViewMatrix;
    float4 directional_light_vector; // 16 bytes
    float4 directional_light_color; // 16 bytes
    float4 ambient_light_color; // 16 bytes
}

struct VOut
{
    float4 position : SV_POSITION;
    float3 texcoord : TEXCOORD;
    float4 color : COLOR;
};


VOut ModelVS(float4 Pos : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
    VOut output;
    

    float3 wvpos = mul(worldViewMatrix, Pos);
    float3 wvnormal = mul(worldViewMatrix, normal);
    float3 eyer = -normalize(wvpos);
    output.position = mul(WVPMatrix, Pos);

    output.texcoord = 2.0 * dot(eyer, wvnormal) * wvnormal - eyer;
    return output;
}

float4 PShader(VOut input) : SV_TARGET
{
    return cube0.Sample(sampler0, input.texcoord);
}

