#version 460 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;  	// color buffer
uniform sampler2D outline;	// outline buffer

uniform float exposure;

const float offset = 1.0 / 300.0;


	vec2 offsets[9] = vec2[](
    	vec2(-offset,  offset), // top-left
    	vec2( 0.0f,    offset), // top-center
    	vec2( offset,  offset), // top-right
    	vec2(-offset,  0.0f),   // center-left
    	vec2( 0.0f,    0.0f),   // center-center
    	vec2( offset,  0.0f),   // center-right
    	vec2(-offset, -offset), // bottom-left
    	vec2( 0.0f,   -offset), // bottom-center
    	vec2( offset, -offset)  // bottom-right    
	);

float sharpen_kernel[9] = float[](
	-1, -1, -1,
	-1,  9, -1,
	-1, -1, -1
);
float blur_kernel[9] = float[](     
	1.0 / 16, 2.0 / 16, 1.0 / 16,     
	2.0 / 16, 4.0 / 16, 2.0 / 16,     
	1.0 / 16, 2.0 / 16, 1.0 / 16 
);
float edge_kernel[9] = float[](
    	1, 1, 1,
    	1,-8, 1,
    	1, 1, 1
	);


// func declarations
vec3 aces_tone_mapping(vec3 col);
vec3 run_kernel(float kernel[9], vec3 col);
vec3 avg_color(vec3 col);
vec3 greater_than_col(vec3 col);


void main()
{	
	vec3 col = vec3(0.0);
	// col = run_kernel(sharpen_kernel, col);
	// col = run_kernel(blur_kernel, col);
	// col = run_kernel(edge_kernel, col);
	// col = avg_color(col);
	// col = greater_than_col(col);
	// if ((col.r + col.g + col.b) == 0.0)
	// {
	//	col = texture(tex, TexCoord).xyz;
	//	col = avg_color(col);
	//}

	// reinhard tone mapping
	vec3 col_hdr = texture(tex, TexCoord).rgb;
	// col = vec3(1.0) - exp(-col_hdr * exposure);
	col = aces_tone_mapping(col_hdr);

	// gamma correction
	// float inv_gamma = 0.4545454545454545; // 1/2.2
	float gamma = 2.2;
	col = pow(col, vec3(1 / gamma));

	// ---- outline ----
	const float width = 1.0 / 800.0; 
	const vec3  outline_color = vec3(11.0f / 255.0f, 1.0, 249.0f / 255.0f);
	int use_outline_color = 0;
	float x = TexCoord.x;
	float y = TexCoord.y;
	vec2 coord = vec2(x, y);
	

	// sample around the current fragment and check if outside the mesh

	float base = texture(outline, coord).r;
	x = TexCoord.x + width;
	y = TexCoord.y;
	coord = vec2(x, y);
	float c = texture(outline, coord).r;
	if (c > 0.0 && base == 0.0) { use_outline_color = 1; }

	x = TexCoord.x - width;
	y = TexCoord.y;
	coord = vec2(x, y);
	c = texture(outline, coord).r;
	if (c > 0.0 && base == 0.0) { use_outline_color = 1; }

	x = TexCoord.x;
	y = TexCoord.y + width;
	coord = vec2(x, y);
	c = texture(outline, coord).r;
	if (c > 0.0 && base == 0.0) { use_outline_color = 1; }

	x = TexCoord.x;
	y = TexCoord.y - width;
	coord = vec2(x, y);
	c = texture(outline, coord).r;
	if (c > 0.0 && base == 0.0) { use_outline_color = 1; }

	x = TexCoord.x + width;
	y = TexCoord.y + width;
	coord = vec2(x, y);
	c = texture(outline, coord).r;
	if (c > 0.0 && base == 0.0) { use_outline_color = 1; }

	x = TexCoord.x - width;
	y = TexCoord.y - width;
	coord = vec2(x, y);
	c = texture(outline, coord).r;
	if (c > 0.0 && base == 0.0) { use_outline_color = 1; }


	if (use_outline_color == 1)
	{
		col = outline_color;
	}


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

vec3 run_kernel(float kernel[9], vec3 col)
{
	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++)
	{
    	sampleTex[i] = vec3(texture(outline, TexCoord.st + offsets[i]));
	}

	for(int i = 0; i < 9; i++)
	{
        	col += sampleTex[i] * kernel[i];
	}

	return col;
}
vec3 avg_color(vec3 col)
{

	float avg = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
	return vec3(avg, avg, avg);  
	
}
vec3 greater_than_col(vec3 col)
{
	if ((col.r + col.g + col.b) * 0.33 >= 0.9)
	{
		col = vec3(1.0);
	}
	else
	{
		col = vec3(0.0);
	}

	return col;
}

