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

#include "Common.h"

#include "../common/tiny_obj_loader.h"

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

/*static const float vertices[] = { -1.0, -1.0, -1.0,   1.0, -1.0, -1.0,   1.0, 1.0, -1.0,     // Face 1
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
-1.0, 1.0, 1.0,   -1.0, 1.0, -1.0,   1.0, 1.0, -1.0 };      // Face 6

static const float couleurs[] = { 1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 1
1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 1

0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 2
0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 2

0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 3
0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 3

1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 4
1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 4

0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 5
0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 5

0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 6
0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0 };          // Face 6

static const float UV[] = {

	0.0f, 0.0f,

	1.0f,0.0f,

	1.0f, 1.0f,

	0.0f,0.0f,
	0.0f,1.0f,
	1.0f,1.0f,

	0.0f, 0.0f,

	1.0f,0.0f,

	1.0f, 1.0f,

	0.0f,0.0f,
	0.0f,1.0f,
	1.0f,1.0f,

	0.0f, 0.0f,

	1.0f,0.0f,

	1.0f, 1.0f,

	0.0f,0.0f,
	0.0f,1.0f,
	1.0f,1.0f,

	0.0f, 0.0f,

	1.0f,0.0f,

	1.0f, 1.0f,

	0.0f,0.0f,
	0.0f,1.0f,
	1.0f,1.0f,

	0.0f, 0.0f,

	1.0f,0.0f,

	1.0f, 1.0f,

	0.0f,0.0f,
	0.0f,1.0f,
	1.0f,1.0f,

	0.0f, 0.0f,

	1.0f,0.0f,

	1.0f, 1.0f,

	0.0f,0.0f,
	0.0f,1.0f,
	1.0f,1.0f

};*/







MeshObject aCube;
MeshObject bCube;
MeshObject sphere;
MeshObject character;
MeshObject ground;


 void Initialize()
{
	printf("Version Pilote OpenGL : %s\n", glGetString(GL_VERSION));
	printf("Type de GPU : %s\n", glGetString(GL_RENDERER));
	printf("Fabricant : %s\n", glGetString(GL_VENDOR));
	int numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	
	//glGetStringi = (PFNGLGETSTRINGIPROC)glutGetProcAddress("glGetStringi");
	
	/*
	for (int index = 0; index < numExtensions; ++index)
	{
		printf("Extension[%d] : %s\n", index, glGetStringi(GL_EXTENSIONS, index));
	}*/

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	GLenum init = glewInit();
	if (init != GL_NO_ERROR)
	{

	}

	//creationVBO
	
	


	/*aCube.CreateVBO();
	aCube.InitText("assets/crate12.jpg");
	aCube.InitShader("basic");

	bCube.CreateVBO();
	bCube.InitText("assets/crate13.jpg");
	bCube.InitShader("basic");*/
	//bCube.Update();


	ground.CreateVBOGround();
	ground.InitText("assets/floor022.jpg");
	ground.InitShader("basic");
	//ground.Translate(glm::vec3(-2.0f, 0.0f, -2.0f));
	
}

void Render()
{
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glClearColor(0.f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	
	//GLuint program = sceneShader.GetProgram();
	//glUseProgram(program);

	projectionview = glm::perspective(70.0, (double)800 / 600, 1.0, 100.0);

	modelview = glm::mat4(1.0f);
	modelview = glm::lookAt(glm::vec3(9, 9, 9), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 saveModelView = modelview;

	//GLuint program = sceneShader.GetProgram();
	//glUseProgram(program);


	/*
	angle += 0.004f;
	if (angle >= 360.0f)
	{
	angle -= 360.0f;
	}
	
	modelview = glm::rotate(modelview, angle, glm::vec3(0, 1, 0));*/
	/*
	aCube.DisplayNoText(projectionview, modelview);

	modelview = saveModelView;


	modelview = glm::translate(modelview,glm::vec3(3.0f, 0.0f, 0.0f));


	bCube.DisplayNoText(projectionview, modelview);
	*/

	ground.DisplayNoText(projectionview, modelview);
	glutSwapBuffers();
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Basic");

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

