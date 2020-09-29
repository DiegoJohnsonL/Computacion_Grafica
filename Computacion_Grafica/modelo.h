#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>'
#include "transformaciones.hpp"
#include <vector>
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

using namespace std;
using namespace glm;


enum class TipoArchivo{	Normal, Color, Textura};

class CModel {
	
public:
	TipoArchivo tipoArchivo;
	string name, filePath;
	GLfloat* vertices;
	GLuint* indices;
	vector<Transformacion> transformaciones;

	GLuint idArrayVertices, idArrayBuffers, idElementBuffer;
	int numVertices, numIndices, verticesInFila;
	int vBufferSize, iBufferSize;

	CModel(string filePath) {
		//Reads file
		this->filePath = filePath;
		this->readFile();
		//Add conditionals for other files formats
	}
	~CModel() {
		if(this->idArrayVertices != NULL)glDeleteVertexArrays(1, &this->idArrayVertices);
		if(this->idArrayBuffers  != NULL)glDeleteBuffers(1, &this->idArrayBuffers);
		if(this->idElementBuffer != NULL)glDeleteBuffers(1, &this->idElementBuffer);
	}

	void setBuffers() {
		this->defBuffers();
		this->indexAndLoadBuffers();
		this->setAttributes();
	}

	void draw() {
		glBindVertexArray(this->idArrayVertices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idElementBuffer);
		if (this->tipoArchivo == TipoArchivo::Textura) {
			glDrawArrays(GL_TRIANGLES, 0, vBufferSize);
		}
		else{
			glDrawElements(GL_TRIANGLES, this->numIndices * 3, GL_UNSIGNED_INT, 0);
		}
	}

	void setTextures(const char* path) {
		unsigned int texture[2];
		glGenTextures(1, &texture[0]);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load and generate the texture
		int width, height, nrChannels;
		// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
		unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}


	
private:
	//Add funtion to use class "Transformaciones" with a vector of transforations returning a Mat4 to sent in CPrograma.setMat4()
	mat4 getTransformacion() {
		mat4 transformacion;
		for (int i = 0; i < this->transformaciones.size(); i++)
		{
			transformacion *= transformaciones.at(i).transformar();
		}
		return transformacion;
	}
	void defBuffers() {
		//Enviando la geometria al gpu: Definiendo los buffers (Vertex Array Objects y Aertex Buffer Objets)
		//VAO  ABO
		glGenVertexArrays(1, &this->idArrayVertices);
		glGenBuffers(1, &this->idArrayBuffers);
		glGenBuffers(1, &this->idElementBuffer);
	} 
	//Anexando los buffers para su ejecucion en la targeta grafica
	//Anexando buffers y cargando los buffer con los datos	
	void indexAndLoadBuffers() {
		glBindVertexArray(this->idArrayVertices);
		this->vBufferSize = this->numVertices * this->verticesInFila * sizeof(GLfloat);
		this->iBufferSize = this->numIndices * 3 * sizeof(GLuint);
		if (tipoArchivo == TipoArchivo::Normal || tipoArchivo == TipoArchivo::Color)
		{	
			glBindBuffer(GL_ARRAY_BUFFER, this->idArrayBuffers);
			glBufferData(GL_ARRAY_BUFFER, vBufferSize, this->vertices, GL_STATIC_DRAW); //vertices*sizeof(float), reservando en memoria de el gpu, || GL_Dynamic_DRAW || GL_Stream_DRAW 

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idElementBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, iBufferSize, this->indices, GL_STATIC_DRAW);
		}
		else {
			glBindBuffer(GL_ARRAY_BUFFER, this->idArrayBuffers);
			glBufferData(GL_ARRAY_BUFFER, vBufferSize, this->vertices, GL_STATIC_DRAW);
		}		
	}
	void setAttributes() {
		//(Atributo location = n del vertex shader, cantidad de datos del vec en locations 0, flotantes , false,  moverte n atributos en el arreglo de vertices, empezar en la posicion n despues del puntero)
		switch (this->tipoArchivo)
		{
		case TipoArchivo::Normal:
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			break;

		case TipoArchivo::Color:
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);			
			break;

		case TipoArchivo::Textura:	
			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// texture coord attribute
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			break;
		}
	}
	void readFile() {
		ifstream file;
		int temp;
		file.open(this->filePath);

		if (file.is_open()) {
			// OFF	
			file >> name;
			file >> this->numVertices >> this->numIndices >> temp;

			string nombre = this->filePath;
			string strTipo = nombre.substr(4, 5);


			cout << "Nombre del Archivo: " << this->filePath << endl;
			cout << "Contiene atributos de: ";
			if (strTipo == "color")
				cout << "Color" << endl;

			else if (strTipo == "textu")
				cout << "textura" << endl;
			else
				cout << "solo posicion" << endl;

			if (strTipo == "color") {
				this->tipoArchivo = TipoArchivo::Color;
				this->verticesInFila = 6;
				float x, y, z, r, g, b;

				this->vertices = new float[this->numVertices * 6];
				this->indices = new GLuint[this->numIndices * 3];

				for (int i = 0; i < this->numVertices; i++) {
					file >> x >> y >> z >> r >> g >> b;
					this->vertices[3 * i] = x;
					this->vertices[3 * i + 1] = y;
					this->vertices[3 * i + 2] = z;
					this->vertices[3 * i + 3] = r;
					this->vertices[3 * i + 4] = g;
					this->vertices[3 * i + 5] = b;
					//cout << " " << x << " " << y << " " << z << " " << r << " " << g << " " << b << endl;
				}
				for (int i = 0; i < this->numIndices; i++) {
					file >> temp >> x >> y >> z;
					this->indices[3 * i] = x;
					this->indices[3 * i + 1] = y;
					this->indices[3 * i + 2] = z;
					//cout << temp << " " << to_string(x) << " " << to_string(y) << " " << to_string(z) << endl;
				}
			}

			else if (strTipo != "textu" and strTipo != "color") {
				this->tipoArchivo = TipoArchivo::Normal;
				this->verticesInFila = 3;
				float x, y, z;

				this->vertices = new float[this->numVertices * 3];
				this->indices = new GLuint[this->numIndices * 3];

				for (int i = 0; i < this->numVertices; i++) {
					file >> x >> y >> z;
					this->vertices[3 * i] = x;
					this->vertices[3 * i + 1] = y;
					this->vertices[3 * i + 2] = z;
					//cout << " " << x << " " << y << " " << z << endl;
				}
				for (int i = 0; i < this->numIndices; i++) {
					file >> temp >> x >> y >> z;
					this->indices[3 * i] = x;
					this->indices[3 * i + 1] = y;
					this->indices[3 * i + 2] = z;
					//cout << temp << " " << to_string(x) << " " << to_string(y) << " " << to_string(z) << endl;
				}
			}

			else if (strTipo == "textu") {
				this->verticesInFila = 5;
				this->tipoArchivo = TipoArchivo::Textura;
				float x, y, z, u, v;

				this->vertices = new float[this->numVertices * 5];
				this->indices = NULL;

				for (int i = 0; i < this->numVertices; i++) {
					file >> x >> y >> z >> u >> v;
					this->vertices[3 * i] = x;
					this->vertices[3 * i + 1] = y;
					this->vertices[3 * i + 2] = z;
					this->vertices[3 * i + 3] = u;
					this->vertices[3 * i + 4] = v;
					//cout << " " << x << " " << y << " " << z << " " << u << " " << v << endl;
				}
			}
		}else {
			cout << "ERROR READING FILE" << endl;
		}

		file.close();
	}
		

};


