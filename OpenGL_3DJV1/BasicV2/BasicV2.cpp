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

//Liste des objets present dans la scene
MeshObject babyCube;
MeshObject plane;
MeshObject desk;
MeshObject bed;

MeshObject CubeMap;

MeshObject ShadowMap;


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
	glm::vec3 position;
	glm::vec3 translation;
	GLuint UBO;

	// Vecteurs orientation de la caméra
	glm::vec3 forward;
	glm::vec3 right;
} g_Camera;

void InitCubemap()
{
	static const float skyboxVertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	static const char* skyboxFiles[] = {
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/back.jpg",
		"skybox/front.jpg",

		/*"skybox/Powerlines/posx.jpg",
		"skybox/Powerlines/negx.jpg",
		"skybox/Powerlines/posy.jpg",
		"skybox/Powerlines/negy.jpg",
		"skybox/Powerlines/posz.jpg",
		"skybox/Powerlines/negz.jpg",*/

		/*"skybox/Park3/posx.jpg",
		"skybox/Park3/negx.jpg",
		"skybox/Park3/posy.jpg",
		"skybox/Park3/negy.jpg",
		"skybox/Park3/posz.jpg",
		"skybox/Park3/negz.jpg",*/
	};

	LoadAndCreateCubeMap(skyboxFiles, CubeMap.textureID);

	glGenVertexArrays(1, &CubeMap.VAO);
	glGenBuffers(1, &CubeMap.VBO);
	glBindVertexArray(CubeMap.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, CubeMap.VBO);

	// RAPPEL: sizeof(skyboxVertices) fonctionne -cad qu'on obtient la taille totale du tableau-
	//			du fait que skyboxVertices est un tableau STATIC et donc que le compilateur peut deduire
	//			sa taille lors de la compilation. Autrement sizeof aurait du renvoyer la taille du pointeur.
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	CubeMap.InitShader("skybox");
	//g_SkyboxShader.LoadVertexShader("skybox.vs");
	//g_SkyboxShader.LoadFragmentShader("skybox.fs");
	//g_SkyboxShader.Create();

	auto program = CubeMap.GetShader();
	glUseProgram(program);

	// l'UBO binde en 0 sera egalement connecte au shader skybox
	auto blockIndex = glGetUniformBlockIndex(program, "ViewProj");
	glUniformBlockBinding(program, blockIndex, 0);
	auto cubemapIndex = glGetUniformLocation(program, "u_CubeMap");

	// pour le shader, la skybox utilisera l'unite de texture 0 mais il est possible d'utiliser 
	// un index specifique pour la cubemap
	glUniform1i(cubemapIndex, 0);
	glUseProgram(0);
}







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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	
	/////////////////////SCENE INIT////////////////////
	glGenBuffers(1, &Material::UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, Material::UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), &g_ShinyMaterial, GL_STATIC_DRAW);

	LoadMesh(g_WallMesh, g_Room);


	LoadAndCreateTextureRGBA("parquet22.jpg", g_Walls.textures[Walls::gWallTexture]);
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gWallTexture]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	LoadAndCreateTextureRGBA("floor111.jpg", g_Walls.textures[Walls::gFloorTexture]);
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

	character.InitMesh("assets/obj/bear-obj.obj", "assets/obj/bear.tga");
	sphere.InitMesh("rock.obj", "Rock-Texture-Surface.jpg");

	character.BindingForInit();
	sphere.BindingForInit();
	
	//EST remplacé par les deux lignes au dessus
	/*glBindVertexArray(character.VAO);
	glBindVertexArray(character.IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, character.IBO);
	
	glBindVertexArray(sphere.VAO);
	glBindVertexArray(sphere.IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.IBO);*/
	
	

	glBindVertexArray(0);
	
	Scene.CreateFBO(800, 600);


	InitCubemap();

	//creationVBO

	g_Camera.translation = glm::vec3(0, 0, 0);
}

