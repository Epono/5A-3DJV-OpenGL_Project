#include "MeshObject.h"



MeshObject::MeshObject()
{
	m_shader.LoadFragmentShader("basic.fs");
	m_shader.LoadVertexShader("basic.vs");
	m_shader.Create();

	float vertices[] = { -1.0, -1.0, -1.0,   1.0, -1.0, -1.0,   1.0, 1.0, -1.0,     // Face 1
		-1.0, -1.0, -1.0,   -1.0, 1.0, -1.0,   1.0, 1.0, -1.0,     // Face 1

		1.0, -1.0, 1.0,   1.0, -1.0, -1.0,   1.0, 1.0, -1.0,       // Face 2
		1.0, -1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, -1.0,         // Face 2

		-1.0, -1.0, 1.0,   1.0, -1.0, 1.0,   1.0, -1.0, -1.0,      // Face 3
		-1.0, -1.0, 1.0,   -1.0, -1.0, -1.0,   1.0, -1.0, -1.0,    // Face 3

		-1.0, -1.0, 1.0,   1.0, -1.0, 1.0,   1.0, 1.0, 1.0,        // Face 4
		-1.0, -1.0, 1.0,   -1.0, 1.0, 1.0,   1.0, 1.0, 1.0,        // Face 4

		-1.0, -1.0, -1.0,   -1.0, -1.0, 1.0,   -1.0, 1.0, 1.0,     // Face 5
		-1.0, -1.0, -1.0,   -1.0, 1.0, -1.0,   -1.0, 1.0, 1.0,     // Face 5

		-1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, -1.0,         // Face 6
		-1.0, 1.0, 1.0,   -1.0, 1.0, -1.0,   1.0, 1.0, -1.0 };

	memcpy(m_vertices,vertices,108*4);
}


MeshObject::~MeshObject()
{
	glDeleteBuffers(1, &m_VBO);
}

void MeshObject::Display()
{
	glUseProgram(m_shader.GetProgram());
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, m_couleurs);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshObject::CreateVBO()
{

	glGenBuffers(1, &m_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	//allocation memoire
	glBufferData(GL_ARRAY_BUFFER, m_tailleVerticesBytes + m_tailleCouleursBytes, 0, GL_STATIC_DRAW);

	//Transfert a la gc
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_tailleVerticesBytes, m_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, m_tailleVerticesBytes, m_tailleCouleursBytes, m_couleurs);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

}