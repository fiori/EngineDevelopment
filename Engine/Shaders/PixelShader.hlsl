Texture2D texture0;
SamplerState sampler0;

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET
{
    return color * texture0.Sample(sampler0, texcoord);
}