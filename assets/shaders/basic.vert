#version 460 core

    layout (location = 0) in vec3 aPosition;    // the position is in attrib-index '0'
    layout (location = 1) in vec3 aNormal;      // the normal is in attrib-index '1'
    layout (location = 2) in vec2 aTexCoords;    // the uv-coordinates in attrib-index '2'
    layout (location = 3) in vec3 aTangent;

    out vec3 Normal;
    out vec3 FragPos; 
    out vec2 TexCoord;
    
    uniform mat4 proj;          // matrix for camera projection
    uniform mat4 view;          // matrix for view transformation
    uniform mat4 model;         // matrix for applying the objects transform
    
    void main() 
    {
        //transforms normal-vec to world-space
        Normal = mat3(transpose(inverse(model))) * aNormal;

        FragPos = vec3(model * vec4(aPosition.xyz, 1.0));
        TexCoord = aTexCoords;

        // the MVP matrix model view projection
        // don't change the order in the multiplication!
        // matrices are multiplied in reverse
        gl_Position = proj * view * model * vec4(aPosition.xyz, 1);
        
    }