#define STB_IMAGE_IMPLEMENTATION
#include "MeshObject.h"

#ifndef BUFFER_OFFSET

#define BUFFER_OFFSET(offset) ((char*)NULL + (offset))

#endif


MeshObject::MeshObject()
{
	projectionview = glm::perspective(70.0, (double)800 / 600, 1.0, 100.0);

	modelview = glm::mat4(1.0f);
	modelview = glm::lookAt(glm::vec3(3, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	//camview = glm::lookAt(glm::vec3(3, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));*/
}


MeshObject::~MeshObject()
{

}

void MeshObject::Destroy()
{
	glDeleteVertexArrays(1,&VAO);
	glDeleteBuffers(1, &VBO);
	sceneShader.Destroy();
}
/*
template<size_t N>
void LoadVBO(const float(&vertices) [N])
{
	;
}*/

void MeshObject::CreateVBO()
{
		
	
		



		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(UV), 0, GL_STATIC_DRAW);
			
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(UV), UV);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//VAO THING
		
		glGenBuffers(1, &VAO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(UV)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);	


		glBindVertexArray(0);


	
}

void MeshObject::CreateVBOGround()
{


	/*
	projectionview = glm::perspective(70.0, (double)800 / 600, 1.0, 100.0);

	modelview = glm::mat4(1.0f);

	camview = glm::lookAt(glm::vec3(3, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	*/



	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices) + sizeof(groundUV), 0, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(groundVertices), groundVertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(groundVertices), sizeof(groundUV), groundUV);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//VAO THING

	glGenBuffers(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(groundUV)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);
}


void MeshObject::Display()
{
	GLuint program = sceneShader.GetProgram();
		glUseProgram(program);


	glBindVertexArray(VAO);


	


	
	
	/*
	float angle = 0.0f;

	angle += 0.004f;
	if (angle >= 360.0f)
	{
		angle -= 360.0f;
	}

	modelview = glm::rotate(modelview, angle, glm::vec3(0, 1, 0));
	*/
	GLuint model = glGetUniformLocation(sceneShader.GetProgram(), "u_modelviewMatrix");
	glUniformMatrix4fv(model, 1, GL_FALSE, value_ptr(modelview));

	GLuint projection = glGetUniformLocation(sceneShader.GetProgram(), "u_projectionMatrix");
	glUniformMatrix4fv(projection, 1, GL_FALSE, value_ptr(projectionview));

	//GLuint view = glGetUniformLocation(sceneShader.GetProgram(), "u_viewMatrix");
	//glUniformMatrix4fv(view, 1, GL_FALSE, value_ptr(camview));

	glBindTexture(GL_TEXTURE_2D, textureID);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void MeshObject::DisplayNoText(glm::mat4 &projectionView, glm::mat4 &modelView)
{
	GLuint program = sceneShader.GetProgram();
	glUseProgram(program);


	glBindVertexArray(VAO);







	/*
	float angle = 0.0f;

	angle += 0.004f;
	if (angle >= 360.0f)
	{
		angle -= 360.0f;
	}

	modelview = glm::rotate(modelview, angle, glm::vec3(0, 1, 0));
	*/
	GLuint model = glGetUniformLocation(sceneShader.GetProgram(), "u_modelviewMatrix");
	glUniformMatrix4fv(model, 1, GL_FALSE, value_ptr(modelView));

	GLuint projection = glGetUniformLocation(sceneShader.GetProgram(), "u_projectionMatrix");
	glUniformMatrix4fv(projection, 1, GL_FALSE, value_ptr(projectionView));

	/*GLuint view = glGetUniformLocation(sceneShader.GetProgram(), "u_viewMatrix");
	glUniformMatrix4fv(view, 1, GL_FALSE, value_ptr(camview));*/

	glBindTexture(GL_TEXTURE_2D, textureID);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void MeshObject::InitText(std::string textureName)
{
	int x, y;
	uint8_t *img;

	img = stbi_load(textureName.c_str(), &x, &y, nullptr, STBI_rgb_alpha);


	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (img)
	{

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	}
	else
	{
		std::cout << "Error texture" << std::endl;
	}


	glBindTexture(GL_TEXTURE_2D, 0);
}

void MeshObject::Update()
{
	modelview = glm::translate(glm::vec3(2.0f, 0.0f, 0.0f));
}

void MeshObject::Translate(glm::vec3 direction)
{
	modelview = glm::translate(direction);
}

void MeshObject::InitShader(std::string shaderName)
{
	std::string vsShader = shaderName + ".vs";
	std::string fsShader = shaderName + ".fs";
	sceneShader.LoadVertexShader(vsShader.c_str());
	sceneShader.LoadFragmentShader(fsShader.c_str());
	sceneShader.Create();

}