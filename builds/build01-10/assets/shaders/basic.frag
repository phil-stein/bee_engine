#version 460 core

	out vec4 FragColor;
	
	in vec3 Normal;
    in vec3 FragPos; 
	in vec2 TexCoord;
	
	uniform sampler2D ourTexture;
	
	void main()
	{
		// why wont you fckn render !?!?!?!?
		// FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		FragColor = texture(ourTexture, TexCoord); //vec4(1.0f, 0.5f, 0.2f, 1.0f);
	}