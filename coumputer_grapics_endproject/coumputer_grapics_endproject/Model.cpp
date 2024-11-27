#include <iostream>
#include <vector>

#include "Model.h"

#include "Texture.h"
#include "Mesh.h"
#include "Material.h"

#include <GL/glew.h>

#include <gl/glm/glm.hpp>
#include <gl/glm\gtc\matrix_transform.hpp>
#include <gl/glm\gtc\type_ptr.hpp>

#include "AssimpGLMHelpers.h"


Model::Model()
{
	translate = glm::vec3(0.f, 0.f, 0.f);
	rotate = glm::vec3(-90.f, 0.f, 0.f);
	scale = glm::vec3(1.f, 1.f, 1.f);

	modelMat = GetModelMat();

	material = nullptr;
}

std::string Model::GetName()
{
	return "Model";
}

void Model::ShowProperties()
{
	std::cout << "Transform" << std::endl;

	std::cout << "Translate: (" << translate.x << ", " << translate.y << ", " << translate.z << ")" << std::endl;
	std::cout << "Rotate: (" << rotate.x << ", " << rotate.y << ", " << rotate.z << ")" << std::endl;
	std::cout << "Scale: (" << scale.x << ", " << scale.y << ", " << scale.z << ")" << std::endl;

	// User input simulation for changing values (For demonstration)
	std::cout << "Enter new Translate (x y z): ";
	std::cin >> translate.x >> translate.y >> translate.z;
	SetTranslate(translate);

	std::cout << "Enter new Rotate (x y z): ";
	std::cin >> rotate.x >> rotate.y >> rotate.z;
	SetRotate(rotate);

	std::cout << "Enter new Scale (x y z): ";
	std::cin >> scale.x >> scale.y >> scale.z;
	SetScale(scale);

	// Material
	Material* currMaterial = GetMaterial();
	std::cout << "Material" << std::endl;

	std::cout << "Specular: " << currMaterial->specular << std::endl;
	std::cout << "Shininess: " << currMaterial->shininess << std::endl;

	// User input simulation for changing material properties (For demonstration)
	std::cout << "Enter new Specular (0 to 5): ";
	std::cin >> currMaterial->specular;

	std::cout << "Enter new Shininess (0 to 512): ";
	std::cin >> currMaterial->shininess;
}


void Model::LoadModel(const std::string& fileName)
{
	// �� �̸� ����
	int firstSlashIdx = fileName.find('/', 0);
	modelName = fileName.substr(0, firstSlashIdx);

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile( /*"Models/"+*/ fileName,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices |
		aiProcess_CalcTangentSpace);

	if (!scene)
	{
		std::cout << fileName << " Failed to load model : " << importer.GetErrorString() << std::endl;
		return;
	}

	LoadNode(scene->mRootNode, scene);
	LoadMaterials(scene);
}

void Model::RenderModel()
{
	std::vector<std::pair<Mesh*, unsigned int>> solidMeshList;
	std::vector<std::pair<Mesh*, unsigned int>> transparentMeshList;

	// LoadMesh �Լ����� ä������ meshList�� ��ȸ�ϸ� �޽õ��� �з��Ѵ�.
	for (size_t i = 0; i < meshList.size(); i++)
	{
		unsigned int materialIndex = meshToTex[i];

		// �޽� �з�
		if (meshList[i]->GetName().find("Hair") != std::string::npos ||
			meshList[i]->GetName().find("facial_serious10") != std::string::npos ||
			meshList[i]->GetName().find("facial_normal9") != std::string::npos)
			transparentMeshList.push_back({ meshList[i], materialIndex });
		else
			solidMeshList.push_back({ meshList[i], materialIndex });
	}

	// ������ �޽� ������
	for (auto& item : solidMeshList)
	{
		int materialIndex = item.second;
		Mesh* mesh = item.first;

		if (materialIndex < diffuseMaps.size() && diffuseMaps[materialIndex])
			diffuseMaps[materialIndex]->UseTexture(GL_TEXTURE0);
		if (materialIndex < normalMaps.size() && normalMaps[materialIndex])
			normalMaps[materialIndex]->UseTexture(GL_TEXTURE1);

		mesh->RenderMesh();
	}

	// ���� �޽� ������
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (auto& item : transparentMeshList)
	{
		int materialIndex = item.second;
		Mesh* mesh = item.first;

		if (materialIndex < diffuseMaps.size() && diffuseMaps[materialIndex])
			diffuseMaps[materialIndex]->UseTexture(GL_TEXTURE0);
		if (materialIndex < normalMaps.size() && normalMaps[materialIndex])
			normalMaps[materialIndex]->UseTexture(GL_TEXTURE1);

		mesh->RenderMesh();
	}
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}


