#include <stdio.h> 
#include <stdlib.h>
#include <direct.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION // only define in one ".c" file
#include "stb\stb_image.h"
#include "stb\stb_ds.h"         // already defined STB_DS_IMPLEMENTATION in renderer.c

// #define STB_DS_IMPLEMENTATION
// #include "stb\stb_ds.h"
#include "GLAD\glad.h"
#include"CGLM\cglm.h"

// #define FAST_OBJ_IMPLEMENTATION // only define in one ".c" file
// #include "fastobj\fast_obj.h"

#include "asset_manager.h"
#include "file_handler.h"
#include "str_util.h"
#include "global.h"


bee_bool file_exists_check(const char* file_path)
{
    FILE* f = fopen(file_path, "rb");
    if (f == NULL) {
        return BEE_FALSE;
    }
    return BEE_TRUE;
}

char* read_text_file(const char* file_path)
{
    FILE* f;
    char* text;
    long len;

    f = fopen(file_path, "rb");
    if (f == NULL) 
    {
        fprintf(stderr, "[ERROR] loading text-file at: %s\n", file_path);
        assert(BEE_FALSE);
    }

    // get len of file
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    assert(len > 0);
    fseek(f, 0, SEEK_SET);

    // alloc memory 
    text = calloc(1, len);
    assert(text != NULL);
    
    // fill text buffer
    fread(text, 1, len, f);
    assert(strlen(text) > 0);
    fclose(f);

    return text;
}
char* read_text_file_len(const char* file_path, int* length)
{
    FILE* f;
    char* text;
    long len;

    f = fopen(file_path, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "[ERROR] loading text-file at: %s\n", file_path);
        assert(BEE_FALSE);
    }

    // get len of file
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    assert(len > 0);
    fseek(f, 0, SEEK_SET);

    // alloc memory 
    text = calloc(1, len);
    assert(text != NULL);

    // fill text buffer
    fread(text, sizeof(char), len, f);
    assert(strlen(text) > 0);
    fclose(f);

    *length = len;
    return text;
}
/*
{
    
    // taken from: https://stackoverflow.com/questions/3463426/in-c-how-should-i-read-a-text-file-and-print-all-strings
    // @TODO: rewrite this completely

    char* buffer = NULL;
    int string_size, read_size;
    FILE* fp = fopen(file_path, "r");

    if (fp)
    {
        // Seek the last byte of the file
        fseek(fp, 0, SEEK_END);
        // Offset from the first to the last byte, or in other words, filesize
        string_size = ftell(fp);
        // go back to the start of the file
        //rewind(fp);
        fseek(fp, 0, SEEK_SET);

        // Allocate a string that can hold it all
        buffer = (char*)malloc(sizeof(char) * (string_size + 1));

        // check malloc worked
        if (buffer == NULL) { fprintf(stderr, "error allocating memory for text file reading"); exit(1); }

        // Read it all in one operation
        read_size = (int)fread(buffer, (size_t)sizeof(char), (size_t)string_size, fp);

        // fread doesn't set it so put a \0 in the last position
        // and buffer is now officially a string
        buffer[read_size] = '\0';

        // @TODO: @STABILITY: i should look for errors here i think 
        if (0 == 1 && string_size != read_size)
        {
            // Something went wrong, throw away the memory and set
            // the buffer to NULL
            free(buffer);
            buffer = NULL;
            fprintf(stderr, "error while loading text file in \"%s\"\n", file_path);
            fprintf(stderr, "string_size: %d, read_size: %d\n", string_size, read_size);
        }

        // Always remember to close the file.
        fclose(fp);
    }

    return buffer;
}
*/

void write_text_file(const char* file_path, const char* txt, int len)
{
    FILE* f;

    f = fopen(file_path, "wb");
    if (f == NULL) 
    {
        fprintf(stderr, "[ERROR] loading text-file at: %s\n", file_path);
        assert(BEE_FALSE);
    }

    int rtn = fwrite(txt, sizeof(char), len, f);
    assert(rtn != EOF);
    // fprintf(f, "%s", txt);

    fclose(f);
}

rtn_code copy_file(char* filepath_src, char* filepath_cpy)
{
    FILE* ptr_src, * ptr_cpy;
    errno_t err_src = 0, err_cpy = 0;
    int  a;

    // open files
    err_src  = fopen_s(&ptr_src, filepath_src, "rb");
    err_cpy  = fopen_s(&ptr_cpy, filepath_cpy, "wb"); // opens / creates file

    // check for errors opening the files
    if (err_src != 0)
    { return  BEE_ERROR; }
    if (err_cpy != 0)
    {
        fclose(ptr_src);
        return  BEE_ERROR;
    }

    // copy bytes from src to copy
    while (1)
    {
        a = fgetc(ptr_src);

        if (!feof(ptr_src))
            fputc(a, ptr_cpy);
        else
            break;
    }

    fclose(ptr_cpy);
    fclose(ptr_src);
    return  BEE_OK;
}



