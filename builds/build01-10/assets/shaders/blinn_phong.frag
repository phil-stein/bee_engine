#version 460 core

    struct Material {
        sampler2D diffuse;
        sampler2D specular;
        float shininess;
        vec2 tile;
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
    in vec3 Normal;
    in vec3 FragPos;
    in vec2 TexCoord;

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
    vec3 CalcDirectionalLight(DirectionalLight light, vec2 texCoords, vec3 normal, vec3 viewDir);
    vec3 CalcPointLight(PointLight light, vec2 texCoords, vec3 normal, vec3 viewDir);
    vec3 CalcSpotLight(SpotLight light, vec2 texCoords, vec3 normal, vec3 viewDir);

    void main() 
    {

        //scale the texcoords to fit the specified tiling
        vec2 normTexCoords = material.tile * TexCoord;

        //get surface normal and the dir the light is coming from
        vec3 norm = normalize(Normal);

        //get the angle between the reflected light-ray and the view-direction        
        vec3 viewDir = normalize(viewPos - FragPos);
     
        vec3 result = vec3(0.0);
        
        for(int i = 0; i < Num_DirLights; i++)
        {
            result += CalcDirectionalLight(dirLights[i], normTexCoords, norm, viewDir);
        }
        
        for(int i = 0; i < Num_PointLights; i++)
        {
            result += CalcPointLight(pointLights[i], normTexCoords, norm, viewDir);
        }

        for(int i = 0; i < Num_SpotLights; i++)
        {
            result += CalcSpotLight(spotLights[i], normTexCoords, norm, viewDir);
        }

        float trasparency = texture(material.diffuse, normTexCoords).a;
        FragColor = vec4(result, trasparency); //vec4(result, texture(material.diffuse, normTexCoords).w); //* ourTexture; //vec3(norm.xyz); 
    }

    vec3 CalcDirectionalLight(DirectionalLight light,vec2 texCoords, vec3 normal, vec3 viewDir)
    {
        //explanaition: https://learnopengl.com/Lighting/Light-casters, LearnOpenGL page 137

        //diffuse----------------------------------
        //vec3 lightDir = normalize(light.position - FragPos);
        vec3 lightDir = normalize(-light.direction);

        //dot product between surface-normal and light-dir, the clamped to get a value between 0-1, would otherwise be neg. if the angle was greater than 90° 
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));

        //ambient----------------------------------
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords)); //light.ambient * material.ambient;

        //specular----------------------------------
        vec3 reflectDir = reflect(-lightDir, normal); //lightDir negated, because reflect() wants a Vec3 pointing from the light-source toward the fragment

        //the shininess-value dictates how focused the spot of reflected light is
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
        vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));

        return (ambient + diffuse + specular);

    }

    vec3 CalcPointLight(PointLight light, vec2 texCoords, vec3 normal, vec3 viewDir)
    {
        //explanaition: https://learnopengl.com/Lighting/Light-casters, LearnOpenGL page 141

        float dist = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist)); 

        //diffuse----------------------------------
        //get surface normal and the dir the light is coming from
        vec3 lightDir = normalize(light.position - FragPos);

        //dot product between surface-normal and light-dir, the clamped to get a value between 0-1, would otherwise be neg. if the angle was greater than 90° 
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));

        //ambient----------------------------------
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords)); //light.ambient * material.ambient;

        //specular----------------------------------
        vec3 reflectDir = reflect(-lightDir, normal); //lightDir negated, because reflect() wants a Vec3 pointing from the light-source toward the fragment

        //the shininess-value dictates how focused the spot of reflected light is
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
        vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));

        return ((ambient * attenuation) + (diffuse * attenuation) + (specular * attenuation));
    }

    vec3 CalcSpotLight(SpotLight light, vec2 texCoords, vec3 normal, vec3 viewDir)
    {
        //explanaition: https://learnopengl.com/Lighting/Light-casters, LearnOpenGL page 143

        //vec2 normTexCoords = material.tile * TexCoord;
        vec2 normTexCoords = texCoords;
        //diffuse----------------------------------
        //get surface normal and the dir the light is coming from
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position - FragPos);

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
        vec3 reflectDir = reflect(-lightDir, norm); //lightDir negated, because reflect() wants a Vec3 pointing from the light-source toward the fragment

        //the shininess-value dictates how focused the spot of reflected light is
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
        vec3 specular = light.specular * spec * vec3(texture(material.specular, normTexCoords));

        //attenuation
        float distance    = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
        ambient  *= attenuation; 
        diffuse   *= attenuation;
        specular *= attenuation; 

        return (ambient + (diffuse * intensity) + (specular * intensity));
    }
