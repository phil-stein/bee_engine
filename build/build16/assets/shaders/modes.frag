#version 460 core
    out vec4 FragColor;

    in vec3 Normal;
    in vec3 FragPos; 
    in vec2 TexCoord;

    uniform int mode;
    uniform vec3 wiref_col;

    void main() 
    {
	// wireframe mode
	if (mode == 0)
	{
        	FragColor = vec4(wiref_col, 1.0);
	}
	// normal mode
	if (mode == 1)
	{
        	FragColor = vec4(Normal.xyz, 1.0);
	}
	// uv mode
	if (mode == 2)
	{
		FragColor = vec4(TexCoord.xy, 0.0, 1.0);	
	}

    }