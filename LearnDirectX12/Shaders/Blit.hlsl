//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************
Texture2D src:register(t0);
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
cbuffer cbPerMaterial:register(b2){
	float4 color3;
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
	float3 normal:NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// // Transform to homogeneous clip space.
	vout.PosH=float4(vin.PosL, 1.0f);
	
	// Just pass vertex color into the pixel shader.
    vout.uv=vin.uv1;
	vout.normal=float3(1,0,0);
	vout.Color=float4(1,0,1,1);
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 color=src.Sample(gsamAnisotropicClamp,pin.uv);
    return color;
}


