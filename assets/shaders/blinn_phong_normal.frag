#version 460 core

    struct Material {
        sampler2D diffuse;
        sampler2D specular;
        sampler2D normal;
        float shininess;
        vec2 tile;
	vec3 tint;
    };

    struct DirectionalLight {
        vec3 direction;
  
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };
    struct PointLight {
        vec3 position;
  
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;

        float constant;
        float linear;
        float quadratic;
    };
    struct SpotLight {
        vec3 position;
        vec3 direction;
  
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;

        float cutOff;
        float outerCutOff;

        float constant;
        float linear;
        float quadratic;
    };
    
    //fragment/pixel color after calculations
    out vec4 FragColor;
    
    //passed from vertex-shader
    in VS_OUT
    {
        vec2 tex_coords;
        vec3 frag_pos;
        mat3 TBN;
        vec4 frag_pos_light_space;
    } fs_in;

    //uniforms
    uniform Material material;

    uniform int Num_DirLights;
    uniform DirectionalLight dirLights[4];
    
    uniform int Num_PointLights;
    uniform PointLight pointLights[32];
        
    uniform int Num_SpotLights;
    uniform SpotLight spotLights[16];

    uniform vec3 viewPos;

    //function prototypes (they need to be declared before being called like in c)
    vec3 calc_directional_light(DirectionalLight light, vec2 texCoords, vec3 normal, vec3 viewDir);
    vec3 calc_point_light(PointLight light, vec2 texCoords, vec3 normal, vec3 viewDir);
    vec3 calc_spot_light(SpotLight light, vec2 texCoords, vec3 normal, vec3 viewDir);

    void main() 
    {

        //scale the texcoords to fit the specified tiling
        vec2 normTexCoords = material.tile * fs_in.tex_coords;

        //get surface normal and the dir the light is coming from
        // vec3 norm = normalize(Normal);
        vec3 normal = texture(material.normal, normTexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(fs_in.TBN * normal);

        //get the angle between the reflected light-ray and the view-direction        
        vec3 viewDir = normalize(viewPos - fs_in.frag_pos);
     
        vec3 result = vec3(0.0);
        
        for(int i = 0; i < Num_DirLights; i++)
        {
            result += calc_directional_light(dirLights[i], normTexCoords, normal, viewDir);
        }
        
        for(int i = 0; i < Num_PointLights; i++)
        {
            result += calc_point_light(pointLights[i], normTexCoords, normal, viewDir);
        }

        for(int i = 0; i < Num_SpotLights; i++)
        {
            result += calc_spot_light(spotLights[i], normTexCoords, normal, viewDir);
        }

        float transparency = texture(material.diffuse, normTexCoords).a;
	    if(transparency < 0.1)         
	    { discard; }
	    result *= material.tint;
        FragColor = vec4(result, transparency); //vec4(result, texture(material.diffuse, normTexCoords).w); //* ourTexture; //vec3(norm.xyz); 
    }

    vec3 calc_directional_light(DirectionalLight light,vec2 texCoords, vec3 normal, vec3 viewDir)
    {
        //explanaition: https://learnopengl.com/Lighting/Light-casters, LearnOpenGL page 137

        //diffuse----------------------------------
        // vec3 lightDir = normalize(light.position - FragPos);
        vec3 lightDir = normalize(-light.direction);

        //dot product between surface-normal and light-dir, the clamped to get a value between 0-1, would otherwise be neg. if the angle was greater than 90° 
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));

        //ambient----------------------------------
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords)); //light.ambient * material.ambient;

        //specular----------------------------------
        // vec3 reflectDir = reflect(-lightDir, normal); //lightDir negated, because reflect() wants a Vec3 pointing from the light-source toward the fragment

        //the shininess-value dictates how focused the spot of reflected light is
        // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
        // vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
	    vec3 halfwayDir = normalize(lightDir + viewDir);
	    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess * 128);
	    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));

        return (ambient + diffuse + specular);
    }

    vec3 calc_point_light(PointLight light, vec2 texCoords, vec3 normal, vec3 viewDir)
    {
        //explanaition: https://learnopengl.com/Lighting/Light-casters, LearnOpenGL page 141

        float dist = length(light.position - fs_in.frag_pos);
        float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist); // (dist * dist) 

        //diffuse----------------------------------
        //get surface normal and the dir the light is coming from
        vec3 lightDir = normalize(light.position - fs_in.frag_pos);

        //dot product between surface-normal and light-dir, the clamped to get a value between 0-1, would otherwise be neg. if the angle was greater than 90° 
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));

        //ambient----------------------------------
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords)); //light.ambient * material.ambient;

        //specular----------------------------------
        // vec3 reflectDir = reflect(-lightDir, normal); //lightDir negated, because reflect() wants a Vec3 pointing from the light-source toward the fragment

        //the shininess-value dictates how focused the spot of reflected light is
        // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
        // vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
	    vec3 halfwayDir = normalize(lightDir + viewDir);
	    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess * 128);
	    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));


        return ((ambient * attenuation) + (diffuse * attenuation) + (specular * attenuation));
    }

    vec3 calc_spot_light(SpotLight light, vec2 texCoords, vec3 normal, vec3 viewDir)
    {
        //explanaition: https://learnopengl.com/Lighting/Light-casters, LearnOpenGL page 143

        //vec2 normTexCoords = material.tile * TexCoord;
        vec2 normTexCoords = texCoords;
        //diffuse----------------------------------
        //get surface normal and the dir the light is coming from
        vec3 norm = normalize(normal);
        vec3 lightDir = normalize(light.position - fs_in.frag_pos);

        //angle between spotlight-direction and the vector from the fragment to the light
        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.00, 1.0);

        //dot product between surface-normal and light-dir, the clamped to get a value between 0-1, would otherwise be neg. if the angle was greater than 90° 
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, normTexCoords));

        //ambient----------------------------------
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, normTexCoords)); //light.ambient * material.ambient;

        //specular----------------------------------
        //get the angle betwee the reflected light-ray and the view-direction        
        // vec3 reflectDir = reflect(-lightDir, norm); //lightDir negated, because reflect() wants a Vec3 pointing from the light-source toward the fragment

        //the shininess-value dictates how focused the spot of reflected light is
        // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
        // vec3 specular = light.specular * spec * vec3(texture(material.specular, normTexCoords));
	    vec3 halfwayDir = normalize(lightDir + viewDir);
	    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess * 128);
	    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));

        //attenuation
        float distance    = length(light.position - fs_in.frag_pos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance); // (distance * distance)
        ambient  *= attenuation; 
        diffuse   *= attenuation;
        specular *= attenuation; 

        return (ambient + (diffuse * intensity) + (specular * intensity));
    }