void Render()
{
	////TEST FBO ////
	glBindFramebuffer(GL_FRAMEBUFFER, Scene.sceneFBO);
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glClearColor(1.f, 0.0f, 0.5f, 1.0f);
	glClearDepth(1.F);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint program = Scene.GetShader();
	glUseProgram(program);

	auto startIndex = 6;
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gWallTexture]);
	glDrawArrays(GL_TRIANGLES, startIndex, 6 * 3);
	startIndex += 6 * 3;	// 4 murs
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gCeilingTexture]);
	glDrawArrays(GL_TRIANGLES, startIndex, 6);
	startIndex += 6;	// plafond
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gFloorTexture]);
	glDrawArrays(GL_TRIANGLES, startIndex, 6);
	startIndex += 6;

	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	////TEST FBO////

	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glClearColor(1.f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto width = glutGet(GLUT_WINDOW_WIDTH);
	auto height = glutGet(GLUT_WINDOW_HEIGHT);
	
	g_Camera.projectionMatrix = glm::perspectiveFov(45.f, (float)width, (float)height, 0.1f, 1000.f);
	// rotation orbitale de la camera

	// OLD
	//float rotY = glm::radians(g_Camera.rotation.y);
	//g_Camera.position = glm::eulerAngleY(rotY) * glm::vec3(0.0f, 20.0f, 50.0f);
	//g_Camera.viewMatrix = glm::lookAt(glm::vec3(g_Camera.position), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
	//g_Camera.viewMatrix = glm::translate(g_Camera.viewMatrix, g_Camera.translation);

	// NEW
	g_Camera.projectionMatrix = glm::perspectiveFov(45.f, (float) width, (float) height, 0.1f, 1000.f);
	glm::vec3 position = g_Camera.position;
	glm::vec3 direction = g_Camera.forward;
	g_Camera.viewMatrix = glm::lookAt(position, position + direction, glm::vec3(0.f, 1.f, 0.f));

	// IL FAUT TRANSFERER LES MATRICES VIEW ET PROJ AU SHADER
	glBindBuffer(GL_UNIFORM_BUFFER, g_Camera.UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, glm::value_ptr(g_Camera.viewMatrix), GL_STREAM_DRAW);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * 2, glm::value_ptr(g_Camera.viewMatrix));

	glUseProgram(CubeMap.GetShader());

	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap.textureID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(CubeMap.VAO);

	// Tres important ! D'une part, comme la cubemap represente un environnement distant
	// il n'est pas utile d'ecrire dans le depth buffer (on est toujours au plus loin)
	// cependant il faut quand effectuer le test de profondeur (donc on n'a pas glDisable(GL_DEPTH_TEST)).
	// Neamoins il faut legerement changer l'operateur du test dans le cas ou 
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 8 * 2 * 3);

	// On reset les machins
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/////////////////// AFFICHAGE DES OBJETS ///////////////////////////
	sphere.DisplayObj(glm::vec3(0.f, 0.f, 0.f));
	character.DisplayObj(glm::vec3(0.f, 0.f, 0.f));
	character.DisplayObj(glm::vec3(10.f, 0.f, -128.f));
	



	///////////////////////////RENDER MUR///////////////////

	// rendu des murs
	
	glBindVertexArray(g_WallMesh.VAO);
	 program = Scene.GetShader();
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

	 startIndex = 6;
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gWallTexture]);
	glDrawArrays(GL_TRIANGLES, startIndex, 6 * 3);
	startIndex += 6 * 3;	// 4 murs
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gCeilingTexture]);
	glDrawArrays(GL_TRIANGLES, startIndex, 6);
	startIndex += 6;	// plafond
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gFloorTexture]);
	glDrawArrays(GL_TRIANGLES, startIndex, 6); 
	startIndex += 6;	// sol



		
	///////////////////////////////////
	

	


	glutSwapBuffers();
	glutPostRedisplay();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float horizontalAngleCamera = 3.14f;				// Initial horizontal angle : toward -Z
float verticalAngleCamera = 0.0f;					// Initial vertical angle : none
float mouseSpeedCamera = 0.005f;
float mouseSpeedMoveObject = 0.05f;
float movementSpeed = 50.0f;

int oldX;
int oldY;
int deltaX;
int deltaY;
unsigned char keyState[255];
unsigned char mouseButtonsState[10];

