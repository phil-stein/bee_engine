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
	
	struct Material
	{
		vec3 tint;
		sampler2D diffuse;
	};

	uniform Material material;	

	void main()
	{
		// FragColor = vec4(material.tint, 1.0);
		FragColor = texture(material.diffuse, _in.tex_coords);
	}
