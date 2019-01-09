TextureCube cube0;
SamplerState sampler0;

cbuffer CBuffer0
{
    matrix WVPMatrix; // 64 bytes

}

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 texcoord : TEXCOORD;
};


VOut ModelVS(float4 Pos : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
    VOut output;

    output.texcoord = Pos.xyz;
    output.position = mul(WVPMatrix, Pos);
    
    //float diffuse_amount = dot(directional_light_color, normal);
    //diffuse_amount = saturate(diffuse_amount);
    //output.color = ambient_light_color + (directional_light_color * diffuse_amount);

    
    return output;
}

float4 PShader(VOut input) : SV_TARGET
{
    return cube0.Sample(sampler0, input.texcoord);
}

//TextureCube cube0;
//SamplerState sampler0;

//cbuffer CBuffer0
//{
//    matrix WVPMatrix; // 64 bytes
//}

//struct SKYMAP_VS_OUTPUT
//{
//    float4 position : SV_POSITION;
//    float3 texcoord : TEXCOORD;
//};



//SKYMAP_VS_OUTPUT SKYMAP_VS(float3 Pos : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
//{
//    SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT) 0;

//    output.position = mul(flloat4(Pos, 1.0f), WVPMatrix).xyww;
//    output.texcoord = Pos;
    
//    return output;
//}

//float4 SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_TARGET
//{
//    return cube0.Sample(sampler0, input.texcoord);
//}