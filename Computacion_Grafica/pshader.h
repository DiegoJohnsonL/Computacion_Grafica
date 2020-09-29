#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

using namespace std;
using namespace glm;

class CProgramaShaders {
	GLuint idPrograma;
public:
	CProgramaShaders(string rutaShaderVertice, string rutaShaderFragmento) {
		//Variables para leer los archivos
		string strCodigoShaderVertice;
		string strCodigoShaderFragmento;
		ifstream pArchivoShaderVertice;
		ifstream pArchivoShaderFragmento;
		//Mostramos excepciones en caso haya
		pArchivoShaderVertice.exceptions(ifstream::failbit | ifstream::badbit);
		pArchivoShaderFragmento.exceptions(ifstream::failbit | ifstream::badbit);
		try {
			//Abriendo los archivos de codigo  de los shader
			pArchivoShaderVertice.open(rutaShaderVertice);
			pArchivoShaderFragmento.open(rutaShaderFragmento);
			//leyendo la informacion de los archivos 
			stringstream lectorShaderVertice, lectorShaderFragmento;
			lectorShaderVertice << pArchivoShaderVertice.rdbuf();
			lectorShaderFragmento << pArchivoShaderFragmento.rdbuf();
			//Cerrando los archivos
			pArchivoShaderVertice.close();
			pArchivoShaderFragmento.close();
			//Pasando la informacion leida a string
			strCodigoShaderVertice = lectorShaderVertice.str();
			strCodigoShaderFragmento = lectorShaderFragmento.str();
		}
		catch (ifstream::failure) { cout << "ERROR: Los archivos no pudieron ser leidos correctamente.\n"; }
		const char* codigoShaderVertice = strCodigoShaderVertice.c_str();
		const char* codigoShaderFragmento = strCodigoShaderFragmento.c_str();

		//Creaando, asociando y compilando los codigos de los shader
		GLuint idShaderVertice, idShaderFramento;

		//Shader de Vertice
		idShaderVertice = glad_glCreateShader(GL_VERTEX_SHADER);
		//linkear shader - Asociando al programa, de la tarjeta grafica, con el identificador "idShaderVertice" con el codigo en la direccion a apuntada por "codigo_vertex_shader"
		glShaderSource(idShaderVertice, 1, &codigoShaderVertice, NULL);
		glCompileShader(idShaderVertice); //copilar shader
		verificarErrores(idShaderVertice, "Vertice");

		//Shader de Fragmento
		idShaderFramento = glad_glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(idShaderFramento, 1, &codigoShaderFragmento, NULL);
		glCompileShader(idShaderFramento); //Pedimos a la tajerta grafica que copile el programa con el identificadoe "idShaderFramento"
		verificarErrores(idShaderFramento, "Fragmento");

		//Programa de shaders -- Enlazando vertex y frame shaders
		this->idPrograma = glCreateProgram();
		glAttachShader(this->idPrograma, idShaderVertice);
		glAttachShader(this->idPrograma, idShaderFramento);
		glLinkProgram(this->idPrograma);
		verificarErrores(this->idPrograma, "Programa");
		//ahora ya podemos eliminar los programasa de los shaaders
		glDeleteShader(idShaderVertice);
		glDeleteShader(idShaderFramento);

	}
	~CProgramaShaders() {
		glDeleteProgram(this->idPrograma);
	}
	void usar() const {
		glUseProgram(this->idPrograma);
	}
	void setVec3(const string& nombre, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(this->idPrograma, nombre.c_str()), x, y, z);
	}
	void setVec3(const string& nombre, const vec3& valor) const {
		glUniform3fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, &valor[0]);
	}
	void setMat4(const string& nombre, const mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, GL_FALSE, &mat[0][0]);
	}


private:
	//get status
	void verificarErrores(GLuint identificador, string tipo) {
		GLint ok;
		GLchar log[1024];

		if (tipo == "Programa") {
			glGetProgramiv(identificador, GL_LINK_STATUS, &ok);
			if (!ok) {
				glGetProgramInfoLog(this->idPrograma, 1024, NULL, log);
				cout << "Error de enlace con el programa: " << log << "\n";
			}
		}
		else {
			glGetShaderiv(identificador, GL_COMPILE_STATUS, &ok);
			if (!ok) {
				glGetShaderInfoLog(identificador, 1024, nullptr, log);
				cout << "Error de compilación con el Shader de " << tipo << ": " << log << "\n";
			}
		}
	}

};