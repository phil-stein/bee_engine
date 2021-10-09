#version 460 core
    out vec4 FragColor;

    //passed from vertex-shader
    in VS_OUT
    {
        vec2 tex_coords;
        vec3 frag_pos;
        vec3 normal;
        mat3 TBN;
        // vec4 frag_pos_light_space;
    } _in;

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
        	FragColor = vec4(_in.normal.xyz, 1.0);
		}
		// uv mode
		if (mode == 2)
		{
			FragColor = vec4(_in.tex_coords.xy, 0.0, 1.0);	
		}

    }