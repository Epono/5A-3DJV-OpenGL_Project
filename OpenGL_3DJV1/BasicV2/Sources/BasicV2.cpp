#include "Common.h"

#include "MeshObject.h"
#include "Scene.h"

///////////////// MACRO UTILE AU VBO ///////////////////
#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(offset) ((char*)NULL + (offset))
#endif
///////////////// MACRO UTILE AU VBO ///////////////////

///////////////// OBJETS DANS LA SCENE ///////////////////
MeshObject babyCube;
MeshObject plane;
MeshObject desk;
MeshObject bed;

MeshObject CubeMap;

MeshObject ShadowMap;

MeshObject aCube;
MeshObject bCube;
MeshObject rock;
MeshObject bear;
MeshObject ground;
MeshObject scene;
///////////////// OBJETS DANS LA SCENE ///////////////////

///////////////// VARIABLES GLOBALES QU'ON LES AIME ///////////////////
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_NAME "OpenGLAGLA"

int previousTime = 0;

float horizontalAngleCamera = 3.14f;				// Initial horizontal angle : toward -Z
float verticalAngleCamera = 0.0f;					// Initial vertical angle : none
float mouseSpeedCamera = 0.005f;
float movementSpeed = 50.0f;

int oldX, oldY;
unsigned char keyState[255], mouseButtonsState[3];

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
///////////////// VARIABLES GLOBALES QU'ON LES AIME ///////////////////

///////////////// PROTOTYPES ///////////////////
void InitializeOpenGL();
void InitCubemap();
void InitializeLogic();

void Render();
void RenderScene(EsgiShader &shader);
void Update();

void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Keyboard(unsigned char key, int x, int y);
void KeyboardUp(unsigned char key, int x, int y);

void RenderOld();
///////////////// PROTOTYPES ///////////////////

///////////////// STRUCTURE CAMERA ///////////////////
struct ViewProj {
	// Matrices de la caméra
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	// Transform de la caméra
	glm::vec3 rotation;
	glm::vec3 position;

	// UBO de la caméra
	GLuint UBO;

	// Vecteurs orientation de la caméra
	glm::vec3 forward;
	glm::vec3 right;
} g_Camera;
///////////////// STRUCTURE CAMERA ///////////////////

///////////////// SHADOW MAPPING ///////////////////
EsgiShader shader;
EsgiShader simpleDepthShader;
EsgiShader debugDepthQuad;

// Light source
glm::vec3 lightPos(0.0f, 10.0f, 0.0f);

const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
GLuint depthMapFBO;
GLuint depthMap;
///////////////// SHADOW MAPPING ///////////////////

////////////////////////////////////////////////////////////////////////////////// main ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow(WINDOW_NAME);

#ifdef FREEGLUT
	// Note: glutSetOption n'est disponible qu'avec freeGLUT
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif

	InitializeOpenGL();
	InitializeLogic();

	glutIdleFunc(Update);
	glutDisplayFunc(Render);

	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);

	glutMainLoop();

	return 0;
}

////////////////////////////////////////////////////////////////////////////////// InitCubemap ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void InitCubemap() {
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
		"Assets/Skybox/Park3/posx.jpg",
		"Assets/Skybox/Park3/negx.jpg",
		"Assets/Skybox/Park3/posy.jpg",
		"Assets/Skybox/Park3/negy.jpg",
		"Assets/Skybox/Park3/posz.jpg",
		"Assets/Skybox/Park3/negz.jpg",
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);
	glBindVertexArray(0);

	CubeMap.InitShader("Shaders/skybox");

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

