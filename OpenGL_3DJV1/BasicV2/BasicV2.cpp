// Basic.cpp : Defines the entry point for the console application.
//

#include "Common.h"

#include "MeshObject.h"
#include "Scene.h"

// Macro utile au VBO

#ifndef BUFFER_OFFSET

#define BUFFER_OFFSET(offset) ((char*)NULL + (offset))

#endif

EsgiShader sceneShader;

glm::mat4 modelview;
glm::mat4 projectionview;
glm::mat4 camview;
GLuint textureID;

MeshObject aCube;
MeshObject bCube;
MeshObject sphere;
MeshObject character;
MeshObject ground;
MeshObject Scene;

int previousTime = 0;

void KeyboardInput(unsigned char key, int x, int y);

/////////////////SCENE TEST///////////////////
extern Vertex g_Room[36];

Walls g_Walls;

Mesh g_WallMesh;

GLuint Material::UBO;

Material g_ShinyMaterial = {
	glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
	glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 32.0f)
};


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

	//Init GLEW		
	GLenum init = glewInit();
	if (init != GL_NO_ERROR)
	{

	}

	
	/////////////////////SCENE INIT////////////////////
	glGenBuffers(1, &Material::UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, Material::UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), &g_ShinyMaterial, GL_STATIC_DRAW);

	LoadMesh(g_WallMesh, g_Room);


	LoadAndCreateTextureRGBA("wall_color_map.jpg", g_Walls.textures[Walls::gWallTexture]);
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gWallTexture]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	LoadAndCreateTextureRGBA("floor_color_map.jpg", g_Walls.textures[Walls::gFloorTexture]);
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gFloorTexture]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	LoadAndCreateTextureRGBA("ceiling_color_map.jpg", g_Walls.textures[Walls::gCeilingTexture]);
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gCeilingTexture]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	Scene.InitShader("yolo");
	//////////////////////////////////////////////////////////////////////////////////////


	//Initialisation caméra 
	glGenBuffers(1, &g_Camera.UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, g_Camera.UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_STREAM_DRAW);

	GLuint id_blockBind = 1;

	glBindBufferBase(GL_UNIFORM_BUFFER, id_blockBind, g_Camera.UBO);

	//Initialisation des meshes //TODO : créer une fonction dans MeshObject
	character.InitShader("basico");
	sphere.InitShader("basico");
	auto basicProgram = character.GetShader();
	auto sphereProgram = sphere.GetShader(); 
	auto sceneProgram = Scene.GetShader();


	auto blockIndex = glGetUniformBlockIndex(basicProgram, "ViewProj");
	auto blockIndex2 = glGetUniformBlockIndex(sphereProgram, "ViewProj");
	auto blockIndex3 = glGetUniformBlockIndex(sceneProgram, "ViewProj");

	glUniformBlockBinding(basicProgram, blockIndex, id_blockBind);
	glUniformBlockBinding(sphereProgram, blockIndex2, id_blockBind);
	glUniformBlockBinding(sceneProgram, blockIndex3, id_blockBind);

	previousTime = glutGet(GLUT_ELAPSED_TIME);

	character.InitMesh("bear-obj.obj", "bear.tga");
	sphere.InitMesh("rock.obj", "Rock-Texture-Surface.jpg");

	glBindVertexArray(character.VAO);
	glBindVertexArray(character.IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, character.IBO);
	
	glBindVertexArray(sphere.VAO);
	glBindVertexArray(sphere.IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.IBO);
	
	glBindVertexArray(0);
	










	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	//creationVBO

	g_Camera.translation = glm::vec3(0, 0, 0);
}

void Render()
{
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glClearColor(1.f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto width = glutGet(GLUT_WINDOW_WIDTH);
	auto height = glutGet(GLUT_WINDOW_HEIGHT);
	
	g_Camera.projectionMatrix = glm::perspectiveFov(45.f, (float)width, (float)height, 0.1f, 1000.f);
	// rotation orbitale de la camera
	float rotY = glm::radians(g_Camera.rotation.y);
	g_Camera.position = glm::eulerAngleY(rotY) * glm::vec4(0.0f, 20.0f, 50.0f, 1.0f);
	g_Camera.viewMatrix = glm::lookAt(glm::vec3(g_Camera.position), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
	g_Camera.viewMatrix = glm::translate(g_Camera.viewMatrix, g_Camera.translation);
	// IL FAUT TRANSFERER LES MATRICES VIEW ET PROJ AU SHADER
	glBindBuffer(GL_UNIFORM_BUFFER, g_Camera.UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, glm::value_ptr(g_Camera.viewMatrix), GL_STREAM_DRAW);

	sphere.DisplayObj(glm::vec3(0.f, 0.f, 0.f));
	character.DisplayObj(glm::vec3(0.f, 0.f, 0.f));
	character.DisplayObj(glm::vec3(10.f, 0.f, 0.f));
	
	//auto program = character.GetShader();
	//glUseProgram(program);


	///////////////////////////RENDER MUR///////////////////

	// rendu des murs avec illumination	
	
	glBindVertexArray(g_WallMesh.VAO);
	auto program = Scene.GetShader();
	/*if (g_EnableDeferred)
	{
		program = g_EnablePositionTexture ? g_GBufferFatShader.GetProgram() : g_GBufferShader.GetProgram();

	}
	else if (g_EnableMultipass) {
		program = shaderClasses[MULTIPASS]->GetProgram();
	}*/

	glUseProgram(program);
	auto worldLocation = glGetUniformLocation(program, "u_worldMatrix");
	glm::mat4& transform = g_Walls.worldMatrix;
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, glm::value_ptr(transform));

	//auto numLightsLocation = glGetUniformLocation(program, "u_numLights");
	//glUniform1i(numLightsLocation, g_NumPointLights);

	auto startIndex = 6;
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gWallTexture]);
	glDrawArrays(GL_TRIANGLES, startIndex, 6 * 3); startIndex += 6 * 3;	// 4 murs
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gCeilingTexture]);
	glDrawArrays(GL_TRIANGLES, startIndex, 6); startIndex += 6;	// plafond
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gFloorTexture]);
	glDrawArrays(GL_TRIANGLES, startIndex, 6); startIndex += 6;	// sol



		
	///////////////////////////////////
	
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