void Update() {
	///////////////////////////////////////////////////////////////////////////////////// Calcul du temps écoulé (pour que la puissance du PC influe pas)
	auto currentTime = glutGet(GLUT_ELAPSED_TIME);
	auto delta = currentTime - previousTime;
	previousTime = currentTime;
	auto elapsedTime = delta / 1000.0f;

	///////////////////////////////////////////////////////////////////////////////////// Gestion du clavier (principalement déplacement)
	if(keyState['z'] == GLUT_DOWN) {
		g_Camera.position += elapsedTime* g_Camera.forward * movementSpeed;
	}
	else if(keyState['s'] == GLUT_DOWN) {
		g_Camera.position -= elapsedTime* g_Camera.forward * movementSpeed;
	}

	if(keyState['q'] == GLUT_DOWN) {
		g_Camera.position -= elapsedTime* g_Camera.right * movementSpeed;
	}
	else if(keyState['d'] == GLUT_DOWN) {
		g_Camera.position += elapsedTime* g_Camera.right * movementSpeed;
	}

	if(keyState['e'] == GLUT_DOWN || keyState[' '] == GLUT_DOWN) {
		g_Camera.position.y += elapsedTime * movementSpeed;
	}
	else if(keyState['a'] == GLUT_DOWN) {
		g_Camera.position.y -= elapsedTime * movementSpeed;
	}

	if(keyState[27] == GLUT_DOWN) {
		exit(0);
	}

	///////////////////////////////////////////////////////////////////////////////////// Gestion de la souris (drag)
	// TODO: difficile à déplacer :/
	if(mouseButtonsState[GLUT_LEFT_BUTTON] == GLUT_DOWN || mouseButtonsState[GLUT_RIGHT_BUTTON] == GLUT_DOWN) {
		if(mouseButtonsState[GLUT_LEFT_BUTTON] == GLUT_DOWN) {

		}
		else if(mouseButtonsState[GLUT_RIGHT_BUTTON] == GLUT_DOWN) {

		}
	}

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	mouseButtonsState[button] = state;
		if(state == GLUT_DOWN) {
			oldX = x;
			oldY = y;
		}
	glutPostRedisplay();
}

void motion(int x, int y) {
		deltaX = oldX - x;
		deltaY = oldY - y;

		int width = glutGet(GLUT_WINDOW_WIDTH);
		int height = glutGet(GLUT_WINDOW_HEIGHT);

		Quaternion qX(
			sin(((y) * M_PI) / height),
			0,
			0,
			cos(((y) * M_PI) / height)
			);
		Quaternion qY(
			0,
			sin(((x) * M_PI) / width),
			0,
			cos(((x) * M_PI) / width)
			);

		Quaternion rotation = qY * qX;

		// Rotation camera
		if(mouseButtonsState[GLUT_RIGHT_BUTTON] == GLUT_DOWN) {
			horizontalAngleCamera += mouseSpeedCamera * deltaX;
			verticalAngleCamera += mouseSpeedCamera * deltaY;

			// Vecteur avant
			glm::vec3 forward = glm::vec3(
				cos(verticalAngleCamera) * sin(horizontalAngleCamera),
				sin(verticalAngleCamera),
				cos(verticalAngleCamera) * cos(horizontalAngleCamera)
				);

			// Vecteur droite
			glm::vec3 right = glm::vec3(
				sin(horizontalAngleCamera - M_PI / 2.0f),
				0,
				cos(horizontalAngleCamera - M_PI / 2.0f)
				);

			// Vecteur haut
			glm::vec3 up = glm::cross(g_Camera.forward, g_Camera.right);

			g_Camera.forward = forward;
			g_Camera.right = right;
			//g_Camera.rotationMatrix = rotation.toRotationMatrix();
		}

		oldX = x;
		oldY = y;
	

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	keyState[key] = GLUT_DOWN;
}

void keyboardUp(unsigned char key, int x, int y) {
	keyState[key] = GLUT_UP;
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

	// Init du clavier
	for(int i = 0; i < 256; i++) {
		keyState[i] = GLUT_UP;
	}

	// Init de la souris
	for(int i = 0; i < 10; i++) {
		mouseButtonsState[i] = GLUT_UP;
	}

	g_Camera.position = glm::vec3(0.0f, 15.0f, 35.0f);
	g_Camera.forward = glm::vec3(0.0f, 0.0f, -1.0f);
	g_Camera.right = glm::vec3(1.0f, 0.0f, 0.0f);

	Initialize();

	glutIdleFunc(Update);
	glutDisplayFunc(Render);

	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);

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


