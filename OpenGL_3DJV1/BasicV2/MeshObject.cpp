
#include "MeshObject.h"

#ifndef BUFFER_OFFSET

#define BUFFER_OFFSET(offset) ((char*)NULL + (offset))

#endif


MeshObject::MeshObject()
{
	Norm = false;
	TextureCoord = false;
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

GLuint MeshObject::GetShader()
{
	return sceneShader.GetProgram();
}

void MeshObject::InitMesh(std::string inputfileObj, std::string inputfileTexture)
{

	std::string inputfile = inputfileObj;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err = tinyobj::LoadObj(shapes, materials, inputfile.c_str());

	if (!err.empty())
	{
		std::cout << err << std::endl;
		std::cin.ignore();
		exit(-1);
	}
	else
		std::cout << "Chargement de l'objet reussi !" << std::endl;

	const std::vector<unsigned int>& indices = shapes[0].mesh.indices;
	const std::vector<float>& positions = shapes[0].mesh.positions;
	const std::vector<float>& normals = shapes[0].mesh.normals;
	const std::vector<float>& texcoords = shapes[0].mesh.texcoords;

	int nbElement = positions.size();

	if (normals.size() > 0)
	{
		nbElement += normals.size();
		Norm = true;
		std::cout << "Ajout des infos sur la présence des normales" << std::endl;
	}
	else
		std::cout << "Aucune normales " << std::endl;

	if (texcoords.size() > 0)
	{
		if (LoadAndCreateTextureRGBA(inputfileTexture.c_str(), textureID) == false)
		{
			std::cout << "Erreur dans le chargement de la texture !" << std::endl;
			std::cin.ignore();
			exit(-1);
		}
		else
		{
			nbElement += texcoords.size();
			TextureCoord = true;

			std::cout << "Chargement de la texture reussi !" << std::endl;
		}
	}
	else
		std::cout << "Le fichier ne contient pas de texCoord" << std::endl;

	position = glm::vec3(0.f, 0.f, 20.f);
	ElementCount = indices.size();

	// VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * nbElement), NULL, GL_STATIC_DRAW);

	float* vbo_buffer = static_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	int vbo_Vertex = 0;
	int vbo_Normals = 0;
	int vbo_TexCoords = 0;
	int it = 0;

	for (int i = 0; i < nbElement; ++i)
	{
		if (it >= 8)
		{
			it = 0;
		}
		if (it >= 0 && it <= 2)
		{
			vbo_buffer[i] = positions[vbo_Vertex];
			vbo_Vertex++;
		}
		if (Norm && (it >= 3 && it <= 5))
		{
			vbo_buffer[i] = normals[vbo_Normals];
			vbo_Normals++;
		}
		if (TextureCoord && (it >= 6 && it <= 7))
		{
			vbo_buffer[i] = texcoords[vbo_TexCoords];
			vbo_TexCoords++;
		}
		it++;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	// IBO
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * ElementCount, &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	int stride = (3 + (Norm * 3) + (TextureCoord * 2))*sizeof(float);
	int offset = 3 * sizeof(float);
	if (Norm)
	{
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		offset += sizeof(float) * 3;
	}
	if (TextureCoord)
	{
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	}
	glBindVertexArray(0);
}
/*
bool MeshObject::LoadAndCreateTextureRGBA(const char *filename, GLuint &texID)
{
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// il est obligatoire de specifier une valeur pour GL_TEXTURE_MIN_FILTER
	// autrement le Texture Object est considere comme invalide
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int w, h;
	uint8_t *data = stbi_load(filename, &w, &h, nullptr, STBI_rgb_alpha);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	return (data != nullptr);
}*/

void MeshObject::DisplayObj(glm::vec3 &position)
{
	//auto width = glutGet(GLUT_WINDOW_WIDTH);
	//auto height = glutGet(GLUT_WINDOW_HEIGHT);


	

	auto program = GetShader();
	glUseProgram(program);

	// variables uniformes (constantes) 

	

	auto worldLocation = glGetUniformLocation(program, "u_worldMatrix");

	glm::mat4& transform = worldMatrix;
	transform[3] = glm::vec4(position,1.0f);

	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, glm::value_ptr(transform));

	auto textureLocation = glGetUniformLocation(program, "u_texture");
	glUniform1i(textureLocation, 0);

	glBindVertexArray(VAO);
	glBindVertexArray(IBO);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	

}