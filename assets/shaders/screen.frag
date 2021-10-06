#version 460 core

	out vec4 FragColor;
	
	in vec2 TexCoord;
	
	uniform sampler2D tex;
	
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


// func declarrations
vec3 run_kernel(float kernel[9], vec3 col);
vec3 avg_color(vec3 col);
vec3 greater_than_col(vec3 col);


void main()
{	
	// FragColor = texture(tex, TexCoord);
	// float avg = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
	// FragColor = vec4(avg, avg, avg, 1.0);  
	
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

	float inv_gamma = 0.4545454545454545; // 1/2.2
	col = texture(tex, TexCoord).xyz;
	col = pow(col, vec3(inv_gamma)); // gamma correction
    	FragColor = vec4(col, 1.0);

}


vec3 run_kernel(float kernel[9], vec3 col)
{
    	vec3 sampleTex[9];
    	for(int i = 0; i < 9; i++)
    	{
        	sampleTex[i] = vec3(texture(tex, TexCoord.st + offsets[i]));
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

