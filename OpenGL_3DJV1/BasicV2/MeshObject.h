#pragma once


// Specifique a Windows
//#if _WIN32
//#include <Windows.h>
//#define FREEGLUT_LIB_PRAGMAS 0
//#pragma comment(lib, "freeglut.lib")
//#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "glew32s.lib")
//#endif

#define GLEW_STATIC 1
#include "GL/glew.h"
// FreeGLUT
//#include "GL/freeglut.h"

#include <iostream>
#include <cstdio>
#include <cmath>
#include <cassert>

#include "../common/EsgiShader.h"

#include "../Libs/glm/glm/glm.hpp"
#include <../Libs/glm/glm/gtx/transform.hpp>

#include <../Libs/glm/glm/gtc/type_ptr.hpp>

#include "Common.h"

#include "../common/tiny_obj_loader.h"

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
	void Update();
	void Translate(glm::vec3);
	void InitShader(std::string);
	void Destroy();

	float nbVertices;



	GLuint VBO;
	GLuint VAO;

	GLuint IBO;
	GLuint UBO;

	GLuint FBO;


	GLuint textureID;

	glm::mat4 modelview;
	glm::mat4 projectionview;
	glm::mat4 camview;

	EsgiShader sceneShader;
	EsgiShader objShader;



};

