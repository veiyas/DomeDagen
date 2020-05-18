#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <map>

#include "model.hpp"
#include "mesh.hpp"
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "utility.hpp"

//This class was written with help of tutorial
//https://learnopengl.com/Model-Loading/Assimp
//Skeletal animations are based on
//http://ogldev.atspace.co.uk/www/tutorial38/tutorial38.html
class Model
{
public:
	Model() = default;

	//Ctor with path to .fbx file (char* because library wanted it)
	Model(char* path);

	//Render model
	void render() const;

	const std::vector<glm::mat4>& getTransforms() const { return mTransforms; }

private:
	//Model data
	std::vector<Mesh> mMeshes;
	std::string mDirectory;

	struct BoneInfo
	{
		glm::mat4 mOffset{ };
		glm::mat4 mFinalTransformation{ };
	};

	//Mapping betweed bone name and id
	std::map<std::string, unsigned> mBoneMap;
	std::vector<BoneInfo> mBoneInfo;
	unsigned mNumBones = 0;
	std::vector<glm::mat4> mTransforms;

	const aiScene* mScene;
	Assimp::Importer mImporter;
	glm::mat4 mGlobalInverseTransform;

	//Load model and sets mDirectory
	void loadModel(const std::string& path);

	//Process all nodes from assimp recursively, property of online tutorial
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	//TODO Description
	void processBones(unsigned meshIndex, const aiMesh* mesh, Vertex& vertex);
	void boneTransform(float time, std::vector<glm::mat4>& transforms);
	void readNodeHeirarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform);

	void updateBonePose(float time);

	//These are pretty much copied
	void calcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);
	void calcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* nodeAnim);
	void calcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);
	unsigned findScaling(float animationTime, const aiNodeAnim* nodeAnim);
	unsigned findRotation(float animationTime, const aiNodeAnim* nodeAnim);
	unsigned findPosition(float animationTime, const aiNodeAnim* nodeAnim);
	const aiNodeAnim* findNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);


	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

};