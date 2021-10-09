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

        int use_shadow;
        sampler2D shadow_map;
        mat4 light_space;
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
    // in vec3 Normal;
    // in vec3 FragPos;
    // in vec2 TexCoord;
    // in vec4 frag_pos_light_space;

    //passed from vertex-shader
    in VS_OUT
    {
        vec2 tex_coords;
        vec3 frag_pos;
        vec3 normal;
        mat3 TBN;
        // vec4 frag_pos_light_space;
    } _in;

    //uniforms

    uniform Material material;

    uniform int Num_DirLights;
    uniform DirectionalLight dirLights[6];
    
    uniform int Num_PointLights;
    uniform PointLight pointLights[32];
        
    uniform int Num_SpotLights;
    uniform SpotLight spotLights[16];

    uniform vec3 viewPos;

    //function prototypes (they need to be declared before being called like in c)
    float calc_shadow(DirectionalLight light, vec3 normal);
    vec3 CalcDirectionalLight(DirectionalLight light, vec2 texCoords, vec3 normal, vec3 viewDir, float shadow);
    vec3 CalcPointLight(PointLight light, vec2 texCoords, vec3 normal, vec3 viewDir, float shadow);
    vec3 CalcSpotLight(SpotLight light, vec2 texCoords, vec3 normal, vec3 viewDir, float shadow);

    void main() 
    {

        //scale the texcoords to fit the specified tiling
        vec2 norm_tex_coords = material.tile * _in.tex_coords;

        //get surface normal and the dir the light is coming from
        vec3 normal;
        if (texture(material.normal, norm_tex_coords).rgb == vec3(1.0, 1.0, 1.0))
        {
            normal = normalize(_in.normal);
        }
        else
        {
            normal = texture(material.normal, norm_tex_coords).rgb;
            normal = normalize(normal * 2.0 - 1.0);
            normal = normalize(_in.TBN * normal);
        }

        //get the angle between the reflected light-ray and the view-direction        
        vec3 view_dir = normalize(viewPos - _in.frag_pos);
     
        vec3 result = vec3(0.0);
        
        // calc the shadow, 0 = shadow, 1 = light
        float shadow = 0.0;
        int   deactivated_shadows = 0;
        for(int i = 0; i < Num_DirLights; i++)
        {
            if (dirLights[i].use_shadow == 0) // set to false
            {
                deactivated_shadows++;
            }
            else 
            {
                shadow += calc_shadow(dirLights[i], normal);
            }
        }
        if ((Num_DirLights - deactivated_shadows) <= 0)
        {
            // no shadow casting lights
            shadow = 1.0;     
        }
        else
        {
            shadow /= (Num_DirLights - deactivated_shadows);
        }
        

        for(int i = 0; i < Num_DirLights; i++)
        {
            result += CalcDirectionalLight(dirLights[i], norm_tex_coords, normal, view_dir, shadow);
        }
        
        for(int i = 0; i < Num_PointLights; i++)
        {
            result += CalcPointLight(pointLights[i], norm_tex_coords, normal, view_dir, shadow);
        }

        for(int i = 0; i < Num_SpotLights; i++)
        {
            result += CalcSpotLight(spotLights[i], norm_tex_coords, normal, view_dir, shadow);
        }

        // result = FragPos; // vec3(frag_pos_light_space.w); // frag_pos_light_space.rgb; // vec3(shadow);

        float transparency = texture(material.diffuse, norm_tex_coords).a;
	    if(transparency < 0.1)         
	    { discard; }
	    result *= material.tint;
        FragColor =  vec4(result, transparency); // vec4(vec3(shadow), 1.0); //vec4(result, texture(material.diffuse, normTexCoords).w); //* ourTexture; //vec3(norm.xyz); 
    }

    float calc_shadow(DirectionalLight light, vec3 normal)
    {
        vec4 frag_pos_light_space = light.light_space * vec4(_in.frag_pos, 1.0);
        // perspective divide
        vec3 proj_coord = frag_pos_light_space.xyz / frag_pos_light_space.w;
        // from range [-1, 1] to [0, 1]
        proj_coord = (proj_coord * 0.5) + 0.5;

        // sample depth from shadow map
        // float closest_depth = texture(shadow_map, proj_coord.xy).r;
        
        vec3 light_dir = light.direction;
        float bias = max(0.01 * (-dot(normal, light_dir)), 0.001); // fighting shadow acne
        
        // proj_coord.z is the current depth
        // float result = proj_coord.z - bias > closest_depth ? 0.05 : 1.0;
        // return  proj_coord.z > 1.0 ? 1.0 : result; // discard frags outside the projected shadow map
        
        float shadow = 0.0;
        vec2 texel_size = 1.0 / textureSize(light.shadow_map, 0);
        for(float x = -1; x <= 1; x += 0.5)
        {
            for(float y = -1; y <= 1; y += 0.5)
            {
                float pcf_depth = texture(light.shadow_map, proj_coord.xy + vec2(x, y) * texel_size).r; 
                // proj_coord.z is the current depth
                shadow += proj_coord.z - bias > pcf_depth ? 0.05 : 1.0;        
            }    
        }
        shadow /= 36.0;

        return proj_coord.z > 1.0 ? 1.0 : shadow; // discard frags outside the projected shadow map
    }

    vec3 CalcDirectionalLight(DirectionalLight light, vec2 texCoords, vec3 normal, vec3 viewDir, float shadow)
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

        return (ambient + (diffuse * shadow) + (specular * shadow));

    }

    vec3 CalcPointLight(PointLight light, vec2 texCoords, vec3 normal, vec3 viewDir, float shadow)
    {
        //explanaition: https://learnopengl.com/Lighting/Light-casters, LearnOpenGL page 141

        float dist = length(light.position - _in.frag_pos);
        float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist); // (dist * dist) 

        //diffuse----------------------------------
        //get surface normal and the dir the light is coming from
        vec3 lightDir = normalize(light.position - _in.frag_pos);

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


        return ((ambient * attenuation) + (diffuse * attenuation * shadow) + (specular * attenuation * shadow));
    }

    vec3 CalcSpotLight(SpotLight light, vec2 texCoords, vec3 normal, vec3 viewDir, float shadow)
    {
        //explanaition: https://learnopengl.com/Lighting/Light-casters, LearnOpenGL page 143

        //vec2 normTexCoords = material.tile * TexCoord;
        vec2 normTexCoords = texCoords;
        //diffuse----------------------------------
        //get surface normal and the dir the light is coming from
        vec3 lightDir = normalize(light.position - _in.frag_pos);

        //angle between spotlight-direction and the vector from the fragment to the light
        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.00, 1.0);

        //dot product between surface-normal and light-dir, the clamped to get a value between 0-1, would otherwise be neg. if the angle was greater than 90° 
        float diff = max(dot(normal, lightDir), 0.0);
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
        float distance    = length(light.position - _in.frag_pos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance); // (distance * distance)
        ambient  *= attenuation; 
        diffuse   *= attenuation;
        specular *= attenuation; 

        return (ambient + (diffuse * intensity * shadow) + (specular * intensity * shadow));
    }
