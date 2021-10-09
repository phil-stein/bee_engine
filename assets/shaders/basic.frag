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
	
	uniform sampler2D tex;
	
	void main()
	{
		// FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		FragColor = texture(tex, _in.tex_coords);
	}
