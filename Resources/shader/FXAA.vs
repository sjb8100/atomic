#version 330 core

    

struct  RenderStates
{
    mat4 ModelViewProjectionMatrix;
    vec4 CameraPosition;
    vec4 CameraDirection;
    vec4 Color;

    vec2 ScreenTexcoord;
    vec2 ScreenSize;
    vec2 RcpScreenSize;
    float AspectRatio;
    float RcpAspectRatio;
    float Frame;
    int Level;
    int ShowMultiresolution;
};

struct  FXAAParams
{
    vec4 fxaaConsolePosPos;
    vec4 fxaaConsoleRcpFrameOpt;
    vec4 fxaaConsoleRcpFrameOpt2;
    vec4 fxaaConsole360RcpFrameOpt2;
    vec4 fxaaConsole360ConstDir;
    vec2 fxaaQualityRcpFrame;
    float fxaaQualitySubpix;
    float fxaaQualityEdgeThreshold;
    float fxaaQualityEdgeThresholdMin;
    float fxaaConsoleEdgeSharpness;
    float fxaaConsoleEdgeThreshold;
    float fxaaConsoleEdgeThresholdMin;
};

struct  FadeParams
{
    vec4 Color;
};

struct  FillParams
{
    vec4 Color;
};

struct  MultiresolutionParams
{
    ivec4 Level;
    vec4 Threshold;
};

struct  DebugShowBufferParams
{
    vec2 BottomLeft; 
    vec2 UpperRight; 
    vec2 ColorRange; 
};

    layout(std140) uniform render_states
    {
        RenderStates u_RS;
    };

    uniform sampler2D u_ParamBuffer;

    
    

    

float SmoothInterpolation2(float v)
{
    return v < 0.5 ? 2.0*v*v : -1.0+4.0*v-2*v*v;
}

float SmoothInterpolation(float v, int n)
{
    return v < 0.5 ? pow(2.0*v, n) : 1.0 - pow(2.0-2.0*v, n);
}

    

    

    

    

    

    
        
    

    
        
    

    
        
    

    
    
    
    
    
    

float FxaaLuma(vec4 rgba) { return rgba.w; }

