cbuffer CBuffer0
{
    matrix WVPMatrix; // 64 bytes
    float4 color;
}

struct VOut
{
    float4 position : SV_POSITION;
    float4 pcolor : COLOR;
    float2 texcoord : TEXCOORD;
};

VOut ModelVS(float4 Pos : POSITION)
{
    VOut output;

    output.position = mul(WVPMatrix, Pos);
    output.pcolor = color;
    output.texcoord = Pos.xy;
    
    return output;
}

float4 PShader(VOut input) : SV_TARGET
{
    float distsq = input.texcoord.x * input.texcoord.x + input.texcoord.y * input.texcoord.y;
    clip(1.0f - distsq);
    return input.pcolor;
}
