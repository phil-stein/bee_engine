#version 330 core

    struct Material 
    {
        sampler2D diffuse;
        sampler2D specular;
        float shininess;
        vec2 tile;
        vec3 tint;
    };

    struct DirectionalLight 
    {
        vec3 direction;
  
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };
    struct PointLight 
    {
        vec3 position;
  
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;

        float constant;
        float linear;
        float quadratic;
    };
    struct SpotLight 
    {
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

    struct LightLevel
    {
        float minVal;
        float maxVal;

        float lightness;

        vec3 tint;
    };

    
    //fragment/pixel color after calculations
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

    //uniforms
    uniform Material mat;
    uniform vec3 view_pos;

    uniform int num_dir_lights;
    uniform DirectionalLight dir_lights[6];
    
    uniform int num_point_lights;
    uniform PointLight point_lights[32];
        
    uniform int num_spot_lights;
    uniform SpotLight spot_lights[16];

    // could be uniforms
    int Num_LightLevels = 0;
    LightLevel lightLevels[8];

    //function prototypes (they need to be declared before being called like in c)
    float CalcDirectionalLight(DirectionalLight light, vec2 texCoords, vec3 normal, vec3 viewDir);
    float CalcPointLight(PointLight light, vec2 texCoords, vec3 normal, vec3 viewDir);
    float CalcSpotLight(SpotLight light, vec2 texCoords, vec3 normal, vec3 viewDir);

    void main() 
    {
        
    	lightLevels[0].minVal    = -1.0;
    	lightLevels[0].maxVal    = 0.1;
    	lightLevels[0].lightness = 0.6;
    	lightLevels[0].tint      = vec3(1, 1, 1);
    	
    	lightLevels[1].minVal    = 0.1;
    	lightLevels[1].maxVal    = 0.8;
    	lightLevels[1].lightness = 0.7;
    	lightLevels[1].tint      = vec3(1, 1, 1);
    	
    	lightLevels[2].minVal    = 0.8;
    	lightLevels[2].maxVal    = 2.0;
    	lightLevels[2].lightness = 0.8;
    	lightLevels[2].tint      = vec3(1, 1, 1);
    	Num_LightLevels = 3;
    	// 0.0f, 0.15f, 0.1f, new Vector3(1.2f, 1.0f, 3.0f)),                     
    	// 0.15f, 0.5f, 0.5f, new Vector3(1.2f, 1.0f, 1.5f)),                     
    	// 0.5f, 2.0f, 0.8f, new Vector3(1.2f, 1.0f, 1.2f)),i

        //scale the texcoords to fit the specified tiling
        vec2 normTexCoords = mat.tile * _in.tex_coords;

        //get surface normal and the dir the light is coming from
        vec3 normal = normalize(_in.normal);

        //get the angle between the reflected light-ray and the view-direction        
        vec3 viewDir = normalize(view_pos - _in.frag_pos);
     
        float result =0.0;
        result += CalcDirectionalLight(dir_lights[0], normTexCoords, normal, viewDir);
        result = clamp(result, 0, 1);
        result *= 2; //with only dir lights it's a bit weak

        vec4 color = texture(mat.diffuse, normTexCoords);

        for(int i = 0; i < Num_LightLevels; i++)
        {
            if(result > lightLevels[i].minVal && result < lightLevels[i].maxVal)
            {
                //float dif = result - lightLevels[i].lightness; //calc difference between cur lightlevel and the desired one
                result = lightLevels[i].lightness; //result -dif //subtract to get the cur lightness to the desired lightness
                color *= vec4(lightLevels[i].tint, 1.0);
            }

        }
        
        //color = vec4(color.rgb * vec3(0.55, 0.25, 0.2), color.a);
        //color = vec4(color.rgb * vec3(0.4, 0.2, 0.2), color.a);
        //color = vec4(color.rgb * vec3(1.0, 0.75, 0.75), color.a);
        FragColor = vec4(color.rgb * result * mat.tint, color.a); //vec4(color.rgb * result, color.a); //color.rgb * result, color.a
    }

    float CalcDirectionalLight(DirectionalLight light,vec2 texCoords, vec3 normal, vec3 viewDir)
    {
        //explanaition: https://learnopengl.com/Lighting/Light-casters, LearnOpenGL page 137

        //diffuse----------------------------------
        //vec3 lightDir = normalize(light.position - FragPos);
        vec3 lightDir = normalize(-light.direction);

        //dot product between surface-normal and light-dir, the clamped to get a value between 0-1, would otherwise be neg. if the angle was greater than 90° 
        float diff = max(dot(normal, lightDir), 0.0);
        float diffuse = (light.diffuse.x * diff) + (light.diffuse.y * diff) + (light.diffuse.z * diff);

        //ambient----------------------------------
        float ambient = (light.ambient.x * 0.5) + (light.ambient.y * 0.5) + (light.ambient.z * 0.5); //light.ambient * material.ambient; //* 0.5 because it's ambient

        //specular----------------------------------
        vec3 reflectDir = reflect(-lightDir, normal); //lightDir negated, because reflect() wants a Vec3 pointing from the light-source toward the fragment

        //the shininess-value dictates how focused the spot of reflected light is
        //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
        //float specular = (light.specular.x * spec) + (light.specular.y * spec) + (light.specular.z * spec);

        return (ambient + diffuse) * 0.33; //+ specular

    }

