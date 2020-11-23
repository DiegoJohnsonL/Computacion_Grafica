#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>'
#include <vector>
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"
#include "shader.h"

using namespace std;
using namespace glm;


enum class Tipo { basico, textura, color, colorTextura, normales, normalesTextura };

class Shape {

public:
	GLfloat* vertices;
	GLuint* indices;
	float xmin = 1e09, xmax = -1e09, ymin = 1e09, ymax = -1e09, zmin = 1e09, zmax = -1e09;

	// Movimiento del objeto
	float xPos = 0.0, yPos = 0.0, zPos = 0.0;
	float xVel = 0.0025, yVel = 0.0030, zVel = 0.0035;
	int sizeObject;

	// Indicador de atributos
	int numTexturas = 0;
	bool colorInterno = false;
	bool texturaInterna = false;
	bool normales = false;

	// Vertex data y bufers 
	GLuint VAO, VBO, EBO;

	// Texturas
	GLuint texture1, texture2;
	float mixValue = 0.2;

	// Data del OFF
	int linVertices, linIndices, linAristas;
	int verticesTotal, indicesTotal, flotantesTotal;

	// Vector de posiciones
	int posicionesTotal;
	glm::vec3* posiciones = nullptr;

	// Vector de colores de luz
	int vectorColoresTotal;
	glm::vec3* vectorColores = nullptr;

	Tipo tipo;
	string name, filePath;

	// Matrices de transformacion
	glm::mat4 vista = glm::mat4(1.0);
	glm::mat4 proyeccion = glm::mat4(1.0);
	glm::vec3 colores = glm::vec3(1.0);
	glm::mat4 modelo = glm::mat4(1.0);

