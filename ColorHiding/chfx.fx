// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4	mWorld;			// �����ϸ� * ȸ�� * �̵� ���
float4x4	mRT;			// �����ϸ��� ������ ȸ�� * �̵� ���
float4x4	mView;			// ��
float4x4	mProj;			// �������� 
float4x4	mWVP;			// ���� * �� * ��������
float4		vColor;			// ��
float4		vLight;			// ��
texture		tOrigin;		// ���� �ؽ�ó
float3		g_vLightView;	// View space light position/direction
float		g_fFarClip;		// Z of far clip plane
float4		g_vShadowColor;	// �׸��ڻ�
float4		Ambient;		// ��ü����Ʈ

sampler sOrigin = sampler_state{
Texture = (tOrigin);
 MinFilter = POINT;
 MagFilter = POINT;
 MipFilter = POINT;
};     // texture for scene rendering

float4 Green={0.0f,1.0f,0.0f,0.0f};
float4 Red={1.0f,0.0f,0.0f,0.0f};
float4 Blue={0.0f,0.0f,1.0f,0.0f};
float4 White={1.0f,1.0f,1.0f,0.0f};
float4 Black={0.0f,0.0f,0.0f,1.0f};

// -------------------------------------------------------------
// �ƿ�ǲ ���ؽ� ���� ����
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
    float2 Tex			: TEXCOORD0;
};


// -------------------------------------------------------------
// �� ���̵�
// -------------------------------------------------------------
VS_OUTPUT VSCell(
	float4 Pos	: POSITION,		// ������ġ��ǥ
	vector Normal	: NORMAL0,		// ��������
	float4	Color	: COLOR0,		// ��
	float2 OriTex	: TEXCOORD0		// �ؽ�ó uv
)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float4x4	mWV = mul(mWorld,mView);
	mWVP = mul(mWV,mProj);

    // ��ġ ��ȯ
    float4 PosView = mul( Pos, mWV );

    // ��ߺ��� �� �����̽��� ��ȯ	( �����ϸ� ���� ) 
    mWV = mul(mRT,mView);
    float3 N = mul( Normal, (float3x3)mWV );
    
    // Light-to-vertex vector in view space
    float3 LightVecView = PosView - g_vLightView;

	LightVecView = normalize(LightVecView);
	float u = dot( N, -LightVecView );
	
	
	if( u < 0.1f ){
		u = 0.3f;
	}else if( u < 0.50f ){
		u = 0.6f;
	}else{
		u = 1.0f;		
	}
    
    Out.Pos = mul( Pos, mWVP );
    Out.Tex = OriTex;
    Out.Color = vColor;
    Out.Color.w = u;
    
    return Out;
}

float4 PSCell(VS_OUTPUT In) : COLOR
{
	return (float4( tex2D( sOrigin, In.Tex ).xyz, 1.0f )*In.Color.w) * In.Color;
}

void VertScene( float4 vPos : POSITION,
                float3 vNormal : NORMAL,
                float2 vTex0 : TEXCOORD0,
                out float4 oPos : POSITION,
                out float4 ViewPos : TEXCOORD0,
                out float3 ViewNormal : TEXCOORD1,
                out float2 oTex0 : TEXCOORD2,
                out float4 oDiffuse : TEXCOORD3 )
{
	float4x4	mWV = mul(mWorld,mView);
	mWVP = mul(mWV,mProj);
	
    // Transform the position from view space to homogeneous projection space
    oPos = mul( vPos, mWVP );

    // Compute view space position
    ViewPos = mul( vPos, mWV );

    // Compute world space normal
    ViewNormal = normalize( mul( vNormal, (float3x3)mWV ) );

    // Modulate material with light to obtain diffuse
    oDiffuse = vColor * White;

    // Just copy the texture coordinate through
    oTex0 = vTex0;
}


float4 PixScene( float4 ViewPos : TEXCOORD0,
                 float3 ViewNormal : TEXCOORD1,
                 float2 Tex0 : TEXCOORD2,
                 float4 Diffuse : TEXCOORD3 ) : COLOR0
{
    // Pixel to light vector
    float3 L = g_vLightView - ViewPos;
    float LenSq = dot( L, L );
    L = normalize( L );

    // Compute lighting amount
    float4 abc = saturate( dot( normalize( ViewNormal ), L ) );

    if( abc.x < 0.5f){
		abc.x = 0.25f;
		abc.y = 0.25f;
		abc.z = 0.25f;
		abc.w = 0.25f;
    }else if(abc.x < 0.8f){
		abc.x = 0.5f;
		abc.y = 0.5f;
		abc.z = 0.5f;
		abc.w = 0.5f;
    }else{
		abc.x = 0.9f;
		abc.y = 0.9f;
		abc.z = 0.9f;
		abc.w = 0.9f;
    }
    abc *= Diffuse;
    
    // Lookup mesh texture and modulate it with diffuse
    return float4( tex2D( sOrigin, Tex0 ).xyz, 1.0f ) * abc;
}
// -------------------------------------------------------------
// ���� ������ ����
// -------------------------------------------------------------
VS_OUTPUT VSShadowVolume( float4 vPos : POSITION,
                       float3 vNormal : NORMAL )
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float4x4	mWV = mul(mWorld,mView);
	mWVP = mul(mWV,mProj);
    // ��ߺ��� �� �����̽��� ��ȯ
    float3 N = mul( vNormal, (float3x3)mWV );

    // ��ġ ��ȯ
    float4 PosView = mul( vPos, mWV );
	
    // Light-to-vertex vector in view space
    float3 LightVecView = PosView - g_vLightView;
  
    if( dot( N, -LightVecView ) < 0.001f )
    {
        if( PosView.z > g_vLightView.z ){
            PosView.xyz += LightVecView * ( g_fFarClip - PosView.z ) / LightVecView.z;
        }else{
            PosView = float4( LightVecView, 0.0f );
        }

        // �������� ��� ��ȯ
        Out.Pos = mul( PosView, mProj );
    } else{
		Out.Pos = mul( vPos, mWVP );
    }
    Out.Color	= Black;
    Out.Tex.x	= 0;
    Out.Tex.y	= 0;
    return Out;
}