////////////////////////////////////////////////////////////////////////////////// InitializeOpenGL ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void InitializeOpenGL() {
	printf("Version Pilote OpenGL : %s\n", glGetString(GL_VERSION));
	printf("Type de GPU : %s\n", glGetString(GL_RENDERER));
	printf("Fabricant : %s\n", glGetString(GL_VENDOR));
	int numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	//Init GLEW		
	GLenum init = glewInit();
	if(init != GL_NO_ERROR) {

	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	/////////////////////SCENE INIT////////////////////
	glGenBuffers(1, &Material::UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, Material::UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), &g_ShinyMaterial, GL_STATIC_DRAW);

	LoadMesh(g_WallMesh, g_Room);

	LoadAndCreateTextureRGBA("Assets/Textures/parquet22.jpg", g_Walls.textures[Walls::gWallTexture]);
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gWallTexture]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	LoadAndCreateTextureRGBA("Assets/Textures/floor111.jpg", g_Walls.textures[Walls::gFloorTexture]);
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gFloorTexture]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	LoadAndCreateTextureRGBA("Assets/Textures/ceiling_color_map.jpg", g_Walls.textures[Walls::gCeilingTexture]);
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gCeilingTexture]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	scene.InitShader("Shaders/scene");
	//////////////////////////////////////////////////////////////////////////////////////

	//Initialisation caméra 
	glGenBuffers(1, &g_Camera.UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, g_Camera.UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_STREAM_DRAW);

	GLuint id_blockBind = 1;

	glBindBufferBase(GL_UNIFORM_BUFFER, id_blockBind, g_Camera.UBO);

	//Initialisation des meshes
	bear.InitShader("Shaders/basic");
	rock.InitShader("Shaders/basic");
	auto bearProgram = bear.GetShader();
	auto rockProgram = rock.GetShader();
	auto sceneProgram = scene.GetShader();

	auto blockIndex = glGetUniformBlockIndex(bearProgram, "ViewProj");
	auto blockIndex2 = glGetUniformBlockIndex(rockProgram, "ViewProj");
	auto blockIndex3 = glGetUniformBlockIndex(sceneProgram, "ViewProj");

	glUniformBlockBinding(bearProgram, blockIndex, id_blockBind);
	glUniformBlockBinding(rockProgram, blockIndex2, id_blockBind);
	glUniformBlockBinding(sceneProgram, blockIndex3, id_blockBind);

	previousTime = glutGet(GLUT_ELAPSED_TIME);

	bear.InitMesh("Assets/Obj/bear-obj.obj", "Assets/Obj/bear.tga");
	rock.InitMesh("Assets/Obj/rock.obj", "Assets/Obj/Rock-Texture-Surface.jpg");

	bear.BindingForInit();
	rock.BindingForInit();

	glBindVertexArray(0);

	scene.CreateFBO(WINDOW_WIDTH, WINDOW_HEIGHT);

	InitCubemap();

	//creationVBO

	/////////////////////////////////////////////////////// Shadow mapping
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	// Define the viewport dimensions
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Setup and compile our shaders
	shader.LoadVertexShader("Shaders/ShadowMapping/shadow_mapping.vs");
	shader.LoadFragmentShader("Shaders/ShadowMapping/shadow_mapping.fs");
	shader.Create();

	simpleDepthShader.LoadVertexShader("Shaders/ShadowMapping/shadow_mapping_depth.vs");
	simpleDepthShader.LoadFragmentShader("Shaders/ShadowMapping/shadow_mapping_depth.fs");
	simpleDepthShader.Create();

	debugDepthQuad.LoadVertexShader("Shaders/ShadowMapping/debug_quad.vs");
	debugDepthQuad.LoadFragmentShader("Shaders/ShadowMapping/debug_quad_depth.fs");
	debugDepthQuad.Create();

	// Set texture samples
	auto program = shader.GetProgram();
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "diffuseTexture"), 0);
	glUniform1i(glGetUniformLocation(program, "shadowMap"), 1);

	// Configure depth map FBO
	glGenFramebuffers(1, &depthMapFBO);
	// - Create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

