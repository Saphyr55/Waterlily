#include "Shared/Defines.hlsli"

struct FSInput
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
};

struct FSOuput
{
    WLS_LOCATION(0) 
    float4 Color : SV_TARGET;
};

FSOuput main(FSInput input)
{
    FSOuput output;
	
    output.Color = input.Color;
	
    return output;
}