// textures -------------------------------------------------------------------------

void texture_load_pixels(char* path, u8** pixels_out, size_t* width_out, size_t* height_out, int* channel_num, bee_bool flip_vertical) 
{
    int width, height;

    // @TODO: flip horizontaly

    // OpenGL has texture coordinates with (0, 0) on bottom
    stbi_set_flip_vertically_on_load(flip_vertical);
    u8* image = stbi_load(path, &width, &height, channel_num, STBI_rgb_alpha);
    assert(image != NULL);

    *pixels_out = malloc((double)(width * height * 4));
    assert(*pixels_out != NULL);
    memcpy(*pixels_out, image, (double)(width * height * 4));
    assert(*pixels_out != NULL);
    *width_out = width;
    *height_out = height;

    stbi_image_free(image);
}

u32 texture_create_from_pixels(u8* pixels, size_t width, size_t height, int channel_num, bee_bool gamma_correct)
{
    u32 handle;

    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);

    // no interpolation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    assert(channel_num >= 1);
    assert(channel_num != 2);
    assert(channel_num <= 4);
    int gl_internal_format = 0;
    int gl_format          = 0;
    switch (channel_num)
    {
        case 1:
            gl_internal_format = GL_R8;
            gl_format = GL_RED;
        case 3:
            gl_internal_format = gamma_correct ? GL_SRGB : GL_RGB; // GL_SRGB
            gl_format = GL_RGB;
        case 4:
            gl_internal_format = gamma_correct ? GL_SRGB_ALPHA : GL_RGBA; // GL_SRGB_APLHA
            gl_format = GL_RGBA;
    }
    assert(gl_format != 0);

    glTexImage2D(GL_TEXTURE_2D, 0, gl_internal_format, width, height, 0, gl_format, GL_UNSIGNED_BYTE, pixels);
    return handle;
}

texture texture_create_from_path(const char* file_path, const char* name, bee_bool flip_vertical, bee_bool gamma_correct)
{
    u8* pixels;
    size_t width, height;
    int channel_num = 0;
    texture_load_pixels(file_path, &pixels, &width, &height, &channel_num, flip_vertical);
    u32 handle = texture_create_from_pixels(pixels, width, height, channel_num, gamma_correct);
    free(pixels);

    texture tex;
    tex.handle = handle;
    tex.size_x = width;
    tex.size_y = height;
    tex.name = name;
    // tex.name = malloc(strlen(name) * sizeof(char));
    // strcpy(tex.name, name);
    tex.path = file_path;
#ifdef EDITOR_ACT
    if (gamma_correct)
    {
        tex.icon_handle = texture_create_from_path(file_path, name, flip_vertical, BEE_FALSE).handle;
    }
    else { tex.icon_handle = handle; }
#endif

    // printf("loaded texture: '%s' from '%s', handle: '%d'\n", name, file_path, handle);

    return tex;
}

u32 load_cubemap(char* right, char* left, char* bottom, char* top, char* front, char* back)
{
    u32 cube_map;
    glGenTextures(1, &cube_map);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map);

    char* cube_map_paths[6];
    cube_map_paths[0] = get_texture_path(right);
    cube_map_paths[1] = get_texture_path(left);
    cube_map_paths[2] = get_texture_path(bottom);
    cube_map_paths[3] = get_texture_path(top);
    cube_map_paths[4] = get_texture_path(front);
    cube_map_paths[5] = get_texture_path(back);


    int width, height, nrChannels;
    for (u32 i = 0; i < 6; ++i)
    {
        // OpenGL has texture coordinates with (0, 0) on bottom
        stbi_set_flip_vertically_on_load(BEE_TRUE);
        u8* data = stbi_load(cube_map_paths[i], &width, &height, &nrChannels, 0);
        if (data)
        {
            // we can increment the first param like this because the enum linearly increments
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
            stbi_image_free(data);
        }
        else
        {
            printf("Cubemap tex failed to load at path: %s\n", cube_map_paths[i]);
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return cube_map;
}


// mesh -----------------------------------------------------------------------------

mesh load_mesh(char* file_path)
{
    //const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    const struct aiScene* scene = aiImportFile(file_path,
        aiProcess_CalcTangentSpace         |
        aiProcess_FlipUVs                  |
        aiProcess_Triangulate           // |
        // aiProcess_JoinIdenticalVertices |
        //aiProcess_SortByPType
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        fprintf(stderr, "-!!!-> ERROR_LOADING_MODEL: %s\n", aiGetErrorString());
        return;
    }
    // else if (!scene->mRootNode->mMeshes)
    // {
    //     fprintf(stderr, "-!!!-> ERROR_LOADING_MODEL: no mesh-data loaded\n");
    //     assert(scene->mRootNode->mMeshes != NULL); // didnt load any mesh data
    // }

    char* name = str_find_last_of(file_path, "\\");
    if (name == NULL)
    {
        name = str_find_last_of(file_path, "/");
    }
    assert(name != NULL);
    name = str_trunc(name, 1); // cut off the last "\"
    assert(name != NULL);
    // printf("mesh name: '%s'\n", name);

    // struct aiMesh* mesh = scene->mMeshes[scene->mRootNode->mChildren[0]->mMeshes[0]]; // hard-coded the first mesh
    // return process_mesh(mesh, scene); // meshes.push_back()

    mesh m;
    process_node(scene->mRootNode, scene, &m, name);
    aiReleaseImport(scene);
    //printf("meshes loaded: %d\n", (int)arrlen(m));
    //assert(arrlen(m) >= 1);
    //mesh _mesh = m[0];
    // arrfree(m);
    return m; //_mesh
}

