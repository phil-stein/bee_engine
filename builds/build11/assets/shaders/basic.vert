#version 460 core

    layout (location = 0) in vec3 aPosition;    //the position is in attrib-index '0'
    layout (location = 1) in vec3 aNormal;      //aColor  //the color is in attrib-index '1'
    layout (location = 2) in vec2 aTexCoord;    //the uv-coordinates in attrib-index '2'

    out vec3 Normal;
    out vec3 FragPos; 
    out vec2 TexCoord;
    
    // uniform vec3 color; //for the unlit-shader

    uniform mat4 proj;          // matrix for camera projection
    uniform mat4 view;          // matrix for view transformation
    uniform mat4 model;         // matrix for applying the objects transform
    
    void main() 
    {
        //transforms normal-vec to world-space
        Normal = mat3(transpose(inverse(model))) * aNormal;
        // Normal = vec3(0.0);
        FragPos = vec3(model * vec4(aPosition.xyz, 1.0));
        TexCoord = aTexCoord;


        // the MVP matrix model view projection
        // don't change the order in the multiplication!
        // matrices are multiplied in reverse
        gl_Position = proj * view * model * vec4(aPosition.xyz, 1);
        
    }