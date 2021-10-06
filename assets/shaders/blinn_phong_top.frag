#version 330 core

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
  
    out vec4 FragColor;

    in vec3 Normal;
    in vec3 FragPos;
    in vec2 TexCoord;

    uniform Material materialOne;
    uniform Material materialTwo;

    uniform int Num_DirLights;
    uniform DirectionalLight dirLights[4];
    
    uniform int Num_PointLights;
    uniform PointLight pointLights[32];    

    uniform int Num_SpotLights;
    uniform SpotLight spotLights[16];
    
    uniform vec3 viewPos;

    //functions declarations
    vec3 CalcDirectionalLight(Material mat, DirectionalLight light, vec3 normal, vec3 viewDir);
    vec3 CalcPointLight(Material mat, PointLight light, vec3 normal, vec3 viewDir);
    vec3 CalcSpotLight(Material mat, SpotLight light, vec3 normal, vec3 viewDir);

    void main() 
    {
        //get surface normal and the dir the light is coming from
        vec3 norm = normalize(Normal);

        //get the angle between the reflected light-ray and the view-direction        
        vec3 viewDir = normalize(viewPos - FragPos);


        //materialOne_one--------------------------------------------------------------------
        vec3 mat_one = vec3(0.0);
        
        for(int i = 0; i < Num_DirLights; i++)
        {
            mat_one += CalcDirectionalLight(materialOne, dirLights[i], norm, viewDir);
        }
        
        for(int i = 0; i < Num_PointLights; i++)
        {
            mat_one += CalcPointLight(materialOne, pointLights[i], norm, viewDir);
        }

        for(int i = 0; i < Num_SpotLights; i++)
        {
            mat_one += CalcSpotLight(materialOne, spotLights[i], norm, viewDir);
        }

        //materialTwo_two--------------------------------------------------------------------
        vec3 mat_two = vec3(0.0);
        
        for(int i = 0; i < Num_DirLights; i++)
        {
            mat_two += CalcDirectionalLight(materialTwo, dirLights[i], norm, viewDir);
        }
        
        for(int i = 0; i < Num_PointLights; i++)
        {
            mat_two += CalcPointLight(materialTwo, pointLights[i], norm, viewDir);
        }

        for(int i = 0; i < Num_SpotLights; i++)
        {
            mat_two += CalcSpotLight(materialTwo, spotLights[i], norm, viewDir);
        }

        //*1.5 remaps the nomal.y value to a range on 0.0 - 1.5, instead of 0.0 - 1.0
        //because of this we can take that value to the power of 2 which will return 
        //smaller values for all values < 1.0 and greater values for all values > 1.0
        //* 0.4 to get the values to a more reasonable range (0.0 - 0.99)       
        float materialOneStrength = pow((norm.y * 1.5), 2) * 0.44;
        float materialTwoStrength = 1 - materialOneStrength;

        //materialOneStrength = 1.0;
        //materialTwoStrength = 0.0;
        
        vec3 result = (mat_one * materialOneStrength) + (mat_two * materialTwoStrength);

        FragColor = vec4(result, 1.0);
    }

    vec3 CalcDirectionalLight(Material mat, DirectionalLight light, vec3 normal, vec3 viewDir)
    {
        //explanaition: https://learnopengl.com/Lighting/Light-casters, LearnOpenGL page 137
        
        vec2 normTexCoords = mat.tile * TexCoord;

        //diffuse----------------------------------
        //vec3 lightDir = normalize(light.position - FragPos);
        vec3 lightDir = normalize(-light.direction);

        //dot product between surface-normal and light-dir, the clamped to get a value between 0-1, would otherwise be neg. if the angle was greater than 90° 
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(mat.diffuse, normTexCoords));

        //ambient----------------------------------
        vec3 ambient = light.ambient * vec3(texture(mat.diffuse, normTexCoords)); //light.ambient * material.ambient;

        //specular----------------------------------
        vec3 reflectDir = reflect(-lightDir, normal); //lightDir negated, because reflect() wants a Vec3 pointing from the light-source toward the fragment

        //the shininess-value dictates how focused the spot of reflected light is
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess * 128);
        vec3 specular = light.specular * spec * vec3(texture(mat.specular, normTexCoords));

        return (ambient + diffuse + specular);

    }

    vec3 CalcPointLight(Material mat, PointLight light, vec3 normal, vec3 viewDir)
    {
        //explanaition: https://learnopengl.com/Lighting/Light-casters, LearnOpenGL page 141
        
        vec2 normTexCoords = mat.tile * TexCoord;

        float dist = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist)); 

        //diffuse----------------------------------
        //get surface normal and the dir the light is coming from
        vec3 lightDir = normalize(light.position - FragPos);

        //dot product between surface-normal and light-dir, the clamped to get a value between 0-1, would otherwise be neg. if the angle was greater than 90° 
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(mat.diffuse, normTexCoords));

        //ambient----------------------------------
        vec3 ambient = light.ambient * vec3(texture(mat.diffuse, normTexCoords)); //light.ambient * material.ambient;

        //specular----------------------------------
        vec3 reflectDir = reflect(-lightDir, normal); //lightDir negated, because reflect() wants a Vec3 pointing from the light-source toward the fragment

        //the shininess-value dictates how focused the spot of reflected light is
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess * 128);
        vec3 specular = light.specular * spec * vec3(texture(mat.specular, normTexCoords));

        return ((ambient * attenuation) + (diffuse * attenuation) + (specular * attenuation));
    }

    vec3 CalcSpotLight(Material mat, SpotLight light, vec3 normal, vec3 viewDir)
    {
        //explanaition: https://learnopengl.com/Lighting/Light-casters, LearnOpenGL page 143

        vec2 normTexCoords = mat.tile * TexCoord;

        //diffuse----------------------------------
        //get surface normal and the dir the light is coming from
        vec3 lightDir = normalize(light.position - FragPos);

        //angle between spotlight-direction and the vector from the fragment to the light
        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.00, 1.0);

        //dot product between surface-normal and light-dir, the clamped to get a value between 0-1, would otherwise be neg. if the angle was greater than 90° 
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(mat.diffuse, normTexCoords));

        //ambient----------------------------------
        vec3 ambient = light.ambient * vec3(texture(mat.diffuse, normTexCoords)); //light.ambient * material.ambient;

        //specular----------------------------------
        //get the angle betwee the reflected light-ray and the view-direction        
        vec3 reflectDir = reflect(-lightDir, normal); //lightDir negated, because reflect() wants a Vec3 pointing from the light-source toward the fragment

        //the shininess-value dictates how focused the spot of reflected light is
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess * 128);
        vec3 specular = light.specular * spec * vec3(texture(mat.specular, normTexCoords));

        //attenuation
        float distance    = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
        ambient  *= attenuation; 
        diffuse   *= attenuation;
        specular *= attenuation; 

        return (ambient + (diffuse * intensity) + (specular * intensity));
    }