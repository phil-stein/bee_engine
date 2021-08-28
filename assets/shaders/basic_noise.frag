#version 460 core

	out vec4 FragColor;
	
	in vec3 Normal;
	in vec3 FragPos; 
	in vec2 TexCoord;
	
	uniform sampler2D tex;
	
	// func decl
	float rand(float n);
	float noise_a(int resolution);
	float noise_b(float p);
	float noise_c(vec2 n);

	void main()
	{
		// FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		//FragColor = texture(tex, TexCoord);
		// float val = noise(FragPos.xy);
		float val = noise_a(50);
		// val = noise_b(val);
		// val = noise_c(TexCoord);
	/*	
		if (val > 0.5)
		{
			val = 1.0;
		}
		else 
		{
			val = 0.0;
		}
	*/
		FragColor = vec4(val, val, val, 1.0);
	}


	float rand(float n) { return fract(sin(n) * 43758.5453123); }
	float noise_a(int resolution) { return (rand(round(TexCoord.x * resolution)) + rand(round(TexCoord.y * resolution))) * 0.5; }
	float noise_b(float p)
	{  
	 	float fl = floor(p);   
		float fc = fract(p);  
		return mix(rand(fl), rand(fl + 1.0), fc); 
	}

	float noise_c(vec2 n) 
	{  
		const vec2 d = vec2(0.0, 1.0);   
		float b_val = floor(n.x + n.y); vec2 b = vec2(b_val, b_val);
		vec2 f = smoothstep(vec2(0.0), vec2(1.0), fract(n));  
		return mix(mix(rand(b.x + b.y), rand(b.x + b.y + d.y + d.x), f.x), mix(rand(b.x + b.y + d.x + d.y), rand(b.x + b.y + d.y + d.y), f.x), f.y); 
	}





