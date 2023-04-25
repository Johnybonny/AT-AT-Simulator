#version 330

out vec4 pixelColor; //Output variable. Almost final pixel color.

uniform sampler2D textureMap0;
uniform sampler2D textureMap1;

//Varying variables
in vec4 n;
in vec4 l;
in vec4 l2;
in vec4 v;
in vec2 iTexCoord0;


void main(void) {
	//Normalized, interpolated vectors
	vec4 ml = normalize(l);
	vec4 ml2 = normalize(l2);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);
	//Reflected vector
	vec4 mr = reflect(-ml, mn);
	vec4 mr2 = reflect(-ml2, mn);

	//Surface parameters
	vec4 kd = texture(textureMap0, iTexCoord0);
	vec4 ks = texture(textureMap1, iTexCoord0);

	//Lighting model computation
	float nl = clamp(dot(mn, ml), 0, 1);
	float rv = pow(clamp(dot(mr, mv), 0, 1), 50);

	float nl2 = clamp(dot(mn, ml2), 0, 1);
	float rv2 = pow(clamp(dot(mr2, mv), 0, 1), 50);

	pixelColor = (vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb * rv, 0)) + (vec4(kd.rgb * nl2, kd.a) + vec4(ks.rgb * rv2, 0));
}