void Model::ClearModel()
{
	for (size_t i = 0; i < meshList.size(); i++)
	{
		if (meshList[i])
		{
			delete meshList[i];
			meshList[i] = nullptr;
		}
	}

	for (size_t i = 0; i < diffuseMaps.size(); i++)
	{
		if (diffuseMaps[i])
		{
			delete diffuseMaps[i];
			diffuseMaps[i] = nullptr;
		}
	}

	delete(material);
}

void Model::LoadNode(aiNode* node, const aiScene* scene)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		// node->mMeshes[i] : �޽� ��ü�� �ƴϰ�, �޽��� ID�� �ǹ��Ѵ�.
		// ���� �޽ô� scene�� ����Ǿ��ֱ� ������ �̷��� �����ϰ� �ȴ�.
		LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
	}

	// �ڽ� ������ ���ȣ���� ���� ��ȸ�ϸ� �޽ø� �� �ε��Ѵ�.
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		LoadNode(node->mChildren[i], scene);
	}
}

// VBO, IBO�� ���� �������� ������ �� ���ش�.
void Model::LoadMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// vertices ä���ֱ�
	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		InitVertexBoneData(vertex);

		// position
		vertex.Position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);

		// texture
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = (mesh->mTextureCoords[0][i].x);
			vec.y = (mesh->mTextureCoords[0][i].y);
			vertex.TexCoords = vec;
		}
		else // �������� ���� ��� �׳� 0�� �־��ֱ�
		{
			vertex.TexCoords = glm::vec2(0.f, 0.f);
		}

		// normal 
		vertex.Normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);

		// tangent, bitangent
		vertex.Tangent = AssimpGLMHelpers::GetGLMVec(mesh->mTangents[i]);
		vertex.Bitangent = AssimpGLMHelpers::GetGLMVec(mesh->mBitangents[i]);

		vertices.push_back(vertex);
	}

	// indices ä���ֱ�
	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	ExtractBoneWeightForVertices(vertices, mesh, scene);

	Mesh* newMesh = new Mesh();
	newMesh->CreateMesh(vertices, indices, mesh->mName.C_Str()); // GPU�� VBO, IBO�� ���ؽ� ������ ���ش�.
	meshList.push_back(newMesh);

	// meshList�� mesh�� ä���ܰ� ���ÿ�, meshToTex���� �� mesh�� materialIndex�� ä���ش�.
	// �̷��� meshList�� meshToTex�� ������ ä�������ν� mesh�� �´� material�� �ս��� ã�� �� �ִ�.
	meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::LoadMaterials(const aiScene* scene)
{
	diffuseMaps.resize(scene->mNumMaterials);
	normalMaps.resize(scene->mNumMaterials);
	for (size_t i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];

		diffuseMaps[i] = nullptr;
		normalMaps[i] = nullptr;

		
		LoadDiffuseMaps(material, i);
		LoadNormalMaps(material, i);
	}

	material = new Material(0.3f, 64.f);
}

