cbuffer CBuffer0
{
    matrix WVPMatrix; // 64 bytes
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

    float4 default_color = { 1.0, 1.0, 1.0, 1.0 };
    output.position = mul(WVPMatrix, Pos);
    output.texcoord = texcoord;
    output.color = default_color;
    return output;
}
