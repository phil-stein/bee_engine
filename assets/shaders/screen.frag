#version 460 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;  	// color buffer

uniform float exposure;
	

// func declarations
vec3 aces_tone_mapping(vec3 col);

void main()
{	

	// tone mapping
	vec4 col_hdr = texture(tex, TexCoord).rgba;
	vec3 col = aces_tone_mapping(col_hdr.rgb);

	// gamma correction
	// float inv_gamma = 0.4545454545454545; // 1/2.2
	float gamma = 2.2;
	col = pow(col, vec3(1 / gamma));


	FragColor = vec4(col, col_hdr.a);

}

// taken from https://64.github.io/tonemapping/
vec3 aces_tone_mapping(vec3 col)
{
	col *= 0.6 * exposure;
	const float a = 2.51; 
	const float b = 0.03; 
	const float c = 2.43; 
	const float d = 0.59; 
	const float e = 0.14;
	return clamp((col * (a * col + b)) / (col * (c * col + d) +e), 0.0, 1.0); 
}
