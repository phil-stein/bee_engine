#version 460 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;  	// color buffer

uniform float exposure;
	

// func declarations
vec3 aces_tone_mapping(vec3 col);

void main()
{	
	vec3 col = vec3(0.0);

	// reinhard tone mapping
	vec3 col_hdr = texture(tex, TexCoord).rgb;
	// col = vec3(1.0) - exp(-col_hdr * exposure);
	col = aces_tone_mapping(col_hdr);

	// gamma correction
	// float inv_gamma = 0.4545454545454545; // 1/2.2
	float gamma = 2.2;
	col = pow(col, vec3(1 / gamma));


	FragColor = vec4(col, 1.0);

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