////////////////////////////////////////////////////////////////////////////////// Render ///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Render() {
	// 1. Render depth of scene to texture (from light's perspective)
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	GLfloat near_plane = 1.0f, far_plane = 2000.0f;
	lightProjection = glm::perspective(60.f, (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	lightSpaceMatrix = lightProjection * lightView;
	// - now render scene from light's point of view
	auto program = simpleDepthShader.GetProgram();
	glUseProgram(program);
	glCullFace(GL_CW);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	RenderScene(simpleDepthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. Render scene as normal
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	program = shader.GetProgram();
	glCullFace(GL_CCW);
	glUseProgram(program);

	// l'autre
	//glm::mat4 projection = glm::perspective(camera.Zoom, (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
	//glm::mat4 view = camera.GetViewMatrix();
	//glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	//glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

	// le notre
	//g_Camera.projectionMatrix = glm::perspectiveFov(45.f, (float) glutGet(GLUT_WINDOW_WIDTH), (float) glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 1000.f);
	//glm::vec3 position = g_Camera.position;
	//glm::vec3 direction = g_Camera.forward;
	//g_Camera.viewMatrix = glm::lookAt(position, position + direction, glm::vec3(0.f, 1.f, 0.f));
	//glBindBuffer(GL_UNIFORM_BUFFER, g_Camera.UBO);
	//glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, glm::value_ptr(g_Camera.viewMatrix), GL_STREAM_DRAW);

	// Le merge
	g_Camera.projectionMatrix = glm::perspectiveFov(45.f, (float) glutGet(GLUT_WINDOW_WIDTH), (float) glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 500.f);
	glm::vec3 position = g_Camera.position;
	glm::vec3 direction = g_Camera.forward;
	g_Camera.viewMatrix = glm::lookAt(position, position + direction, glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(g_Camera.projectionMatrix));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(g_Camera.viewMatrix));

	// Set light uniforms
	glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, &lightPos[0]);
	glUniform3fv(glGetUniformLocation(program, "viewPos"), 1, &g_Camera.position[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	// Enable/Disable shadows by pressing 'SPACE'
	glUniform1i(glGetUniformLocation(program, "shadows"), true);

	// L'autre
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, woodTexture);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, depthMap);

	// Le notre (on active/bind d'abord la depthMap, puis on réactive/bind la texture 0 pour chaque objet (vu qu'on a pas les mêmes textures)
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glActiveTexture(GL_TEXTURE0);
	// 

	RenderScene(shader);

	////////////////////////////////////////////////////////// SKYBOX
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
	////////////////////////////////////////////////////////// SKYBOX

	glutSwapBuffers();
	glutPostRedisplay();
}

void RenderScene(EsgiShader &shader) 
{
	/////////////////// AFFICHAGE DES OBJETS ///////////////////////////
	rock.DisplayObjShadowMap(glm::vec3(lightPos), shader);
	bear.DisplayObjShadowMap(glm::vec3(0.f, 0.f, 0.f), shader);
	bear.DisplayObjShadowMap(glm::vec3(10.f, 0.f, -128.f), shader);


	///////////////////////////RENDER MUR///////////////////

	glBindVertexArray(g_WallMesh.VAO);
	auto worldLocation = glGetUniformLocation(shader.GetProgram(), "model");
	glm::mat4& transform = g_Walls.worldMatrix;
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, glm::value_ptr(transform));

	auto startIndex = 6;
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gWallTexture]);
	glDrawArrays(GL_TRIANGLES, startIndex, 6 * 3);
	startIndex += 6 * 3;																// 4 murs
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gCeilingTexture]);
	glDrawArrays(GL_TRIANGLES, startIndex, 6);
	startIndex += 6;																	// plafond
	glBindTexture(GL_TEXTURE_2D, g_Walls.textures[Walls::gFloorTexture]);
	glDrawArrays(GL_TRIANGLES, startIndex, 6);
	startIndex += 6;																	// sol

	///////////////////////////RENDER MUR///////////////////
}

