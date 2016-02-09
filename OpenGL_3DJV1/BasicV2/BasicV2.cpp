// Basic.cpp : Defines the entry point for the console application.
//

// Specifique a Windows
#if _WIN32
#include <Windows.h>
#define FREEGLUT_LIB_PRAGMAS 0
#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32s.lib")
#endif

// Entete OpenGL 
//#include <gl/GL.h>
//#include "GL/glext.h"
#define GLEW_STATIC 1
#include "GL/glew.h"
// FreeGLUT
#include "GL/freeglut.h"

#include <iostream>
#include <cstdio>
#include <cmath>
#include <cassert>

#include "../common/EsgiShader.h"

#include "glm/glm/glm.hpp"
#include <glm/glm/gtx/transform.hpp>

#include <glm/glm/gtc/type_ptr.hpp>
#include "glm/gtx/euler_angles.hpp"


#include "Common.h"

#include "tinyobjloader/tiny_obj_loader.h"

#include "MeshObject.h"

// Macro utile au VBO

#ifndef BUFFER_OFFSET

#define BUFFER_OFFSET(offset) ((char*)NULL + (offset))

#endif

EsgiShader sceneShader;
//PFNGLGETSTRINGIPROC glGetStringi = nullptr;

glm::mat4 modelview;
glm::mat4 projectionview;
glm::mat4 camview;
GLuint textureID;


MeshObject aCube;
MeshObject bCube;
MeshObject sphere;
MeshObject character;
MeshObject ground;

int previousTime = 0;

void KeyboardInput(unsigned char key, int x , int y);

struct ViewProj
{
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::vec3 rotation;
	glm::vec4 position;
	glm::vec3 translation;
	GLuint UBO;
} g_Camera;

 void Initialize()
{
	printf("Version Pilote OpenGL : %s\n", glGetString(GL_VERSION));
	printf("Type de GPU : %s\n", glGetString(GL_RENDERER));
	printf("Fabricant : %s\n", glGetString(GL_VENDOR));
	int numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	
	
	
	/*
	for (int index = 0; index < numExtensions; ++index)
	{
		printf("Extension[%d] : %s\n", index, glGetStringi(GL_EXTENSIONS, index));
	}*/

	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	
	GLenum init = glewInit();
	if (init != GL_NO_ERROR)
	{

	}

	
	//test Obj load
	
	character.InitShader("basico");
	auto basicProgram = character.GetShader();


	

	auto blockIndex = glGetUniformBlockIndex(basicProgram, "ViewProj");
	GLuint id_blockBind = 1;

	glGenBuffers(1, &g_Camera.UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, g_Camera.UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, id_blockBind, g_Camera.UBO);

	glUniformBlockBinding(basicProgram, blockIndex, id_blockBind);

	previousTime = glutGet(GLUT_ELAPSED_TIME);

	character.InitMesh("bear-obj.obj", "bear.tga");
	//sphere.InitMesh("bear-obj.obj", "bear.tga");
	glBindVertexArray(character.VAO);
	glBindVertexArray(character.IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, character.IBO);
	
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	
	//creationVBO
		
	g_Camera.translation = glm::vec3(0, 0, 0);

	/*aCube.CreateVBO();
	aCube.InitText("assets/crate12.jpg");
	aCube.InitShader("basic");

	bCube.CreateVBO();
	bCube.InitText("assets/crate13.jpg");
	bCube.InitShader("basic");*/
	//bCube.Update();


	//ground.CreateVBOGround();
	//ground.InitText("assets/floor022.jpg");
	//ground.InitShader("basic");
	//ground.Translate(glm::vec3(-2.0f, 0.0f, -2.0f));
	
}

void Render()
{
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glClearColor(1.f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto width = glutGet(GLUT_WINDOW_WIDTH);
	auto height = glutGet(GLUT_WINDOW_HEIGHT);

	
	
	/*
	projectionview = glm::perspective(70.0, (double)800 / 600, 1.0, 100.0);

	modelview = glm::mat4(1.0f);
	modelview = glm::lookAt(glm::vec3(9, 9, 9), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 saveModelView = modelview;

	
	
	ground.DisplayNoText(projectionview, modelview);

	*/

	auto program = character.GetShader();
	glUseProgram(program);

	// variables uniformes (constantes) 

	g_Camera.projectionMatrix = glm::perspectiveFov(45.f, (float)width, (float)height, 0.1f, 1000.f);
	// rotation orbitale de la camera
	float rotY = glm::radians(g_Camera.rotation.y);
	g_Camera.position = glm::eulerAngleY(rotY) * glm::vec4(0.0f, 20.0f, 50.0f, 1.0f);
	g_Camera.viewMatrix = glm::lookAt(glm::vec3(g_Camera.position), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
	g_Camera.viewMatrix = glm::translate(g_Camera.viewMatrix, g_Camera.translation);
	// IL FAUT TRANSFERER LES MATRICES VIEW ET PROJ AU SHADER
	glBindBuffer(GL_UNIFORM_BUFFER, g_Camera.UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, glm::value_ptr(g_Camera.viewMatrix), GL_STREAM_DRAW);

	auto worldLocation = glGetUniformLocation(program, "u_worldMatrix");

	glm::mat4& transform = character.worldMatrix;
	transform[3] = glm::vec4(0.f, 0.f, 0.f, 1.0f);
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, glm::value_ptr(transform));


	glBindVertexArray(character.VAO);
	glBindVertexArray(character.IBO);

	auto textureLocation = glGetUniformLocation(program, "u_texture");
	glUniform1i(textureLocation, 0);

	glDrawElements(GL_TRIANGLES, character.ElementCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);




	glutSwapBuffers();
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Basic");
	glutKeyboardFunc(KeyboardInput);

#ifdef FREEGLUT
	// Note: glutSetOption n'est disponible qu'avec freeGLUT
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
				  GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif

	Initialize();

	glutDisplayFunc(Render);

	glutMainLoop();

	return 0;
}

void KeyboardInput(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'z':
		{
			g_Camera.translation += glm::vec3(0, 0, 1);
			break;
		}
		case 's':
		{
			g_Camera.translation += glm::vec3(0, 0, -1);
			break;
		}
		case 'q':
		{
			g_Camera.translation += glm::vec3(1, 0, 0);
			break;
		}
		case 'd':
		{
			g_Camera.translation += glm::vec3(-1, 0, 0);
			break;
		}
		case 'e':
		{
			g_Camera.rotation.y += -1;
			break;
		}
		case 'a':
		{
			g_Camera.rotation.y += 1;
			break;
		}
	}
}