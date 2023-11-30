cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj; 
};

struct VertexIn
{
  float3 Pos  : POSITION;
  float4 Color : COLOR;
};

struct VertexOut
{
  float4 PosH : SV_POSITION;
  float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
  VertexOut vout;

  // 把顶点变换到齐次裁剪空间
  vout.PosH = mul(float4(vin.Pos, 1.0f), gWorldViewProj);

  // 直接将顶点颜色传至像素着色器
  vout.Color = vin.Color;

  return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
  return pin.Color;
}