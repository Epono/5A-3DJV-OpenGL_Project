#pragma once

#include "Common.h"

#include "MeshesInfo.h"

class MeshObject
{
public:
	MeshObject();
	~MeshObject();
	void CreateVBO();
	void CreateVBOGround();
	void Display();
	void DisplayNoText(glm::mat4 &,glm::mat4 &);
	void InitText(std::string);
	GLuint GetShader();
	void Update();
	void Translate(glm::vec3);
	void InitShader(std::string);
	void Destroy();

	void BindingForInit();
	void CreateFBO(int width, int height);
	void DestroyFBO();

	
	//pour la création du mesh
	void InitMesh(std::string, std::string);
	void DisplayObj(glm::vec3 &);
	//bool LoadAndCreateTextureRGBA(const char *, GLuint &); // Méthode transferé dans Common


	float nbVertices;
	bool Norm;
	bool TextureCoord;


	GLuint VBO;
	GLuint VAO;

	GLuint IBO;
	GLuint UBO;

	GLuint FBO;
	GLuint sceneFBO; // utilité ?

	GLuint blockBind;

	GLuint textureID;
	GLuint textureCoord;

	GLuint ElementCount;
	glm::vec3 position;

	glm::mat4 modelview;
	glm::mat4 projectionview;
	glm::mat4 camview;

	glm::mat4 worldMatrix;

	EsgiShader sceneShader;
	EsgiShader objShader;



};

