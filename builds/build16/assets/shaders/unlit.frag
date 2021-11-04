#version 460 core

	out vec4 FragColor;
	
	in vec3 Normal;
	in vec3 FragPos; 
	in vec2 TexCoord;
	
	struct Material
	{
		vec3 tint;
	};

	uniform Material material;	

	void main()
	{
		FragColor = vec4(material.tint, 1.0);
		// FragColor = texture(tex, TexCoord);
	}
