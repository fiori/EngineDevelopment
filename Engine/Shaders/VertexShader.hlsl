struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer CBuffer0
{
    matrix WVPMatrix; // 64 bytes
    float scale; // 4 bytes
    float3 packing; // 3x4 = 12 bytes
}


VOut VShader(float4 Pos : POSITION, float4 Clr : COLOR)
{
    VOut output;
    output.position = mul(WVPMatrix, Pos);
    output.position.xy *= scale;
    output.color = Clr;
    return output;
}