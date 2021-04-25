#version 460 core

	out vec4 FragColor;
	
	in vec3 Normal;
	in vec3 FragPos; 
	in vec2 TexCoord;
	
	uniform sampler2D tex;
	
	void main()
	{
		// FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		FragColor = texture(tex, TexCoord);
	}
