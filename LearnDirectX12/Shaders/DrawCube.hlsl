//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************
Texture2D chicken:register(t0);
SamplerState gsamPointWrap    : register(s0);
SamplerState gsamPointClamp    : register(s1);
SamplerState gsamLinearWrap    : register(s2);
SamplerState gsamLinearClamp   : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
cbuffer cbPerObject : register(b0)
{
	float4x4 ObjToWorldMatrix;
	float4 data; 
};
cbuffer cbPerFrame:register(b1){
	float4x4 gWorldViewProj;
};

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
	float2 uv1:TEXCOORD0;
	float2 uv2:TEXCOORD1;
	float3 Normal:NORMAL;
	float3 tangent:TANGENT;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
	float2 uv:TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// // Transform to homogeneous clip space.
	vout.PosH=mul(float4(vin.PosL, 1.0f), ObjToWorldMatrix);
	vout.PosH = mul(vout.PosH, gWorldViewProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
    vout.uv=vin.uv1;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 color=chicken.Sample(gsamAnisotropicWrap,pin.uv);
    return color;
}


