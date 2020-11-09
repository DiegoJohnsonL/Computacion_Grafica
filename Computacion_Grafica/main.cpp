#include "modelo.h"
#include "Camera.h" 
using namespace std;
using namespace glm;

const unsigned int ANCHO = 800;
const unsigned int ALTO = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = ANCHO / 2.0f;
float lastY = ALTO / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f;


void framebuffer_size_callback(GLFWwindow* ventana, int alto, int ancho) {
	glViewport(0, 0, ancho, alto); //actualiza el view port dependiendo de la ventana
}

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


int main() {
	// camera
	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = ANCHO / 2.0f;
	float lastY = ALTO / 2.0f;
	bool firstMouse = true;

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f;

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
	//else
	glfwMakeContextCurrent(ventana);
	//actualizacion del view port
	glfwSetFramebufferSizeCallback(ventana, framebuffer_size_callback);
	glfwSetCursorPosCallback(ventana, mouse_callback);
	glfwSetScrollCallback(ventana, scroll_callback);

	glfwSetInputMode(ventana, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Cargar Glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Problemas al cargar GLAD\n";
		return -1;
	}

	// Arreglo de posiciones de los cubos
	glm::vec3 posiciones[] = {
		glm::vec3(0.0,  0.0,  0.0),
		glm::vec3(2.0,  5.0, -15.0),
		glm::vec3(-1.5, -2.2, -2.5),
		glm::vec3(-3.8, -2.0, -12.3),
		glm::vec3(2.4, -0.4, -3.5),
		glm::vec3(-1.7,  3.0, -7.5),
		glm::vec3(1.3, -2.0, -2.5),
		glm::vec3(1.5,  2.0, -2.5),
		glm::vec3(1.5,  0.2, -1.5),
		glm::vec3(-1.3,  1.0, -1.5)
	};

	// Figura 1
	CModel modelo("OFF/cuboNT.off"); // iniciando y cargando el off
	modelo.setArrayName("posiciones", posiciones); // cargar posiciones
	modelo.setBuffers(); // definiendo los atributos de la figura
	modelo.loadTextures("container2.png", "container2_specular.png"); // Cargar Texturas

	// Iniciando programa Shader con el vs y fs de la figura
	CProgramaShaders programa(modelo.vertexShader(), modelo.fragmentShader());

	// Configuracion del Shader para las texturas de la figura
	modelo.shaderConfiguration(programa);

	while (!glfwWindowShouldClose(ventana)) {


	}

	//librerando memoria
	modelo.~CModel();
	programa.~CProgramaShaders();
	glfwTerminate();
	return 0;

}