uniform vec4 FogColor;
uniform float FogEnd;
uniform float FogScale;

uniform vec3 MaterialEmit;
uniform float MaterialOpacity;

uniform sampler2D Texture[8];

varying vec3 position, normal;


void main(void)
{
	vec3 N = normalize(normal);

	float light = 0.5;// + max(0.0, pow(dot(vec3(1.0, 0.0, 0.0), N)*3.0, 4.0))*0.5;

	float v = 0.5;//dot(normalize(-position), N);
	v = clamp(pow(v*3.0, 20.0), 0.0, 1.0)*light;
	gl_FragColor = vec4(v, v, v, 1.0);
}
