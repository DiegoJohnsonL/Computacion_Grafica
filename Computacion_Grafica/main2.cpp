#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GLFW/glfw3native.h>
#include "Camera.h"

using namespace std;



const unsigned int ANCHO = 800;
const unsigned int ALTO = 600;

class CProgramaShaders {
	GLuint idPrograma;
public:
	CProgramaShaders(string rutaShaderVertice, string rutaShaderFragmento) {
		//Variables para leer los archivos de código
		string strCodigoShaderVertice;
		string strCodigoShaderFragmento;
		ifstream pArchivoShaderVertice;
		ifstream pArchivoShaderFragmento;
		//Mostramos excepciones en caso haya
		pArchivoShaderVertice.exceptions(ifstream::failbit | ifstream::badbit);
		pArchivoShaderFragmento.exceptions(ifstream::failbit | ifstream::badbit);
		try {
			//Abriendo los archivos de código de los shader
			pArchivoShaderVertice.open(rutaShaderVertice);
			pArchivoShaderFragmento.open(rutaShaderFragmento);
			//Leyendo la información de los archivos
			stringstream lectorShaderVertice, lectorShaderFragmento;
			lectorShaderVertice << pArchivoShaderVertice.rdbuf();
			lectorShaderFragmento << pArchivoShaderFragmento.rdbuf();
			//Cerrando los archivos
			pArchivoShaderVertice.close();
			pArchivoShaderFragmento.close();
			//Pasando la información leida a string
			strCodigoShaderVertice = lectorShaderVertice.str();
			strCodigoShaderFragmento = lectorShaderFragmento.str();
		}
		catch (ifstream::failure) {
			cout << "ERROR: Los archivos no pudieron ser leidos correctamente.\n";
		}
		const char* codigoShaderVertice = strCodigoShaderVertice.c_str();
		const char* codigoShaderFragmento = strCodigoShaderFragmento.c_str();
		//Asociando y compilando los códigos de los shader
		GLuint idShaderVertice, idShaderFragmento;
		//Shader de Vértice
		idShaderVertice = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(idShaderVertice, 1, &codigoShaderVertice, NULL);
		glCompileShader(idShaderVertice);
		verificarErrores(idShaderVertice, "Vértice");
		//Shader de Fragmento
		idShaderFragmento = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(idShaderFragmento, 1, &codigoShaderFragmento, NULL);
		glCompileShader(idShaderFragmento);
		verificarErrores(idShaderVertice, "Fragmento");
		//Programa de Shaders
		this->idPrograma = glCreateProgram();
		glAttachShader(this->idPrograma, idShaderVertice);
		glAttachShader(this->idPrograma, idShaderFragmento);
		glLinkProgram(this->idPrograma);
		//Ahora ya podemos eliminar los programas de los shaders
		glDeleteShader(idShaderVertice);
		glDeleteShader(idShaderFragmento);
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
	void setVec3(const string& nombre, const glm::vec3& valor) const {
		glUniform3fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, &valor[0]);
	}
	void setMat4(const string& nombre, const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setTTexture(const GLchar* valor) {
		glUniform1i(glGetUniformLocation(this->idPrograma, valor), 0);
	}
	void setInt(const string& nombre, int valor) const {
		glUniform1i(glGetUniformLocation(idPrograma, nombre.c_str()), valor);
	}
	void setFloat(const string& nombre, float valor) const {
		glUniform1f(glGetUniformLocation(idPrograma, nombre.c_str()), valor);
	}
private:
	void verificarErrores(GLuint identificador, string tipo) {
		GLint ok;
		GLchar log[1024];

		if (tipo == "Programa") {
			glGetProgramiv(this->idPrograma, GL_LINK_STATUS, &ok);
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

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = ANCHO / 2.0f;
float lastY = ALTO / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(0.7f, 0.5f, 3.0f);



void framebuffer_tamanho_callback(GLFWwindow* ventana, int ancho, int alto) {
	glViewport(0, 0, ancho, alto);
}
void procesarEntrada(GLFWwindow* ventana) {
	if (glfwGetKey(ventana, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(ventana, true);
	if (glfwGetKey(ventana, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(ventana, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(ventana, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(ventana, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


int main() {
	//Inicializar glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creando la ventana
	GLFWwindow* ventana = glfwCreateWindow(ANCHO, ALTO, "Compu Grafica", NULL, NULL);
	if (ventana == NULL) {
		cout << "Problemas al crear la ventana\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(ventana);
	glfwSetFramebufferSizeCallback(ventana, framebuffer_tamanho_callback);
	glfwSetCursorPosCallback(ventana, mouse_callback);
	glfwSetScrollCallback(ventana, scroll_callback);

	glfwSetInputMode(ventana, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Cargar Glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Problemas al cargar GLAD\n";
		return -1;
	}

	CProgramaShaders programa_shaders = CProgramaShaders("GLSL/codigoilum.vs", "GLSL/codigoilum.fs");
	CProgramaShaders programa_light = CProgramaShaders("GLSL/codigoLight.vs", "GLSL/codigoLight.fs");
	//CProgramaShaders programa_lighting = CProgramaShaders("GLSL/colors.vs", "GLSL/colors.fs");
	//Definiendo la geometría de la figura en función de vértices
	/*
	float vertices[] = {
		0.0,  0.0, 1.0, 1.0, 0.0, 0.0,  0.0, 0.0,
		0.4,  0.7, 1.0, 0.0, 1.0, 0.0,	1.0, 1.0,
		0.6,  0.0, 0.0, 1.0, 1.0, 0.0,	1.0, 1.0,
		0.4,  -0.7, 0.0, 0.0, 0.0, 1.0,	1.0, 0.0,
		-0.4, -0.7,1.0, 1.0, 0.0, 1.0,	0.0, 0.0,
		-0.6, 0.0, 1.0, 0.0, 1.0, 0.0,	0.0, 1.0,
		-0.4, 0.7, 0.0, 1.0, 1.0, 0.0,	0.0, 1.0
	};
	unsigned int indices[] = {
		0,1,2,
		2,3,0,
		3,4,0,
		4,5,0,
		5,6,0,
		6,1,0
	};

	float vertices[] = {

		 -0.5 , -0.5 , -0.5, 0.000059f, 0.999996,
		  0.5 , -0.5 , -0.5, 0.667969f, 0.328111,
		  0.5 ,  0.5 , -0.5, 0.000004f, 0.32813,
		 -0.5 ,  0.5 , -0.5, 0.336098f, 0.999929,
		 -0.5 , -0.5,  0.5,  0.000103f, 0.663952,
		  0.5 , -0.5,  0.5,  0.336098f, 0.999929,
		  0.5 ,  0.5,  0.5,  0.000103f, 0.663952,
		 -0.5 ,  0.5,  0.5,  0.335973f, 0.664097

	};
	unsigned int indices[] = {
		 0 ,1 ,2,
		 2 ,3 ,0,
		 4 ,5 ,6,
		 6 ,7 ,4,
		 7 ,3 ,0,
		 0 ,4 ,7,
		 6 ,2 ,1,
		 1 ,5 ,6,
		 0 ,1 ,5,
		 5 ,4 ,0,
		 3 ,2 ,6,
		 6 ,7 ,3
	};
	*/
	/*
	float vertices[] = {
		 -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};*/

	float vertices[] = {
		-0.5 ,-0.5, -0.5,  0.0,  0.0, -1.0,  0.0,  0.0,
		 0.5 ,-0.5, -0.5,  0.0,  0.0, -1.0,  1.0,  0.0,
		 0.5 , 0.5, -0.5,  0.0,  0.0, -1.0,  1.0,  1.0,
		 0.5 , 0.5, -0.5,  0.0,  0.0, -1.0,  1.0,  1.0,
		-0.5 , 0.5, -0.5,  0.0,  0.0, -1.0,  0.0,  1.0,
		-0.5 ,-0.5, -0.5,  0.0,  0.0, -1.0,  0.0,  0.0,
		-0.5 ,-0.5,  0.5,  0.0,  0.0,  1.0,  0.0,  0.0,
		 0.5 ,-0.5,  0.5,  0.0,  0.0,  1.0,  1.0,  0.0,
		 0.5 , 0.5,  0.5,  0.0,  0.0,  1.0,  1.0,  1.0,
		 0.5 , 0.5,  0.5,  0.0,  0.0,  1.0,  1.0,  1.0,
		-0.5 , 0.5,  0.5,  0.0,  0.0,  1.0,  0.0,  1.0,
		-0.5 ,-0.5,  0.5,  0.0,  0.0,  1.0,  0.0,  0.0,
		-0.5 , 0.5,  0.5, -1.0,  0.0,  0.0,  1.0,  0.0,
		-0.5 , 0.5, -0.5, -1.0,  0.0,  0.0,  1.0,  1.0,
		-0.5 ,-0.5, -0.5, -1.0,  0.0,  0.0,  0.0,  1.0,
		-0.5 ,-0.5, -0.5, -1.0,  0.0,  0.0,  0.0,  1.0,
		-0.5 ,-0.5,  0.5, -1.0,  0.0,  0.0,  0.0,  0.0,
		-0.5 , 0.5,  0.5, -1.0,  0.0,  0.0,  1.0,  0.0,
		 0.5 , 0.5,  0.5,  1.0,  0.0,  0.0,  1.0,  0.0,
		 0.5 , 0.5, -0.5,  1.0,  0.0,  0.0,  1.0,  1.0,
		 0.5 ,-0.5, -0.5,  1.0,  0.0,  0.0,  0.0,  1.0,
		 0.5 ,-0.5, -0.5,  1.0,  0.0,  0.0,  0.0,  1.0,
		 0.5 ,-0.5,  0.5,  1.0,  0.0,  0.0,  0.0,  0.0,
		 0.5 , 0.5,  0.5,  1.0,  0.0,  0.0,  1.0,  0.0,
		-0.5 ,-0.5, -0.5,  0.0, -1.0,  0.0,  0.0,  1.0,
		 0.5 ,-0.5, -0.5,  0.0, -1.0,  0.0,  1.0,  1.0,
		 0.5 ,-0.5,  0.5,  0.0, -1.0,  0.0,  1.0,  0.0,
		 0.5 ,-0.5,  0.5,  0.0, -1.0,  0.0,  1.0,  0.0,
		-0.5 ,-0.5,  0.5,  0.0, -1.0,  0.0,  0.0,  0.0,
		-0.5 ,-0.5, -0.5,  0.0, -1.0,  0.0,  0.0,  1.0,
		-0.5 , 0.5, -0.5,  0.0,  1.0,  0.0,  0.0,  1.0,
		 0.5 , 0.5, -0.5,  0.0,  1.0,  0.0,  1.0,  1.0,
		 0.5 , 0.5,  0.5,  0.0,  1.0,  0.0,  1.0,  0.0,
		 0.5 , 0.5,  0.5,  0.0,  1.0,  0.0,  1.0,  0.0,
		-0.5 , 0.5,  0.5,  0.0,  1.0,  0.0,  0.0,  0.0,
		-0.5 , 0.5, -0.5,  0.0,  1.0,  0.0,  0.0,  1.0
	};
	glm::vec3 cubePositions[] = {
		 glm::vec3(0.0f,  0.0f,  0.0f),
		 glm::vec3(2.0f,  5.0f, -15.0f),
		 glm::vec3(-1.5f, -2.2f, -2.5f),
		 glm::vec3(-3.8f, -2.0f, -12.3f),
		 glm::vec3(2.4f, -0.4f, -3.5f),
		 glm::vec3(-1.7f,  3.0f, -7.5f),
		 glm::vec3(1.3f, -2.0f, -2.5f),
		 glm::vec3(1.5f,  2.0f, -2.5f),
		 glm::vec3(1.5f,  0.2f, -1.5f),
		 glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	/// FOR LIGHT

	float verticesLight[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLight), verticesLight, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	///////////////HASTA AQUI NOMAS


	unsigned int id_array_buffers, id_array_vertices, id_element_buffer;
	glGenVertexArrays(1, &id_array_vertices);
	glGenBuffers(1, &id_array_buffers);
	glGenBuffers(1, &id_element_buffer);

	glBindVertexArray(id_array_vertices);

	glBindBuffer(GL_ARRAY_BUFFER, id_array_buffers);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_element_buffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);






	glEnable(GL_DEPTH_TEST);


	unsigned int diffuseMap;
	diffuseMap = loadTexture("container2.png");
	unsigned int specularMap;
	specularMap = loadTexture("container2_specular.png");





	programa_shaders.usar();
	programa_shaders.setInt("material.diffuse", 0);
	programa_shaders.setInt("material.specular", 1);

	while (!glfwWindowShouldClose(ventana)) {

		procesarEntrada(ventana);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		programa_shaders.usar();
		//programa_shaders.setInt("texture", 0);
		//programa_shaders.setInt("specularMap ", 1);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;



		//update uniforms


		programa_shaders.setVec3("light.position", camera.Position);
		programa_shaders.setVec3("light.direction", camera.Front);
		programa_shaders.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
		//programa_shaders.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
		programa_shaders.setVec3("viewPos", camera.Position);

		// light properties
		programa_shaders.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
		// we configure the diffuse intensity slightly higher; the right lighting conditions differ with each lighting method and environment.
		// each environment and lighting type requires some tweaking to get the best out of your environment.
		programa_shaders.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
		programa_shaders.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		programa_shaders.setFloat("light.constant", 1.0f);
		programa_shaders.setFloat("light.linear", 0.09f);
		programa_shaders.setFloat("light.quadratic", 0.032f);

		// material properties
		programa_shaders.setFloat("material.shininess", 32.0f);

		///INITIAL MODEL
		glm::mat4 vista = camera.GetViewMatrix();
		programa_shaders.setMat4("vista", vista);

		glm::mat4 proyeccion;
		proyeccion = glm::perspective(glm::radians(camera.Zoom), (float)ANCHO / (float)ALTO, 0.1f, 100.0f);
		programa_shaders.setMat4("proyeccion", proyeccion);

		///FINISHED MODEL

		//glm::mat4 model = glm::mat4(1.0f);
		//programa_shaders.setMat4("model", model);
		/*glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		programa_shaders.setMat4("model", model);
		*/


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);



		for (unsigned int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			if (i % 3 != 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.
				angle = glfwGetTime() * 25.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			programa_shaders.setMat4("model", model);

			glBindVertexArray(id_array_vertices);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_element_buffer);
			glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(int));
		}
		//glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

		//FOR LIGHT

		//programa_lighting.usar();
		//programa_lighting.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		//programa_lighting.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)ANCHO / (float)ALTO, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		//programa_lighting.setMat4("projection", projection);
		//programa_lighting.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		/*model = glm::translate(model, lightPos2);
		programa_lighting.setMat4("model", model);

		// render the cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		*/
		programa_light.usar();


		programa_light.setMat4("projection", projection);
		programa_light.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		programa_light.setMat4("model", model);


		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//FINISHED
		glfwSwapBuffers(ventana);
		glfwPollEvents();



	}

	//Liberando memoria
	glDeleteVertexArrays(1, &id_array_vertices);
	glDeleteBuffers(1, &id_array_buffers);
	glfwTerminate();
	return 0;
}