////////////////////////////////////////////////////////////////////////////////// InitializeLogic ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void InitializeLogic() {
	// Init du clavier
	for(int i = 0; i < 255; i++) {
		keyState[i] = GLUT_UP;
	}

	// Init de la souris
	for(int i = 0; i < 3; i++) {
		mouseButtonsState[i] = GLUT_UP;
	}

	g_Camera.position = glm::vec3(0.0f, 15.0f, 35.0f);
	g_Camera.forward = glm::vec3(0.0f, 0.0f, -1.0f);
	g_Camera.right = glm::vec3(1.0f, 0.0f, 0.0f);
}

////////////////////////////////////////////////////////////////////////////////// Update ///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Update() {
	// Calcul du temps écoulé (pour que la puissance du PC influe pas)
	auto currentTime = glutGet(GLUT_ELAPSED_TIME);
	auto delta = currentTime - previousTime;
	previousTime = currentTime;
	auto elapsedTime = delta / 1000.0f;

	// Gestion du clavier (principalement déplacement)
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

	//
	if(keyState['t'] == GLUT_DOWN) {
		lightPos += glm::vec3(g_Camera.forward.x, 0, g_Camera.forward.z);
	}
	else if(keyState['g'] == GLUT_DOWN) {
		lightPos -= glm::vec3(g_Camera.forward.x, 0, g_Camera.forward.z);
	}

	if(keyState['f'] == GLUT_DOWN) {
		lightPos -= glm::vec3(g_Camera.right.x, 0, g_Camera.right.z);
	}
	else if(keyState['h'] == GLUT_DOWN) {
		lightPos += glm::vec3(g_Camera.right.x, 0, g_Camera.right.z);
	}

	if(keyState['r'] == GLUT_DOWN) {
		lightPos.y -= 1 ;
	}
	else if(keyState['y'] == GLUT_DOWN) {
		lightPos.y += 1 ;
	}

	if(keyState[27] == GLUT_DOWN) {
		exit(0);
	}

	// Gestion de la souris (drag)
	if(mouseButtonsState[GLUT_LEFT_BUTTON] == GLUT_DOWN || mouseButtonsState[GLUT_RIGHT_BUTTON] == GLUT_DOWN) {
		if(mouseButtonsState[GLUT_LEFT_BUTTON] == GLUT_DOWN) {

		}
		else if(mouseButtonsState[GLUT_RIGHT_BUTTON] == GLUT_DOWN) {

		}
	}

	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////// Mouse ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Mouse(int button, int state, int x, int y) {
	mouseButtonsState[button] = state;
	if(state == GLUT_DOWN) {
		oldX = x;
		oldY = y;
	}
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////// Motion ///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Motion(int x, int y) {
	auto deltaX = oldX - x;
	auto deltaY = oldY - y;

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

////////////////////////////////////////////////////////////////////////////////// Keyboard ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Keyboard(unsigned char key, int x, int y) {
	keyState[key] = GLUT_DOWN;
}

////////////////////////////////////////////////////////////////////////////////// KeyboardUp ///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void KeyboardUp(unsigned char key, int x, int y) {
	keyState[key] = GLUT_UP;
}











/* OLD RENDER */

void RenderOld() {


	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glClearColor(1.f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto width = glutGet(GLUT_WINDOW_WIDTH);
	auto height = glutGet(GLUT_WINDOW_HEIGHT);

	g_Camera.projectionMatrix = glm::perspectiveFov(45.f, (float) width, (float) height, 0.1f, 1000.f);

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
	rock.DisplayObj(glm::vec3(0.f, 0.f, 0.f));
	bear.DisplayObj(glm::vec3(0.f, 0.f, 0.f));
	bear.DisplayObj(glm::vec3(10.f, 0.f, -128.f));




	///////////////////////////RENDER MUR///////////////////

	// rendu des murs

	glBindVertexArray(g_WallMesh.VAO);
	auto program = scene.GetShader();
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