	Shape(string filePath) {
		//Reads file
		this->filePath = filePath;
		this->readFile();
		//Add conditionals for other files formats
	}
	~Shape() {	}
	void freeMemory() {
		delete[] vertices;
		delete[] indices;
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	// Configurar vertex y fragment shaders adecuados segun el archivo OFF
	string vertexShader() {
		string VS = "GLSL/colorTextura.vs";
		if (tipo == Tipo::textura) VS = "GLSL/textura.vs";
		if (tipo == Tipo::normalesTextura) VS = "GLSL/normalesTextura.vs";
		return VS;
	}

	string fragmentShader() {
		string FS = "GLSL/colorTextura.fs";
		if (tipo == Tipo::textura) FS = "GLSL/textura.fs";
		if (tipo == Tipo::normalesTextura) FS = "GLSL/normalesTextura.fs";
		return FS;
	}

	void setBuffers() {
		this->defBuffers();
		this->indexAndLoadBuffers();
		this->setAttributes();
	}

	void draw(CProgramaShaders& shader) {
		//* Enlazar texturas segun cuantas hayan sido cargadas
		if (numTexturas > 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);
		}

		if (numTexturas > 1) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture2);
		} //*/

		// Establecer valores condicionales de los Shaders
		shader.setFloat("mixValue", mixValue); // Valor de mezcla de ambas texturas
		shader.setBool("colorInterno", colorInterno); // Si el modelo tiene color en sus vertices
		shader.setBool("texturaInterna", texturaInterna); // Si el modelo tiene textura en sus vertices

		// Dibujar la superficie segun los vertices o indices
		glBindVertexArray(VAO);
		if (indicesTotal > 0) {
			// Dibujar utilizando el arreglo de indices
			glDrawElements(GL_TRIANGLES, indicesTotal, GL_UNSIGNED_INT, 0);
		}
		else {
			// Dibujar utilizando solo el arreglo de vertices
			glDrawArrays(GL_TRIANGLES, 0, linVertices);
		}
		glBindVertexArray(0);
	}

	// Enlazar texturas segun cuantas hayan sido cargadas
	void bindTexture() {
		if (numTexturas > 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);
		}

		if (numTexturas > 1) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture2);
		}
	}

	// Cargar textura de ruta a ID de textura
	void loadTexture(string path, unsigned int& textureID) {

		glGenTextures(1, &textureID);
		int width, height, nrComponents;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
		if (data) {

			GLenum format;
			if (nrComponents == 1) format = GL_RED;
			else if (nrComponents == 3) format = GL_RGB;
			else if (nrComponents == 4) format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(data);
		}
		else {
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}
	}

	// Cargar texturas segun las rutas que se especifican
	void loadTextures(string ruta1 = "", string ruta2 = "") {
		// Textura 1
		if (numTexturas > 0) {
			stbi_set_flip_vertically_on_load(true);
			loadTexture(ruta1, texture1);
		}
		if (ruta2 != "") {
			// Textura 2
			numTexturas = 2;
			loadTexture(ruta2, texture2);
		}
	}

	// Establecer valores condicionales de los Shaders
	void setShaderConditionals(CProgramaShaders& shader) {
		shader.setFloat("mixValue", mixValue); // Valor de mezcla de ambas texturas
		shader.setBool("colorInterno", colorInterno); // Si el modelo tiene color en sus vertices
		shader.setBool("texturaInterna", texturaInterna); // Si el modelo tiene textura en sus vertices
	}

	// Configurar el shader con las texturas cargadas
	void shaderConfiguration(CProgramaShaders& shader) {
		shader.usar();
		if (normales) {
			shader.setInt("material.diffuse", 0);
			shader.setInt("material.specular", 1);
			return;
		}
		shader.setInt("texture1", 0);
		shader.setInt("texture2", 1);
	}

	// Movimiento del objeto
	void movement() {
		xPos += xVel;
		yPos += yVel;
		zPos += zVel;
	}

	// Traslacion con rebote en los Limites
	void sandboxMovement(float multiplier, int limit = 1) {
		if (xPos > (limit - sizeObject) + abs(sizeObject * multiplier) || xPos < (sizeObject - limit) - abs(sizeObject * multiplier)) xVel *= -1;
		if (yPos > (limit - sizeObject) + abs(sizeObject * multiplier) || yPos < (sizeObject - limit) - abs(sizeObject * multiplier)) yVel *= -1;
		if (zPos > (limit - sizeObject) + abs(sizeObject * multiplier) || zPos < (sizeObject - limit) - abs(sizeObject * multiplier)) zVel *= -1;
	}
	template <typename T, typename T2, size_t n>
	void setArray(T(&theArray)[n], T2*& pointerArray, int& size) {
		// Eliminar el arreglo de punteros por si ya existe
		delete[] pointerArray;
		// Guardar el valor del tamaño del arreglo
		size = sizeof(theArray) / sizeof(T);
		// Formatear el arreglo de punteros con el tamaño del arreglo
		pointerArray = new T2[size];
		// Copiar todos los elementos del arreglo al arreglo de punteros
		for (int i = 0; i < size; i++) pointerArray[i] = (T2)theArray[i];
	}

	// Copiar arreglo exterior al arreglo de punteros interior segun el nombre del arreglo
	template <typename T, size_t n>
	void loadArray(string name, T(&theArray)[n]) {
		//if (name == "vertices") setArray(theArray, vertices, verticesTotal);
		//if (name == "indices") setArray(theArray, indices, indicesTotal);
		if (name == "posiciones") setArray(theArray, posiciones, posicionesTotal);
		if (name == "colores") setArray(theArray, vectorColores, vectorColoresTotal);
	}

