uniform sampler2D displacementMap;



#define textureSize 256.0
#define texelSize 1.0 / 256.0

vec4 texture2D_bilinear( uniform sampler2D tex, vec2 uv )
{
	vec2 f = fract( uv.xy * textureSize );
	vec4 t00 = texture2D( tex, uv );
	vec4 t10 = texture2D( tex, uv + vec2( texelSize, 0.0 ));
	vec4 tA = mix( t00, t10, f.x );
	vec4 t01 = texture2D( tex, uv + vec2( 0.0, texelSize ) );
	vec4 t11 = texture2D( tex, uv + vec2( texelSize, texelSize ) );
	vec4 tB = mix( t01, t11, f.x );
	return mix( tA, tB, f.y );
}

void main(void)
{
	vec4 newVertexPos;
	vec4 dv;
	float df;
	
	gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;
	
	//dv = texture2D_bilinear( displacementMap, gl_MultiTexCoord0.xy );
	dv = texture2D( displacementMap, gl_MultiTexCoord0.xy );

	df = 0.30*dv.x + 0.59*dv.y + 0.11*dv.z;
	
	newVertexPos = vec4(gl_Normal * df * 1.5, 0.0) + gl_Vertex;
	
	gl_Position = gl_ModelViewProjectionMatrix * newVertexPos;
}

