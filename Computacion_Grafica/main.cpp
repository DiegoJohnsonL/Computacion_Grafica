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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
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
	//else
	glfwMakeContextCurrent(ventana);
	//actualizacion del view port
	glfwSetFramebufferSizeCallback(ventana, framebuffer_size_callback);

	glfwSetCursorPosCallback(ventana, mouse_callback);
	glfwSetScrollCallback(ventana, scroll_callback);

	// tell GLFW to capture our mouse
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
	CModel cubo("OFF/cuboT.off"); // iniciando y cargando el off
	cubo.setArrayName("posiciones", posiciones); // cargar posiciones
	cubo.setBuffers(); // definiendo los atributos de la figura
	cubo.loadTextures("container2.png", "container2_specular.png"); // Cargar Texturas

	// Iniciando programa Shader con el vs y fs de la figura
	CProgramaShaders modelProgram(cubo.vertexShader(), cubo.fragmentShader());

	// Configuracion del Shader para las texturas de la figura
	cubo.shaderConfiguration(modelProgram);
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(ventana)) {
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//---------------------
		procesarEntrada(ventana);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cubo.bindTexture();
		modelProgram.usar();

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)ANCHO / (float)ALTO, 0.1f, 100.0f);
		modelProgram.setMat4("proyeccion", projection);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		modelProgram.setMat4("vista", view);

		vec3 colores = glm::vec3(1.0);	
		modelProgram.setVec3("colores", colores);

		for (unsigned int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			model = glm::translate(model, posiciones[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			modelProgram.setMat4("modelo", model);
			cubo.draw();
		}
		
		glfwSwapBuffers(ventana);
		glfwPollEvents();	
	}

	//librerando memoria
	cubo.freeMemory();
	cubo.~CModel();
	modelProgram.~CProgramaShaders();
	glfwTerminate();
	return 0;

}