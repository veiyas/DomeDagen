#include "model.hpp"

Model::Model(char* path)
{
    loadModel(path);
}

void Model::render() const
{
    for (const Mesh& m : mMeshes)
    {
        m.render();
    }
}

void Model::loadModel(const std::string& path)
{
    Assimp::Importer import;
    import.SetPropertyBool(AI_CONFIG_PP_PTV_NORMALIZE, true);
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate| aiProcess_FlipUVs | aiProcess_PreTransformVertices);
    //const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << "\n";
        return;
    }
    mDirectory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    //Process all the node's meshes (if any)
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        mMeshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex tempVertex;

        //Process vertex positions
        glm::vec3 vertexVector;
        vertexVector.x = mesh->mVertices[i].x;
        vertexVector.y = mesh->mVertices[i].y;
        vertexVector.z = mesh->mVertices[i].z;
        tempVertex.mPosition = vertexVector;

        //Process normals
        glm::vec3 normalVector;
        normalVector.x = mesh->mNormals[i].x;
        normalVector.y = mesh->mNormals[i].y;
        normalVector.z = mesh->mNormals[i].z;
        tempVertex.mNormal = normalVector;

        //Process textures
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 texVector;
            texVector.x = mesh->mTextureCoords[0][i].x;
            texVector.y = mesh->mTextureCoords[0][i].y;
            tempVertex.mTexCoords = texVector;
        }
        else
            tempVertex.mTexCoords = glm::vec2(0.f, 0.f);

        vertices.push_back(tempVertex);
    }

    //Process indices
    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    //Process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        texture.mId = Utility::TextureFromFile(str.C_Str(), mDirectory);
        texture.mType = typeName;
        texture.mPath = mDirectory + "/" + str.C_Str();
        textures.push_back(texture);
    }
    return textures;
}