float4 PSShadowVolume(VS_OUTPUT In) : COLOR0
{
    return float4( g_vShadowColor.xyz, 0.5f );
}

// -------------------------------------------------------------
// �ܰ��� ����
// -------------------------------------------------------------
VS_OUTPUT VSLine(
	float4 Pos	: POSITION		// ������ġ��ǥ
	)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
	float4x4 mWV = mul(mWorld,mView);
	float4x4 mWVP = mul(mWV,mProj);
	Out.Pos = mul(Pos,mWVP);

	return Out;
}

float4 PSLine(VS_OUTPUT In) : COLOR
{
	return Black;
}

// -------------------------------------------------------------
// �⺻ ����
// -------------------------------------------------------------
VS_OUTPUT VertSceneAmbient( float4 vPos : POSITION,
						float4	Color	: COLOR0,
                       float2 vTex0 : TEXCOORD0 )
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float4x4	mWV = mul(mWorld,mView);
	mWVP = mul(mWV,mProj);
    // ��ǥ ��ȯ
    Out.Pos = mul( vPos, mWVP );

    Out.Tex = vTex0;
    
    Out.Color = vColor;
    
    return Out;
}


float4 PixSceneAmbient( VS_OUTPUT In ) : COLOR0
{
    return Ambient * tex2D( sOrigin, In.Tex ) * In.Color;
}

// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
// ���� ����
technique Cell
{
    pass P0
    {
        VertexShader = compile vs_2_0 VertScene();
        PixelShader  = compile ps_2_0 PixScene();
		//VertexShader = compile vs_2_0 VSCell();
		//PixelShader  = compile ps_2_0 PSCell();
        
		ZEnable = true;
		ZFunc = LessEqual;
		StencilEnable = true;
		AlphaBlendEnable = true;
		BlendOp = ADD;
		SrcBlend = One;
		DestBlend = 3;
		StencilRef = 1;
		StencilFunc = Greater;
		StencilPass = Keep;        
    }
}
// Line ����
technique Liner
{
    pass P0
    {
        VertexShader = compile vs_2_0 VSLine();
        PixelShader  = compile ps_2_0 PSLine();
        CullMode = CW;
        AlphaBlendEnable = false;
        ZEnable = true;
		ZFunc = LessEqual;
    }
}
// ���� ���̵�
technique RenderShadowVolume
{
    pass P0
    {
        VertexShader = compile vs_2_0 VSShadowVolume();
        PixelShader  = compile ps_2_0 PSShadowVolume();
        CullMode = Ccw;
        // Disable writing to the frame buffer
        AlphaBlendEnable = true;
        SrcBlend = Zero;
        DestBlend = One;
        // Disable writing to depth buffer
        ZWriteEnable = false;
        ZFunc = Less;
        // Setup stencil states
        StencilEnable = true;
        StencilRef = 1;
        StencilMask = 0xFFFFFFFF;
        StencilWriteMask = 0xFFFFFFFF;
        StencilFunc = Always;
        StencilZFail = Decr;
        StencilPass = Keep;
    }
    pass P1
    {
        VertexShader = compile vs_2_0 VSShadowVolume();
        PixelShader  = compile ps_2_0 PSShadowVolume();
        CullMode = Cw;
        StencilZFail = Incr;
    }
}

// ���� ���̵� 2Sied
technique RenderShadowVolume2Sided
{
    pass P0
    {
        VertexShader = compile vs_2_0 VSShadowVolume();
        PixelShader  = compile ps_2_0 PSShadowVolume();
        CullMode = None;
        // ����Ű�� ���� �κ��� zero ���� ���� one
        AlphaBlendEnable = true;
        SrcBlend = Zero;
        DestBlend = One;
        // Z���� ���� ����
        ZWriteEnable = false;
        ZFunc = Less;
        // ���ٽ� ���� ����
        TwoSidedStencilMode = true;
        StencilEnable = true;
        StencilRef = 1;
        StencilMask = 0xFFFFFFFF;
        StencilWriteMask = 0xFFFFFFFF;
        Ccw_StencilFunc = Always;
        Ccw_StencilZFail = Incr;
        Ccw_StencilPass = Keep;
        StencilFunc = Always;
        StencilZFail = Decr;
        StencilPass = Keep;
    }
}

technique RenderSceneAmbient
{
    pass P0
    {
        VertexShader = compile vs_2_0 VertSceneAmbient();
        PixelShader  = compile ps_2_0 PixSceneAmbient();
        StencilEnable = false;
        ZFunc = LessEqual;
    }
}

technique RenderSceneAmbientWithAlpha
{
    pass P0
    {
        VertexShader = compile vs_2_0 VertSceneAmbient();
        PixelShader  = compile ps_2_0 PixSceneAmbient();
        AlphaBlendEnable = true;
        SrcBlend = ZERO;
        DestBlend = One;
        StencilEnable = false;
        ZFunc = LessEqual;
    }
}