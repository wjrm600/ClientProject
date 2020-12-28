matrix		g_matWorld;
matrix		g_matView;
matrix		g_matProj;

texture		g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;

	minfilter = linear;
	magfilter = linear;

};

texture g_DepthTexture;

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};

struct	VS_IN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct	VS_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	vector		vProjPos : TEXCOORD1;
};

VS_OUT		VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float2 vTexUV : TEXCOORD0;
	vector vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	vector vColor : COLOR0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	float2		vDepthUV = (float2)0.f;

	// �켱 z�����Ⱑ ���� ���� ��ǥ�� ��ȯ -> �ؽ�ó UV��ǥ�� ��ȯ
	
	// (-1 ~ 1) ������ ���� ��ǥ x�� (0 ~ 1) ������ �ؽ��� U ������ ��ȯ
	vDepthUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	// (1 ~ -1)������ ���� ��ǥ y�� (0 ~ 1) ������ �ؽ��� V ������ ��ȯ
	vDepthUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float fViewZ = tex2D(DepthSampler, vDepthUV).y * 1000.f;

	Out.vColor.a = Out.vColor.a * saturate(fViewZ - In.vProjPos.w);

	return Out;
}

technique Default_Device
{
	pass
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

	
		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_MAIN();
	}

};