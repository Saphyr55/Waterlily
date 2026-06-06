#include "Shared/Defines.hlsli"

struct VSInput
{
	WLSL_LOCATION(0) 
	float3 Position : POSITION0;
	
	WLSL_LOCATION(1) 
	float4 Color : COLOR0;
};

struct VSOuput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
};

VSOuput main(VSInput input)
{
    VSOuput output;
	
    output.Position = float4(input.Position, 1.0f);
    output.Color = input.Color;
	
	return output;
}