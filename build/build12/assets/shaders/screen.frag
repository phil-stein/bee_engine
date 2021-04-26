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



vec3 run_kernel(float kernel[9], vec3 col);

void main()
{	
	// FragColor = texture(tex, TexCoord);
	// float avg = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
	// FragColor = vec4(avg, avg, avg, 1.0);  
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

	vec3 col = vec3(0.0);
	// col = run_kernel(sharpen_kernel, col);
	// col = run_kernel(blur_kernel, col);
	// col = run_kernel(edge_kernel, col);

	col = texture(tex, TexCoord).xyz;
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


