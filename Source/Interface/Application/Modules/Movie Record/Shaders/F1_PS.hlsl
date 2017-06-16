#include "Common_PS.hlsl"

struct PS_OUTPUT
{
	float4 Color : SV_Target0;
};

Texture2D Frame : register(t0);
Texture2D Sample1 : register(t1);

PS_OUTPUT PSMain(PS_INPUT input)
{
	PS_OUTPUT ret;

	float4 framecol = float4(Frame.Sample(TextureSampler, input.Tex).rgb, 1.0);
	float4 sample1col = float4(Sample1.Sample(TextureSampler, input.Tex).rgb, 1.0);

	ret.Color = framecol + sample1col;

	return ret;
}