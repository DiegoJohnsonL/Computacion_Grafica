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
	CProgramaShaders(string rutaShaderVertice, string rutaShaderFragmento, string rutaShaderGeometria = "") {
		//Variables para leer los archivos
		string strCodigoShaderVertice;
		string strCodigoShaderFragmento;
		string strCodigoShaderGeometria;
		ifstream pArchivoShaderVertice;
		ifstream pArchivoShaderFragmento;
		ifstream pArchivoShaderGeometria;
		//Mostramos excepciones en caso haya
		pArchivoShaderVertice.exceptions(ifstream::failbit | ifstream::badbit);
		pArchivoShaderFragmento.exceptions(ifstream::failbit | ifstream::badbit);
		pArchivoShaderGeometria.exceptions(ifstream::failbit | ifstream::badbit);
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
			//Shader de Geometria
			if (rutaShaderGeometria != "") {
				pArchivoShaderGeometria.open(rutaShaderGeometria);
				stringstream lectorShaderGeometria;
				lectorShaderGeometria << pArchivoShaderGeometria.rdbuf();
				pArchivoShaderGeometria.close();
				strCodigoShaderGeometria = lectorShaderGeometria.str();
			}
		}
		catch (ifstream::failure) { cout << "ERROR: Los archivos no pudieron ser leidos correctamente.\n"; }
		const char* codigoShaderVertice = strCodigoShaderVertice.c_str();
		const char* codigoShaderFragmento = strCodigoShaderFragmento.c_str();

		//Creaando, asociando y compilando los codigos de los shader
		GLuint idShaderVertice, idShaderFragmento;

		//Shader de Vertice
		idShaderVertice = glad_glCreateShader(GL_VERTEX_SHADER);
		//linkear shader - Asociando al programa, de la tarjeta grafica, con el identificador "idShaderVertice" con el codigo en la direccion a apuntada por "codigo_vertex_shader"
		glShaderSource(idShaderVertice, 1, &codigoShaderVertice, NULL);
		glCompileShader(idShaderVertice); //copilar shader
		verificarErrores(idShaderVertice, "Vertice");

		//Shader de Fragmento
		idShaderFragmento = glad_glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(idShaderFragmento, 1, &codigoShaderFragmento, NULL);
		glCompileShader(idShaderFragmento); //Pedimos a la tajerta grafica que copile el programa con el identificadoe "idShaderFramento"
		verificarErrores(idShaderFragmento, "Fragmento");
		//Shader de Geometria
		GLuint idShaderGeometria;
		if (rutaShaderGeometria != "") {
			const char* codigoShaderGeometria = strCodigoShaderGeometria.c_str();
			idShaderGeometria = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(idShaderGeometria, 1, &codigoShaderGeometria, NULL);
			glCompileShader(idShaderGeometria);
			verificarErrores(idShaderGeometria, "Geometria");
		}
		//Programa de shaders -- Enlazando vertex y frame shaders
		idPrograma = glCreateProgram();
		glAttachShader(idPrograma, idShaderVertice);
		glAttachShader(idPrograma, idShaderFragmento);
		if (rutaShaderGeometria != "") glAttachShader(idPrograma, idShaderGeometria);
		glLinkProgram(idPrograma);
		verificarErrores(idShaderVertice, "Programa");
		//Ahora ya podemos eliminar los programas de los shaders
		glDeleteShader(idShaderVertice);
		glDeleteShader(idShaderFragmento);
		if (rutaShaderGeometria != "") glDeleteShader(idShaderGeometria);

	}
	~CProgramaShaders() {
		glDeleteProgram(this->idPrograma);
	}
	// Activa el shader
	void usar() const {
		glUseProgram(idPrograma);
	}
	void setBool(const string& nombre, bool valor) const {
		glUniform1i(glGetUniformLocation(idPrograma, nombre.c_str()), (int)valor);
	}
	void setInt(const string& nombre, int valor) const {
		glUniform1i(glGetUniformLocation(idPrograma, nombre.c_str()), valor);
	}
	void setFloat(const string& nombre, float valor) const {
		glUniform1f(glGetUniformLocation(idPrograma, nombre.c_str()), valor);
	}
	void setVec2(const std::string& nombre, float x, float y) const {
		glUniform2f(glGetUniformLocation(idPrograma, nombre.c_str()), x, y);
	}
	void setVec2(const std::string& nombre, const glm::vec2& valor) const {
		glUniform2fv(glGetUniformLocation(idPrograma, nombre.c_str()), 1, &valor[0]);
	}
	void setVec3(const string& nombre, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(idPrograma, nombre.c_str()), x, y, z);
	}
	void setVec3(const string& nombre, const glm::vec3& valor) const {
		glUniform3fv(glGetUniformLocation(idPrograma, nombre.c_str()), 1, &valor[0]);
	}
	void setVec4(const std::string& nombre, float x, float y, float z, float w) const {
		glUniform4f(glGetUniformLocation(idPrograma, nombre.c_str()), x, y, z, w);
	}
	void setVec4(const string& nombre, const glm::vec4& valor) const {
		glUniform4fv(glGetUniformLocation(idPrograma, nombre.c_str()), 1, &valor[0]);
	}
	void setMat2(const string& nombre, const glm::mat2& mat) const {
		glUniformMatrix2fv(glGetUniformLocation(idPrograma, nombre.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat3(const string& nombre, const glm::mat3& mat) const {
		glUniformMatrix3fv(glGetUniformLocation(idPrograma, nombre.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat4(const string& nombre, const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(idPrograma, nombre.c_str()), 1, GL_FALSE, &mat[0][0]);
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