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
//Dans ce cas il faudra binder toutes les fonctions
//cela étant compliquer et fastidieux on va utiliser GLEW pour le faire
/*
#include <gl/GL.h>
#include "GL/glext.h"
*/
#define GLEW_STATIC 1
#include <GL/glew.h>

//Include GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"

#include "../common/tiny_obj_loader.h"

// FreeGLUT
#include "GL/freeglut.h"

#include <cstdio>
#include <iostream>
#include "../common/EsgiShader.h"
#include "MeshObject.h";
EsgiShader Shader;
MeshObject * firstVBO;

glm::mat4 projection;
glm::mat4 modelview;
glm::mat4 view;
GLuint textureID;


struct Object
{
	GLuint VBO;
};

//Object g_TriangleObj;

//Binding des fonctions si GLEW non utilisé
/*
PFNGLGETSTRINGIPROC glGetStringi = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
*/

static const float g_cubeVertices[] = {
	-1.f, -1.f, 1.0f,		// 0
	1.f, -1.f, 1.0f,  		// 1	
	1.f, 1.f, 1.0f,			// 2
	-1.f, 1.f, 1.0f,		// 3	
	-1.f, -1.f, -1.0f,		// 4
	1.f, -1.f, -1.0f,		// 5
	1.f, 1.f, -1.0f,		// 6
	-1.f, 1.f, -1.0f		// 7
};







void Initialize()
{
	//Quand on utilise Glew c'est la seule ligne nécessaire au setup
	GLenum error = glewInit();
	if (error != GL_NO_ERROR) {
		printf("ERROR");
	}

	Shader.LoadFragmentShader("basic.fs");
	Shader.LoadVertexShader("basic.vs");
	Shader.Create();

	firstVBO = new MeshObject();
	firstVBO->CreateVBO();

	glEnable(GL_DEPTH_TEST);

	int x, y;
	uint8_t *img;

	img = stbi_load("crate13.jpg", &x, &y, nullptr, STBI_rgb_alpha);


	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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

	//glEnable(GL_CULL_FACE);
	//printf("Version Pilote OpenGL : %s\n", glGetString(GL_VERSION));
	//printf("Type de GPU : %s\n", glGetString(GL_RENDERER));
	//printf("Fabricant : %s\n", glGetString(GL_VENDOR));
	//int numExtensions;
	//glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	
	//glGetStringi = (PFNGLGETSTRINGIPROC)glutGetProcAddress("glGetStringi");
	//glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glutGetProcAddress("glVertexAttribPointer");
	//glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glutGetProcAddress("glEnableVertexAttribArray");
		
	/*for (int index = 0; index < numExtensions; ++index)
	{
		printf("Extension[%d] : %s\n", index, glGetStringi(GL_EXTENSIONS, index));
	}*/
	


	//auto width = glutGet(GLUT_WINDOW_WIDTH);
	//auto height = glutGet(GLUT_WINDOW_HEIGHT);

	/*
	glGenBuffers(1, &g_TriangleObj.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, g_TriangleObj.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3, g_cubeVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	*/
	//modelview = glm::mat4(1.0); //matrice d'identité
	//modelview = glm::rotate(modelview, 60.0f, glm::vec3(0, 0, 1));
	
}

void Render()
{
	//glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	
	auto width = glutGet(GLUT_WINDOW_WIDTH);
	auto height = glutGet(GLUT_WINDOW_HEIGHT);

	//projection = glm::perspectiveFov(15.0f, (float)width, (float)height, 0.1f, 100.f);
	projection = glm::perspective(45.0, (double)width / height, 0.1, 100.0);
	
	modelview = glm::mat4(1.0);
	//1.6
	view = glm::lookAt(glm::vec3(3,3,3), glm::vec3(0,0,0), glm::vec3(0, 1, 0));

	//glClearColor(0.f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static const float vertices[] = { -1.0, -1.0, -1.0,   1.0, -1.0, -1.0,   1.0, 1.0, -1.0,     // Face 1
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

	};

	

	//On doit appeler l'utilisation du shader dans le Render
	GLuint program = Shader.GetProgram();
	glUseProgram(program);

	GLuint modelviewLocation = glGetUniformLocation(program, "u_modelviewMatrix");

	GLuint projLocation = glGetUniformLocation(program, "u_projectionMatrix");

	GLuint viewLocation = glGetUniformLocation(program, "u_viewMatrix");
	

	//glBindBuffer(GL_ARRAY_BUFFER, firstVBO->m_VBO);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	firstVBO->Display();

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	//glEnableVertexAttribArray(0);

	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, UV);
	//glEnableVertexAttribArray(1);

	//glBindTexture(GL_TEXTURE_2D, textureID);


	float angle = 0.0f;

	angle += 0.001f;
	if (angle >= 360.0f)
	{
		angle -= 360.0f;
	}

	modelview = glm::rotate(modelview, angle, glm::vec3(0, 1, 0));



	glUniformMatrix4fv(modelviewLocation, 1, GL_FALSE, glm::value_ptr(modelview));

	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projection));
	
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

	/*
	glBindBuffer(GL_ARRAY_BUFFER, g_TriangleObj.VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	*/




	

	
	
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindBuffer(GL_TEXTURE_2D, 0);
	//modelview = glm::translate(modelview, glm::vec3(0.4, 0, 0));

	//glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(modelview));

	//glUniformMatrix4fv(projLocation, 1, GL_FALSE, value_ptr(projection));

	//glDrawArrays(GL_TRIANGLES, 0, 3);
	
	//glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

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

