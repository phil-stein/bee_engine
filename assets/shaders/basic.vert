#version 460 core

    layout (location = 0) in vec3 aPosition;    // the position is in attrib-index '0'
    layout (location = 1) in vec3 aNormal;      // the normal is in attrib-index '1'
    layout (location = 2) in vec2 aTexCoords;    // the uv-coordinates in attrib-index '2'
    layout (location = 3) in vec3 aTangent;


    out VS_OUT
    {
        vec2 tex_coords;
        vec3 frag_pos;
        vec3 normal;
        mat3 TBN;
        // vec4 frag_pos_light_space;
    } _out;
    // out vec3 Normal;
    // out vec3 FragPos; 
    // out vec2 TexCoord;
    // out vec4 frag_pos_light_space;
    
    uniform mat4 proj;          // matrix for camera projection
    uniform mat4 view;          // matrix for view transformation
    uniform mat4 model;         // matrix for applying the objects transform
    // uniform mat4 light_space;
    
    void main() 
    {
        //transforms normal-vec to world-space
        _out.normal = mat3(transpose(inverse(model))) * aNormal;

        _out.frag_pos = vec3(model * vec4(aPosition.xyz, 1.0));
        _out.tex_coords = aTexCoords;

        // TBN matrix for normal mapping
        vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
        vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
        T = normalize(T - dot(T, N) * N);
        vec3 B = cross(N, T);
        _out.TBN = mat3(T, B, N);

        // frag_pos transformed to light_space for shadow mapping
        // _out.frag_pos_light_space = light_space * vec4(_out.frag_pos, 1.0);

        // the MVP matrix model view projection
        // don't change the order in the multiplication!
        // matrices are multiplied in reverse
        gl_Position = proj * view * model * vec4(aPosition.xyz, 1);
        
    }