void Model::LoadDiffuseMaps(aiMaterial* material, const size_t& i)
{
	// Diffuse �ؽ��İ� �����ϴ� �� ���� Ȯ��
	//if (material->GetTextureCount(aiTextureType_DIFFUSE))
	{
		printf("road");
		aiString texturePath;
		// �ؽ��� ��θ� �������� �� �����ߴٸ�
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS)
		{
			printf("road2");
			// Ȥ�ó� �ؽ��� ��ΰ� ���� ��η� �Ǿ��ִٸ� �׿� ���� ó��
			int idx = std::string(texturePath.data).rfind("/");
			std::string textureName = std::string(texturePath.data).substr(idx + 1);
			idx = std::string(textureName).rfind("\\");
			textureName = std::string(textureName).substr(idx + 1);

			std::string texPath = /*"Models/" + */modelName + "/textures/" + textureName;

			diffuseMaps[i] = new Texture(texPath.c_str());
			std::cout << "Loading Diffuse : " << texPath << std::endl;

			// �ؽ��ĸ� ��ũ���� �޸𸮷� �ε�, GPU�� ���ش�.
			if (!diffuseMaps[i]->LoadTexture(4))
			{ // ���� ��
				std::cout << "Failed to load texture : " << texPath << std::endl;
				delete diffuseMaps[i];
				diffuseMaps[i] = nullptr;
			}
		}
	}
}

void Model::LoadNormalMaps(aiMaterial* material, const size_t& i)
{
	// normal map��
	if (material->GetTextureCount(aiTextureType_NORMALS) || material->GetTextureCount(aiTextureType_HEIGHT))
	{
		aiString texturePath;
		// �ؽ��� ��θ� �������� �� �����ߴٸ�
		if (material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == aiReturn_SUCCESS ||
			material->GetTexture(aiTextureType_HEIGHT, 0, &texturePath) == aiReturn_SUCCESS)
		{
			// Ȥ�ó� �ؽ��� ��ΰ� ���� ��η� �Ǿ��ִٸ� �׿� ���� ó��
			int idx = std::string(texturePath.data).rfind("/");
			std::string textureName = std::string(texturePath.data).substr(idx + 1);
			idx = std::string(textureName).rfind("\\");
			textureName = std::string(textureName).substr(idx + 1);

			std::string texPath = "Models/" + modelName + "/textures/" + textureName;

			normalMaps[i] = new Texture(texPath.c_str());
			std::cout << "Loading Normal : " << texPath << std::endl;

			// �ؽ��ĸ� ��ũ���� �޸𸮷� �ε�, GPU�� ���ش�.
			if (!normalMaps[i]->LoadTexture(3))
			{ // ���� ��
				std::cout << "Failed to load texture : " << texPath << std::endl;
				delete normalMaps[i];
				normalMaps[i] = nullptr;
			}
		}
	}
}

void Model::InitVertexBoneData(Vertex& vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		vertex.m_BoneIDs[i] = -1;
		vertex.m_Weights[i] = 0.f;
	}
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		if (vertex.m_BoneIDs[i] < 0)
		{
			// �ϳ��� ä��� ��������
			vertex.m_Weights[i] = weight;
			vertex.m_BoneIDs[i] = boneID;
			break;
		}
	}
}

void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
	{
		int boneID = -1;

		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			BoneInfo boneInfo;
			boneInfo.id = boneCounter;
			auto offsetMat = mesh->mBones[boneIndex]->mOffsetMatrix;
			boneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
				mesh->mBones[boneIndex]->mOffsetMatrix
			);

			boneInfoMap[boneName] = boneInfo;
			boneID = boneCounter;
			boneCounter++;
		}
		else
		{
			boneID = boneInfoMap[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; weightIndex++)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}

glm::mat4 Model::GetModelMat()
{
	// model Matrix ����
	glm::mat4 T = glm::translate(glm::mat4(1.f), glm::vec3(translate[0], translate[1], translate[2]));
	glm::mat4 R = glm::mat4_cast(glm::quat(glm::vec3(glm::radians(rotate[0]), glm::radians(rotate[1]), glm::radians(rotate[2]))));
	glm::mat4 S = glm::scale(glm::mat4(1.f), glm::vec3(scale[0], scale[1], scale[2]));
	return modelMat = T * R * S;
}

void Model::UpdateTransform(glm::mat4 newModelMat)
{
	glm::vec3 translation, rotation, scale;
	//ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(newModelMat), &translation[0], &rotation[0], &scale[0]);
	glm::value_ptr(newModelMat), & translation[0], & rotation[0], & scale[0];
	SetTranslate(translation);
	SetRotate(rotation);
	SetScale(scale);
}

Model::~Model()
{
	ClearModel();
}