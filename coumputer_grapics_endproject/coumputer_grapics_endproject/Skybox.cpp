#include "Skybox.h"

#include "Shader.h"
#include "Mesh.h"

#include <vector>

Skybox::Skybox(std::vector<std::string> faceLocations)
{
	// shader setup
	skyShader = new Shader();
	skyShader->CreateFromFiles("Skybox.vert", "Skybox.frag");

	loc_PVM = skyShader->GetPVMLoc();
	loc_sampler = skyShader->GetColorSamplerLoc();

	// texture setup
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, bitDepth;
	// skybox�� 6�鿡 ���� �̹����� ���ش�.
	for (size_t i = 0; i < 6; i++)
	{
		unsigned char* texData = stbi_load(faceLocations[i].c_str(), &width, &height, &bitDepth, 0);
		if (!texData)
		{
			printf("Failed to find: %s\n", faceLocations[i].c_str());
			return;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		stbi_image_free(texData);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// mesh setup
	std::vector<unsigned int> skyboxIndices = {
		// front
		0, 1, 2,
		2, 1, 3,
		// right
		2, 3, 5,
		5, 3, 7,
		// back
		5, 7, 4,
		4, 7, 6,
		// left
		4, 6, 0,
		0, 6, 1,
		// top
		4, 0, 5,
		5, 0, 2,
		// bottom
		1, 6, 3,
		3, 6, 7
	};

	std::vector<Vertex> skyboxVertices = {
		Vertex(-1.0f, 1.0f, -1.0f) ,
		Vertex(-1.0f, -1.0f, -1.0f),
		Vertex(1.0f, 1.0f, -1.0f),
		Vertex(1.0f, -1.0f, -1.0f),

		Vertex(-1.0f, 1.0f, 1.0f),
		Vertex(1.0f, 1.0f, 1.0f),
		Vertex(-1.0f, -1.0f, 1.0f),
		Vertex(1.0f, -1.0f, 1.0f),
	};

	skyMesh = new Mesh();
	skyMesh->CreateMesh(skyboxVertices, skyboxIndices);
}

void Skybox::DrawSkybox(glm::mat4 viewMat, glm::mat4 projMat)
{
	// viewMat���� translate �κ��� ��������.
	viewMat = glm::mat4(glm::mat3(viewMat));

	// skybox�� depth mask ��Ȱ��ȭ
	glDepthMask(GL_FALSE);

	skyShader->UseShader();

	glm::mat4 PVM = projMat * viewMat; // modelMat�� ������� ����
	glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(PVM));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glUniform1i(loc_sampler, 0);

	skyMesh->RenderMesh();

	glDepthMask(GL_TRUE);
}