void process_node(struct aiNode* node, const struct aiScene* scene, mesh* m, const char* name)
{
    // process all the node's meshes (if any)
    // assert(node->mNumMeshes <= 1);
    for (u32 i = 0; i < node->mNumMeshes; i++)
    {
        struct aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        *m = process_mesh(mesh, scene, name); // meshes.push_back()
    }
    // then do the same for each of its children
    // assert(node->mNumChildren <= 0);
    for (u32 i = 0; i < node->mNumChildren; i++)
    {
        process_node(node->mChildren[i], scene, m, name);
    }
}

mesh process_mesh(struct aiMesh* m_assimp, struct aiScene* scene, const char* name)
{
    static int calls = 0; calls++;
    // printf("process_mesh called: %d\n", calls);

    // data to fill
    int verts_len = 0;
    f32* verts = NULL;
    int indices_len = 0;
    u32* indices = NULL;

    // walk through each of the mesh's vertices
    for (u32 i = 0; i < m_assimp->mNumVertices; i++)
    {
        // positions
        arrput(verts, m_assimp->mVertices[i].x); verts_len++;
        arrput(verts, m_assimp->mVertices[i].y); verts_len++;
        arrput(verts, m_assimp->mVertices[i].z); verts_len++;

        // normals
        if (m_assimp->mNormals != NULL)
        {
            arrput(verts, m_assimp->mNormals[i].x); verts_len++;
            arrput(verts, m_assimp->mNormals[i].y); verts_len++;
            arrput(verts, m_assimp->mNormals[i].z); verts_len++;
        }
        else
        {
            // printf("-!!!-> loaded mesh did not contain normal information\n");
            arrput(verts, 0.0f); verts_len++;
            arrput(verts, 0.0f); verts_len++;
            arrput(verts, 0.0f); verts_len++;
        }
        // texture coordinates
        if (m_assimp->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            arrput(verts, m_assimp->mTextureCoords[0][i].x); verts_len++;
            arrput(verts, m_assimp->mTextureCoords[0][i].y); verts_len++;
        }
        else
        {
            // printf("-!!!-> loaded mesh did not contain uv-coordinates\n");
            arrput(verts, 0.0f); verts_len++;
            arrput(verts, 0.0f); verts_len++;
        }
        if (m_assimp->mTangents != NULL)
        {
            // tangent
            arrput(verts, m_assimp->mTangents[i].x); verts_len++;
            arrput(verts, m_assimp->mTangents[i].y); verts_len++;
            arrput(verts, m_assimp->mTangents[i].z); verts_len++;
        }
        else
        {
            arrput(verts, 0.0f); verts_len++;
            arrput(verts, 0.0f); verts_len++;
            arrput(verts, 0.0f); verts_len++;
        }
    }

    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (u32 i = 0; i < m_assimp->mNumFaces; i++)
    {
        struct aiFace face = m_assimp->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (u32 j = 0; j < face.mNumIndices; j++)
        {
            arrput(indices, face.mIndices[j]); indices_len++;
        }
    }

    mesh m = make_mesh(verts, verts_len, indices, indices_len, name);

    arrfree(verts);
    arrfree(indices);

    return m;



    // // process materials
    // aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // // Same applies to other texture as the following list summarizes:
    // // diffuse: texture_diffuseN
    // // specular: texture_specularN
    // // normal: texture_normalN
    // 
    // // 1. diffuse maps
    // vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    // textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // // 2. specular maps
    // vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    // textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // // 3. normal maps
    // std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    // textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // // 4. height maps
    // std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    // textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
}
