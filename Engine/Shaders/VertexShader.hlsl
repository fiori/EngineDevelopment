struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VOut VShader(float4 Pos : POSITION, float4 Clr : COLOR)
{
    VOut output;
    output.position = Pos;
    output.color = Clr;
    return output;
}