private:

	void defBuffers() {
		//Enviando la geometria al gpu: Definiendo los buffers (Vertex Array Objects y Aertex Buffer Objets)
		//VAO  ABO
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);
	}

	void indexAndLoadBuffers() {

		//Anexando los buffers para su ejecucion en la targeta grafica
		glBindVertexArray(this->VAO);

		//Anexando buffers y cargando los buffer con los datos	
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, verticesTotal * sizeof(GLfloat), vertices, GL_STATIC_DRAW); //reservando en memoria de el gpu, || GL_Dynamic_DRAW || GL_Stream_DRAW 

		if (indicesTotal > 0) {
			// Indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesTotal * sizeof(GLuint), indices, GL_STATIC_DRAW);
		}
	}

	void setAttributes() {
		//(Atributo location = n del vertex shader, cantidad de datos del vec en locations 0, flotantes , false,  moverte n atributos en el arreglo de vertices, empezar en la posicion n despues del puntero)
		switch (this->tipo)
		{

		case Tipo::basico:
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(0);
			break;

		case Tipo::textura:
			numTexturas = 1;
			texturaInterna = true;
			// Atributo de posicion
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(0);
			// Atributo de textura
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			break;

		case Tipo::color:
			colorInterno = true;
			// Atributo de posicion
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(0);
			// Atributo de color
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			break;

		case Tipo::colorTextura:
			numTexturas = 1;
			texturaInterna = true;
			colorInterno = true;
			// Atributo de posicion
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(0);
			// Atributo de color
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
			// Atributo de textura
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);

			break;

		case Tipo::normalesTextura:
			numTexturas = 1;
			texturaInterna = true;

			// Atributo de posicion
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(0);
			// Atributo de normales
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
			// Atributo de textura
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);

			break;

		}
	}

	// Revisar los valores X, Y y Z maximos y minimos
	void checkMaxMin(int i) {
		if (i == 0 || i % flotantesTotal == 0) {
			if (vertices[i] < xmin) xmin = vertices[i];
			if (vertices[i] > xmax) xmax = vertices[i];
		}
		if (i == 1 || i % flotantesTotal == 1) {
			if (vertices[i] < ymin) ymin = vertices[i];
			if (vertices[i] > ymax) ymax = vertices[i];
		}
		if (i == 2 || i % flotantesTotal == 2) {
			if (vertices[i] < zmin) zmin = vertices[i];
			if (vertices[i] > zmax) zmax = vertices[i];
		}
	}
	// Obtener los maximos y minimos si es que no se ha usado un archivo OFF
	void getMaxMin() {
		for (int i = 0; i < verticesTotal; i++) checkMaxMin(i);
	}

	// Arreglar para vertices e indices, funciona sin problemas para las posiciones
	// En caso de no tener un archivo OFF, cargar los arreglos manualmente
	// figura.setArrayName("vertices", vertices);
	// figura.setArrayName("indices", indices);
	// figura.getMaxMin() // Para obtener los maximos y minimos
	//------------------------------------------------------------------------

	void readFile() {
		ifstream file;
		string line;
		float temp;
		file.open(this->filePath);

		if (file.is_open()) {
			file >> name; // OFF	
			file >> linVertices >> linIndices >> linAristas;

			for (int i = 0; i < linVertices; i++) {
				// Se ejecuta en la primera linea
				if (i == 0) {
					file.ignore(); // dummy line
					getline(file, line); // Se lee la primera linea de los vertices
					stringstream ss(line);
					vector<float> auxVec;
					while (ss >> temp) {
						auxVec.push_back(temp); // Se añaden los flotantes del string Line al vector auxVec
					}

					// Se encuentran los flotantes en total de la primera linea
					flotantesTotal = auxVec.size();
					// Se encuentra el numero de vertices en total multiplicando el numero de lineas de vertices por los flotantes en total
					verticesTotal = linVertices * flotantesTotal;
					// Se crea el arreglo con el numero total de flotantes vertices
					vertices = new GLfloat[verticesTotal];

					// Se añaden todos los flotantes del vector auxiliar al arreglo de vertices
					for (int j = 0; j < auxVec.size(); j++) {
						vertices[j] = auxVec[j];

						// Revisa los maximos valores de x y z
						checkMaxMin(j);

						// Revisa si hay normales
						if (!normales && j > 3 && j <= 6 && vertices[j] < 0) normales = true;
					}

					// Se define el tipo de modelo
					if (flotantesTotal == 3) tipo = Tipo::basico;
					if (flotantesTotal == 5) tipo = Tipo::textura;
					if (flotantesTotal == 6) tipo = Tipo::color;
					if (flotantesTotal == 8) tipo = Tipo::colorTextura;

				}

				// Se lee el resto de lineas de acuerdo al numero de flotantes encontrados
				else {
					for (int j = flotantesTotal * i; j < flotantesTotal * i + flotantesTotal; j++) {
						file >> temp;
						vertices[j] = temp;

						// Revisa los maximos valores de x y z
						checkMaxMin(j);

						// Revisa si hay normales
						if (!normales && j % flotantesTotal > 3 && j % flotantesTotal <= 6 && vertices[j] < 0) normales = true;
					}
				}
			}

			// Se revisa si hubo normales
			if (normales) tipo = Tipo::normales;
			if (normales && flotantesTotal == 8) tipo = Tipo::normalesTextura;

			// Se leen las lineas de los indices solo si hay
			if (linIndices > 0) {
				indicesTotal = linIndices * 3;
				indices = new GLuint[indicesTotal];

				for (int i = 0; i < linIndices; i++) {
					file >> temp;
					for (int j = 3 * i; j < 3 * i + 3; j++) {
						file >> temp;
						indices[j] = temp;
					}
				}
			}
		}

		else {
			cout << "ERROR READING FILE" << endl;
		}
		// Se cierra el archivo
		file.close();

		//string nombre = this->filePath;
		//string strTipo = nombre.substr(4, 5);

		////cout << "Nombre del Archivo: " << this->filePath << endl;
		///*cout << "Contiene atributos de: ";
		//if (strTipo == "color")
		//	cout << "Color" << endl;

		//else if (strTipo == "textu")
		//	cout << "textura" << endl;
		//else
		//	cout << "solo posicion" << endl;
		//	*/
		//if (strTipo == "color") {
		//	this->tipoArchivo = TipoArchivo::Color;
		//	this->verticesInFila = 6;
		//	float x, y, z, r, g, b;

		//	this->vertices = new float[this->numVertices * verticesInFila];
		//	this->indices = new GLuint[this->numIndices * 3];

		//	for (int i = 0; i < this->numVertices; i++) {
		//		file >> x >> y >> z >> r >> g >> b;
		//		this->vertices[6 * i] = x;
		//		this->vertices[6 * i + 1] = y;
		//		this->vertices[6 * i + 2] = z;
		//		this->vertices[6 * i + 3] = r;
		//		this->vertices[6 * i + 4] = g;
		//		this->vertices[6 * i + 5] = b;
		//		//cout << " " << x << " " << y << " " << z << " " << r << " " << g << " " << b << endl;
		//	}
		//	for (int i = 0; i < this->numIndices; i++) {
		//		file >> temp >> x >> y >> z;
		//		this->indices[3 * i] = x;
		//		this->indices[3 * i + 1] = y;
		//		this->indices[3 * i + 2] = z;
		//		//cout << temp << " " << to_string(x) << " " << to_string(y) << " " << to_string(z) << endl;
		//	}
		//}

		//else if (strTipo != "textu" and strTipo != "color") {
		//	this->tipoArchivo = TipoArchivo::Normal;
		//	this->verticesInFila = 3;
		//	float x, y, z;

		//	this->vertices = new float[this->numVertices * 3];
		//	this->indices = new GLuint[this->numIndices * 3];

		//	for (int i = 0; i < this->numVertices; i++) {
		//		file >> x >> y >> z;
		//		this->vertices[3 * i] = x;
		//		this->vertices[3 * i + 1] = y;
		//		this->vertices[3 * i + 2] = z;
		//		//cout << " " << x << " " << y << " " << z << endl;
		//	}
		//	for (int i = 0; i < this->numIndices; i++) {
		//		file >> temp >> x >> y >> z;
		//		this->indices[3 * i] = x;
		//		this->indices[3 * i + 1] = y;
		//		this->indices[3 * i + 2] = z;
		//		//cout << temp << " " << to_string(x) << " " << to_string(y) << " " << to_string(z) << endl;
		//	}
		//}

		//else if (strTipo == "textu") {
		//	this->verticesInFila = 5;
		//	this->tipoArchivo = TipoArchivo::Textura;
		//	float x, y, z, u, v;

		//	this->vertices = new float[this->numVertices * 5];
		//	this->indices = new GLuint[this->numIndices * 3];

		//	for (int i = 0; i < this->numVertices; i++) {
		//		file >> x >> y >> z >> u >> v;
		//		this->vertices[5 * i] = x;
		//		this->vertices[5 * i + 1] = y;
		//		this->vertices[5 * i + 2] = z;
		//		this->vertices[5 * i + 3] = u;
		//		this->vertices[5 * i + 4] = v;
		//		//cout << " " << x << " " << y << " " << z << " " << u << " " << v << endl;
		//	}
		//	for (int i = 0; i < this->numIndices; i++) {
		//		file >> temp >> x >> y >> z;
		//		this->indices[3 * i] = x;
		//		this->indices[3 * i + 1] = y;
		//		this->indices[3 * i + 2] = z;
		//	}
		//}


		//else if (strTipo == "verte") {
		//	// crear TipoArchivo::Vertices
		//	this->tipoArchivo = TipoArchivo::Vertices;
		//	this->verticesInFila = 3;
		//	float x, y, z;

		//	this->vertices = new float[this->numVertices * 3];

		//	for (int i = 0; i < this->numVertices; i++) {
		//		file >> x >> y >> z;
		//		this->vertices[3 * i] = x;
		//		this->vertices[3 * i + 1] = y;
		//		this->vertices[3 * i + 2] = z;
		//		//cout << " " << x << " " << y << " " << z << endl;
		//	}
		//}
	}

};
