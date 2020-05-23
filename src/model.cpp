#include "model.hpp"
#include "glm/ext/matrix_transform.hpp"

Model::Model(char* path) : mImporter{ }
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
	mImporter.SetPropertyBool(AI_CONFIG_PP_PTV_NORMALIZE, true);
	//mScene = mImporter.ReadFile(path, aiProcess_Triangulate| aiProcess_FlipUVs | aiProcess_PreTransformVertices);
	mScene = mImporter.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_LimitBoneWeights);
	//const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!mScene || mScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !mScene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << mImporter.GetErrorString() << "\n";
		return;
	}
	mDirectory = path.substr(0, path.find_last_of('/'));
	mGlobalInverseTransform = Utility::aiMatrixToGlm((mScene->mRootNode->mTransformation).Inverse());
	processNode(mScene->mRootNode, mScene);
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

	processBones(mesh, vertices);

	return Mesh(vertices, indices, textures);
}

void Model::processBones(const aiMesh* mesh, std::vector<Vertex>& vertices)
{
	//unsigned numInserted = 0;

	std::cout << "mNumBones: " << mesh->mNumBones << '\n';

	for (unsigned i = 0; i < mesh->mNumBones; ++i)
	{
		unsigned boneIndex = 0;
		std::string boneName(mesh->mBones[i]->mName.data);

		if (mBoneMap.find(boneName) == mBoneMap.end()) //bone not in mBoneMap
		{
			boneIndex = mNumBones++;
			BoneInfo bone;
			mBoneInfo.push_back(bone);
			mBoneInfo[boneIndex].mOffset = Utility::aiMatrixToGlm(mesh->mBones[i]->mOffsetMatrix);
			mBoneMap[boneName] = boneIndex;
		}
		else
		{
			boneIndex = mBoneMap[boneName];
		}

		for (unsigned j = 0; j < mesh->mBones[i]->mNumWeights; ++j)
		{
			// FIXME wat am i even doing here hhehe 

			//std::cout << "mesh->mBones[i]->mNumWeights: " << mesh->mBones[i]->mNumWeights << '\n';

			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			//vertex.addBoneData(boneIndex, weight);
			//vertex.mBoneIds[numInserted] = boneIndex;
			//vertex.mWeights[numInserted] = weight;
			//++numInserted;


			auto& vertex = vertices[mesh->mBones[i]->mWeights[j].mVertexId];

			for (unsigned k = 0; i < NUM_BONES_PER_VERTEX; ++k) {
				if (vertex.mWeights[k] == 0.0) {
					vertex.mBoneIds[k] = boneIndex;
					vertex.mWeights[k] = weight;
					break;
				}
			}
		}
	}
}

void Model::boneTransform(float time) // actually this should be in each mesh maybe 
{
	glm::mat4 identity{ 1.0f };

	float ticksPerSecond = mScene->mAnimations[0]->mTicksPerSecond;
	if (ticksPerSecond == 0)
		ticksPerSecond = 25.0f;

	float timeInTicks = time * ticksPerSecond;
	float animationTime = fmod(timeInTicks, mScene->mAnimations[0]->mDuration);

	readNodeHeirarchy(animationTime, mScene->mRootNode, identity);

	mTransforms.resize(mNumBones);

	for (unsigned i = 0; i < mNumBones; ++i)
	{
		mTransforms[i] = mBoneInfo[i].mFinalTransformation;
	}
}

