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
	
	struct material
	{
		vec3 tint;
		sampler2D diffuse;
	};

	uniform material mat;	
	uniform int just_tint;

	void main()
	{
		vec4 col;
		if (just_tint == 1)
		{
			col = vec4(mat.tint, 1.0);
		}
		else
		{
			col = texture(mat.diffuse, _in.tex_coords) * vec4(mat.tint, 1.0);
		}
		FragColor = col;
	}
