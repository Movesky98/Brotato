
#include "Share.hlsli"

cbuffer CBTileMap : register(b10)
{
    float2 cbTileLTUV;
    float2 cbTileRBUV;
}

float2 ComputeTileUV(float2 UV)
{
    float2 Result = (float2) 0.f;
    
    if (UV.x > 0.f)
    {
        Result.x = cbTileRBUV.x;
    }
    else
    {
        Result.x = cbTileLTUV.x;
    }
    
    if (UV.y > 0.f)
    {
        Result.y = cbTileLTUV.y;
    }
    else
    {
        Result.y = cbTileRBUV.y;
    }
    
    return Result;
}

Texture2D tbTileTexture : register(t1);

VS_OUTPUT_TEX TileMapVS(VS_INPUT_TEX input)
{
    VS_OUTPUT_TEX output = (VS_OUTPUT_TEX) 0;

    output.Pos = mul(float4(input.Pos, 1.f), cbWVP);
    output.UV = ComputeTileUV(input.UV);
    
    return output;
}

PS_OUTPUT_COLOR2D TileMapPS(VS_OUTPUT_TEX input)
{
    PS_OUTPUT_COLOR2D output = (PS_OUTPUT_COLOR2D) 0;
    
    // 쿼드 가장자리 픽셀의 UV가 프레임 경계에 정확히 걸리면 옆 아틀라스 셀의 텍셀을 찍는다.
    // 프레임 안쪽 반텍셀로 가둬서 경계를 넘지 못하게 한다.
    // 이미 안쪽에 있는 샘플은 clamp가 값을 바꾸지 않으므로 텍셀:픽셀 1:1 대응은 그대로다.
    float Width, Height;
    tbTileTexture.GetDimensions(Width, Height);
    
    float2 HalfTexel = float2(0.5f / Width, 0.5f / Height);
    float2 MinUV = min(cbTileLTUV, cbTileRBUV) + HalfTexel;
    float2 MaxUV = max(cbTileLTUV, cbTileRBUV) - HalfTexel;
    
    output.Color = tbTileTexture.Sample(sbSampler, clamp(input.UV, MinUV, MaxUV));
    
    return output;
}