vec4 FxaaPixelShader(
    vec2 pos,
    vec4 fxaaConsolePosPos,
    sampler2D tex,
    sampler2D fxaaConsole360TexExpBiasNegOne,
    sampler2D fxaaConsole360TexExpBiasNegTwo,
    vec2 fxaaQualityRcpFrame,
    vec4 fxaaConsoleRcpFrameOpt,
    vec4 fxaaConsoleRcpFrameOpt2,
    vec4 fxaaConsole360RcpFrameOpt2,
    float fxaaQualitySubpix,
    float fxaaQualityEdgeThreshold,
    float fxaaQualityEdgeThresholdMin,
    float fxaaConsoleEdgeSharpness,
    float fxaaConsoleEdgeThreshold,
    float fxaaConsoleEdgeThresholdMin,
    vec4 fxaaConsole360ConstDir
) {
    vec2 posM;
    posM.x = pos.x;
    posM.y = pos.y;
    vec4 rgbyM = textureLod(tex, posM, 0.0);

    float lumaS = FxaaLuma(textureLodOffset(tex, posM, 0.0, ivec2( 0, 1)));
    float lumaE = FxaaLuma(textureLodOffset(tex, posM, 0.0, ivec2( 1, 0)));
    float lumaN = FxaaLuma(textureLodOffset(tex, posM, 0.0, ivec2( 0,-1)));
    float lumaW = FxaaLuma(textureLodOffset(tex, posM, 0.0, ivec2(-1, 0)));
    float maxSM = max(lumaS, rgbyM.w);
    float minSM = min(lumaS, rgbyM.w);
    float maxESM = max(lumaE, maxSM);
    float minESM = min(lumaE, minSM);
    float maxWN = max(lumaN, lumaW);
    float minWN = min(lumaN, lumaW);
    float rangeMax = max(maxWN, maxESM);
    float rangeMin = min(minWN, minESM);
    float rangeMaxScaled = rangeMax * fxaaQualityEdgeThreshold;
    float range = rangeMax - rangeMin;
    float rangeMaxClamped = max(fxaaQualityEdgeThresholdMin, rangeMaxScaled);
    bool earlyExit = range < rangeMaxClamped;
    if(earlyExit)
        

            return rgbyM;
        
    float lumaNW = FxaaLuma(textureLodOffset(tex, posM, 0.0, ivec2(-1,-1)));
    float lumaSE = FxaaLuma(textureLodOffset(tex, posM, 0.0, ivec2( 1, 1)));
    float lumaNE = FxaaLuma(textureLodOffset(tex, posM, 0.0, ivec2( 1,-1)));
    float lumaSW = FxaaLuma(textureLodOffset(tex, posM, 0.0, ivec2(-1, 1)));
    float lumaNS = lumaN + lumaS;
    float lumaWE = lumaW + lumaE;
    float subpixRcpRange = 1.0/range;
    float subpixNSWE = lumaNS + lumaWE;
    float edgeHorz1 = (-2.0 * rgbyM.w) + lumaNS;
    float edgeVert1 = (-2.0 * rgbyM.w) + lumaWE;
    float lumaNESE = lumaNE + lumaSE;
    float lumaNWNE = lumaNW + lumaNE;
    float edgeHorz2 = (-2.0 * lumaE) + lumaNESE;
    float edgeVert2 = (-2.0 * lumaN) + lumaNWNE;
    float lumaNWSW = lumaNW + lumaSW;
    float lumaSWSE = lumaSW + lumaSE;
    float edgeHorz4 = (abs(edgeHorz1) * 2.0) + abs(edgeHorz2);
    float edgeVert4 = (abs(edgeVert1) * 2.0) + abs(edgeVert2);
    float edgeHorz3 = (-2.0 * lumaW) + lumaNWSW;
    float edgeVert3 = (-2.0 * lumaS) + lumaSWSE;
    float edgeHorz = abs(edgeHorz3) + edgeHorz4;
    float edgeVert = abs(edgeVert3) + edgeVert4;
    float subpixNWSWNESE = lumaNWSW + lumaNESE;
    float lengthSign = fxaaQualityRcpFrame.x;
    bool horzSpan = edgeHorz >= edgeVert;
    float subpixA = subpixNSWE * 2.0 + subpixNWSWNESE;
    if(!horzSpan) lumaN = lumaW;
    if(!horzSpan) lumaS = lumaE;
    if(horzSpan) lengthSign = fxaaQualityRcpFrame.y;
    float subpixB = (subpixA * (1.0/12.0)) - rgbyM.w;
    float gradientN = lumaN - rgbyM.w;
    float gradientS = lumaS - rgbyM.w;
    float lumaNN = lumaN + rgbyM.w;
    float lumaSS = lumaS + rgbyM.w;
    bool pairN = abs(gradientN) >= abs(gradientS);
    float gradient = max(abs(gradientN), abs(gradientS));
    if(pairN) lengthSign = -lengthSign;
    float subpixC = clamp(abs(subpixB) * subpixRcpRange, 0.0, 1.0);
    vec2 posB;
    posB.x = posM.x;
    posB.y = posM.y;
    vec2 offNP;
    offNP.x = (!horzSpan) ? 0.0 : fxaaQualityRcpFrame.x;
    offNP.y = ( horzSpan) ? 0.0 : fxaaQualityRcpFrame.y;
    if(!horzSpan) posB.x += lengthSign * 0.5;
    if( horzSpan) posB.y += lengthSign * 0.5;
    vec2 posN;
    posN.x = posB.x - offNP.x * 1.0;
    posN.y = posB.y - offNP.y * 1.0;
    vec2 posP;
    posP.x = posB.x + offNP.x * 1.0;
    posP.y = posB.y + offNP.y * 1.0;
    float subpixD = ((-2.0)*subpixC) + 3.0;
    float lumaEndN = FxaaLuma(textureLod(tex, posN, 0.0));
    float subpixE = subpixC * subpixC;
    float lumaEndP = FxaaLuma(textureLod(tex, posP, 0.0));
    if(!pairN) lumaNN = lumaSS;
    float gradientScaled = gradient * 1.0/4.0;
    float lumaMM = rgbyM.w - lumaNN * 0.5;
    float subpixF = subpixD * subpixE;
    bool lumaMLTZero = lumaMM < 0.0;
    lumaEndN -= lumaNN * 0.5;
    lumaEndP -= lumaNN * 0.5;
    bool doneN = abs(lumaEndN) >= gradientScaled;
    bool doneP = abs(lumaEndP) >= gradientScaled;
    if(!doneN) posN.x -= offNP.x * 1.5;
    if(!doneN) posN.y -= offNP.y * 1.5;
    bool doneNP = (!doneN) || (!doneP);
    if(!doneP) posP.x += offNP.x * 1.5;
    if(!doneP) posP.y += offNP.y * 1.5;
    if(doneNP) {
        if(!doneN) lumaEndN = FxaaLuma(textureLod(tex, posN.xy, 0.0));
        if(!doneP) lumaEndP = FxaaLuma(textureLod(tex, posP.xy, 0.0));
        if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
        if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
        doneN = abs(lumaEndN) >= gradientScaled;
        doneP = abs(lumaEndP) >= gradientScaled;
        if(!doneN) posN.x -= offNP.x * 2.0;
        if(!doneN) posN.y -= offNP.y * 2.0;
        doneNP = (!doneN) || (!doneP);
        if(!doneP) posP.x += offNP.x * 2.0;
        if(!doneP) posP.y += offNP.y * 2.0;
        
        if(doneNP) {
            if(!doneN) lumaEndN = FxaaLuma(textureLod(tex, posN.xy, 0.0));
            if(!doneP) lumaEndP = FxaaLuma(textureLod(tex, posP.xy, 0.0));
            if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
            if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
            doneN = abs(lumaEndN) >= gradientScaled;
            doneP = abs(lumaEndP) >= gradientScaled;
            if(!doneN) posN.x -= offNP.x * 4.0;
            if(!doneN) posN.y -= offNP.y * 4.0;
            doneNP = (!doneN) || (!doneP);
            if(!doneP) posP.x += offNP.x * 4.0;
            if(!doneP) posP.y += offNP.y * 4.0;
            
            if(doneNP) {
                if(!doneN) lumaEndN = FxaaLuma(textureLod(tex, posN.xy, 0.0));
                if(!doneP) lumaEndP = FxaaLuma(textureLod(tex, posP.xy, 0.0));
                if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
                if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
                doneN = abs(lumaEndN) >= gradientScaled;
                doneP = abs(lumaEndP) >= gradientScaled;
                if(!doneN) posN.x -= offNP.x * 12.0;
                if(!doneN) posN.y -= offNP.y * 12.0;
                doneNP = (!doneN) || (!doneP);
                if(!doneP) posP.x += offNP.x * 12.0;
                if(!doneP) posP.y += offNP.y * 12.0;
            }
            
        }
        
    }
    float dstN = posM.x - posN.x;
    float dstP = posP.x - posM.x;
    if(!horzSpan) dstN = posM.y - posN.y;
    if(!horzSpan) dstP = posP.y - posM.y;
    bool goodSpanN = (lumaEndN < 0.0) != lumaMLTZero;
    float spanLength = (dstP + dstN);
    bool goodSpanP = (lumaEndP < 0.0) != lumaMLTZero;
    float spanLengthRcp = 1.0/spanLength;
    bool directionN = dstN < dstP;
    float dst = min(dstN, dstP);
    bool goodSpan = directionN ? goodSpanN : goodSpanP;
    float subpixG = subpixF * subpixF;
    float pixelOffset = (dst * (-spanLengthRcp)) + 0.5;
    float subpixH = subpixG * fxaaQualitySubpix;
    float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;
    float pixelOffsetSubpix = max(pixelOffsetGood, subpixH);
    if(!horzSpan) posM.x += pixelOffsetSubpix * lengthSign;
    if( horzSpan) posM.y += pixelOffsetSubpix * lengthSign;
    

        return vec4(textureLod(tex, posM, 0.0).xyz, rgbyM.w);
    
}

layout(std140) uniform fxaa_params
{
    FXAAParams u_FXAA;
};

layout(location=0) in           vec2 ia_VertexPosition;
layout(location=3) in          vec2 ia_VertexTexcoord0;

out vec2 vs_Texcoord;

void main(void)
{
    vs_Texcoord = ia_VertexTexcoord0 * u_RS.ScreenTexcoord;
    gl_Position = vec4(ia_VertexPosition, 0.0, 1.0);
}
