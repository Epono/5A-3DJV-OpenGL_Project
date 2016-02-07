#pragma once

#include "../common/EsgiShader.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class MeshObject
{
public:
	MeshObject();
	~MeshObject();

	void Display();
	void CreateVBO();

	GLuint m_VBO;
	GLuint m_VAO;
	EsgiShader m_shader;
	float m_vertices[108];
	float m_couleurs[108];
	GLuint TextureID;


	int m_tailleVerticesBytes;
	int m_tailleCouleursBytes;

	protected:
		

		
	
};

