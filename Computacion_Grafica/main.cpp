#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>;
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

using namespace std;
using namespace glm;

void framebuffer_size_callback(GLFWwindow* ventana, int alto, int ancho) {
	glViewport(0, 0, ancho, alto); //actualiza el view port dependiendo de la ventana
}
void procesarEntrada(GLFWwindow* ventana) {
	if (glfwGetKey(ventana, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(ventana, true);
}

const unsigned int ANCHO = 800;
const unsigned int ALTO = 600;

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
	void setVec3(const string& nombre, const vec3 &valor) const {
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

int main() {

	//Inicializar glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creando la ventana
	GLFWwindow* ventana = glfwCreateWindow(ANCHO, ALTO, "Compu Grafica", NULL , NULL);
	if (ventana == NULL) {
		cout << "Problemas al crear la ventana\n";
		glfwTerminate();
		return -1;
	}
	//else
	glfwMakeContextCurrent(ventana);
	//actualizacion del view port
	glfwSetFramebufferSizeCallback(ventana, framebuffer_size_callback);

	//Cargar Glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Problemas al cargar GLAD\n";
		return -1; 
	}
	
	CProgramaShaders programa_shaders = CProgramaShaders("GLSL/codigo.vs", "GLSL/codigo.fs");	


	//Definiendo la geometria de la figura en funciond de vertices 
	float vertices[] = {
		//x----y----z
		 0.0,  0.0, 0.0,//0
		-0.25, 0.5, 0.0,//1
		 0.25, 0.5, 0.0,//2
	     0.5,  0.0, 0.0,//3
		 0.25,-0.5, 0.0,//4
	    -0.25,-0.5, 0.0,//5
		-0.5,  0.0, 0.0//6
	};

	GLuint indices[] = {
		0, 2, 1,
		0, 3, 2,
		0, 4, 3,
		0, 5, 4,
		0, 6, 5,
		0, 1, 6
	};

	//Enviando la geometria al gpu: Definiendo los buffers (Vertex Array Objects y Vertex Buffer Objets)
	//VAO  BAO
	GLuint id_array_vertices, id_array_buffers, id_element_buffer;
	glGenVertexArrays(1, &id_array_vertices);
	glGenBuffers(1, &id_array_buffers);
	glGenBuffers(1, &id_element_buffer);

	//Anexando los buffers para su ejecucion en la targeta grafica
	glBindVertexArray(id_array_vertices); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_element_buffer);

	//Anexando buffers y cargando los buffer con los datos
	glBindBuffer(GL_ARRAY_BUFFER, id_array_buffers);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //9vertices*sizeof(float), reservando en memoria de el gpuGL_Dynamic_DRAW , GL_Stream_DRAW 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//INDICANDO LAS ESPECIFICACIONES DE LOS ATRIBUTOS

	//(Atributo location = n del vertex shader, cantidad de datos del vec en locations 0, flotantes , false,  moverte n atributos en el arreglo de vertices, empezar en la posicion n despues del puntero)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);	


	float x = 0.0; float y = 0.0; float dx = 0.003; float dy = 0.003;
	float red; float green; float blue;
	while (!glfwWindowShouldClose(ventana))	{

		red = -sin((float)glfwGetTime()); green = sin((float)glfwGetTime()); blue = -sin((float)glfwGetTime());
		procesarEntrada(ventana); 
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		programa_shaders.usar();
		//creando una matriz homografica
		mat4 transformacion = mat4(1.0);

		transformacion = translate(transformacion, vec3(x, y, 0.0));
		transformacion = scale(transformacion, vec3(sin((float)glfwGetTime()), sin((float)glfwGetTime()), 1.0));
		transformacion = rotate(transformacion, (float)glfwGetTime(), vec3(0, 0, 1));
		programa_shaders.setMat4("transformacion", transformacion);
		programa_shaders.setVec3("colors", vec3(red, green + 0.5, blue));
		glBindVertexArray(id_array_vertices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_element_buffer);
		//glDrawArrays(GL_TRIANGLES, 0, 6);   
		//Dibuja con los elementos  marcados por los indices
		glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0); //size of indexs
		glfwSwapBuffers(ventana);
		glfwPollEvents();

		if (x >= 0.8 || x <= -0.75) { dx *= -1; }
		if (y >= 0.8 || y <= -0.75) { dy *= -1; }
		x += dx;
		y += dy;
	}

	//librerando memoria
	glDeleteVertexArrays(1, &id_array_vertices);
	glDeleteBuffers(1, &id_array_buffers);
	programa_shaders.~CProgramaShaders();

	glfwTerminate();
	return 0;	
}