void Model::readNodeHeirarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform)
{
	std::string nodeName(node->mName.data);
	const aiAnimation* animation = mScene->mAnimations[0];
	glm::mat4 nodeTransformation = Utility::aiMatrixToGlm(node->mTransformation);
	const aiNodeAnim* nodeAnim = findNodeAnim(animation, nodeName);

	if (nodeAnim)
	{
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D scaling;
		calcInterpolatedScaling(scaling, animationTime, nodeAnim);
		/*Matrix4f ScalingM;
		ScalingM.InitScaleTransform(scaling.x, scaling.y, scaling.z);*/
		glm::mat4 scalingM = glm::scale(glm::mat4(1.0f), glm::vec3(scaling.x, scaling.y, scaling.z));

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion rotationQ;
		calcInterpolatedRotation(rotationQ, animationTime, nodeAnim);
		//Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());
		glm::mat4 rotationM = Utility::aiMatrixToGlm(aiMatrix4x4(rotationQ.GetMatrix()));

		// Interpolate translation and generate translation transformation matrix
		aiVector3D translation;
		calcInterpolatedPosition(translation, animationTime, nodeAnim);
		/*Matrix4f TranslationM;
		TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);*/
		glm::mat4 translationM = glm::translate(glm::mat4(1.0f),
			glm::vec3(translation.x, translation.y, translation.z));

		// Combine the above transformations
		nodeTransformation = translationM * rotationM * scalingM;
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransformation;

	if (mBoneMap.find(nodeName) != mBoneMap.end())
	{
		unsigned boneIndex = mBoneMap[nodeName];
		mBoneInfo[boneIndex].mFinalTransformation = mGlobalInverseTransform * globalTransformation *
													mBoneInfo[boneIndex].mOffset;
	}

	for (unsigned i = 0; i < node->mNumChildren; ++i)
	{
		readNodeHeirarchy(animationTime, node->mChildren[i], globalTransformation);
	}
}

void Model::calcInterpolatedScaling(aiVector3D & out, float animationTime, const aiNodeAnim * nodeAnim)
{
	if (nodeAnim->mNumPositionKeys == 1) {
		out = nodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned PositionIndex = findPosition(animationTime, nodeAnim);
	unsigned NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < nodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(nodeAnim->mPositionKeys[NextPositionIndex].mTime - nodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (animationTime - (float)nodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = nodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = nodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	out = Start + Factor * Delta;
}

void Model::calcInterpolatedRotation(aiQuaternion & out, float animationTime, const aiNodeAnim * nodeAnim)
{
	// we need at least two values to interpolate...
	if (nodeAnim->mNumRotationKeys == 1) {
		out = nodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned RotationIndex = findRotation(animationTime, nodeAnim);
	unsigned NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < nodeAnim->mNumRotationKeys);
	float DeltaTime = nodeAnim->mRotationKeys[NextRotationIndex].mTime - nodeAnim->mRotationKeys[RotationIndex].mTime;
	float Factor = (animationTime - (float)nodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = nodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = nodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, Factor);
	out = out.Normalize();
}

void Model::calcInterpolatedPosition(aiVector3D & out, float animationTime, const aiNodeAnim * nodeAnim)
{
	if (nodeAnim->mNumScalingKeys == 1) {
		out = nodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned ScalingIndex = findScaling(animationTime, nodeAnim);
	unsigned NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < nodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(nodeAnim->mScalingKeys[NextScalingIndex].mTime - nodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (animationTime - (float)nodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = nodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = nodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	out = Start + Factor * Delta;
}

unsigned Model::findPosition(float animationTime, const aiNodeAnim* nodeAnim)
{
	for (unsigned i = 0; i < nodeAnim->mNumPositionKeys - 1; i++) {
		if (animationTime < (float)nodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


unsigned Model::findRotation(float animationTime, const aiNodeAnim* nodeAnim)
{
	assert(nodeAnim->mNumRotationKeys > 0);

	for (unsigned i = 0; i < nodeAnim->mNumRotationKeys - 1; i++) {
		if (animationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


unsigned Model::findScaling(float animationTime, const aiNodeAnim* nodeAnim)
{
	assert(nodeAnim->mNumScalingKeys > 0);

	for (unsigned i = 0; i < nodeAnim->mNumScalingKeys - 1; i++) {
		if (animationTime < (float)nodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

const aiNodeAnim* Model::findNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for (unsigned i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

void updateBonePose(float time)
{

}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		Texture texture;
		texture.mId = Utility::textureFromFile(str.C_Str(), mDirectory);
		texture.mType = typeName;
		texture.mPath = mDirectory + "/" + str.C_Str();
		textures.push_back(texture);
	}
	return textures;
}
