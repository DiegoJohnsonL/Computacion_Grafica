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

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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

	// Arreglo de posiciones de las lamparas
	glm::vec3 posiciones2[] = {
	  glm::vec3(0.7f,  0.2f,  2.0f),
	  glm::vec3(2.3f, -3.3f, -4.0f),
	  glm::vec3(-4.0f,  2.0f, -12.0f),
	  glm::vec3(0.0f,  0.0f, -3.0f)
	};

	//Cubo
	CModel cubo("OFF/cuboNT.off"); // iniciando y cargando el off
	cubo.setArrayName("posiciones", posiciones); // cargar posiciones
	cubo.setBuffers(); // definiendo los atributos de la figura
	cubo.loadTextures("container2.png", "container2_specular.png"); // Cargar Texturas

	// Figura 2 - Lampara
	CModel light("OFF/cubo2.off");
	light.setArrayName("posiciones", posiciones2);
	light.setBuffers();

	// Iniciando programa Shader con el vs y fs de la figura
	CProgramaShaders cuboShader(cubo.vertexShader(), cubo.fragmentShader());

	// Iniciando programa Shader con el vs y fs de la luz
	CProgramaShaders lightShader(light.vertexShader(), light.fragmentShader());

	// Configuracion del Shader para las texturas de la figura
	cubo.shaderConfiguration(cuboShader);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(ventana)) {
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//---------------------

		procesarEntrada(ventana);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cubo.bindTexture();

		// USANDO PROGRAMA DEL CUBO
		cuboShader.usar();

		cuboShader.setVec3("viewPos", camera.Position);
		cuboShader.setFloat("material.shininess", 32.0f);

		/*
		   Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		   the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		   by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		   by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/
		// directional light
		cuboShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		cuboShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		cuboShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		cuboShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		cuboShader.setVec3("pointLights[0].position", posiciones2[0]);
		cuboShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		cuboShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		cuboShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		cuboShader.setFloat("pointLights[0].constant", 1.0f);
		cuboShader.setFloat("pointLights[0].linear", 0.09);
		cuboShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		cuboShader.setVec3("pointLights[1].position", posiciones2[1]);
		cuboShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		cuboShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		cuboShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		cuboShader.setFloat("pointLights[1].constant", 1.0f);
		cuboShader.setFloat("pointLights[1].linear", 0.09);
		cuboShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		cuboShader.setVec3("pointLights[2].position", posiciones2[2]);
		cuboShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		cuboShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		cuboShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		cuboShader.setFloat("pointLights[2].constant", 1.0f);
		cuboShader.setFloat("pointLights[2].linear", 0.09);
		cuboShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		cuboShader.setVec3("pointLights[3].position", posiciones2[3]);
		cuboShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		cuboShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		cuboShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		cuboShader.setFloat("pointLights[3].constant", 1.0f);
		cuboShader.setFloat("pointLights[3].linear", 0.09);
		cuboShader.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		cuboShader.setVec3("spotLight.position", camera.Position);
		cuboShader.setVec3("spotLight.direction", camera.Front);
		cuboShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		cuboShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		cuboShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		cuboShader.setFloat("spotLight.constant", 1.0f);
		cuboShader.setFloat("spotLight.linear", 0.09);
		cuboShader.setFloat("spotLight.quadratic", 0.032);
		cuboShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		cuboShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)ANCHO / (float)ALTO, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		cuboShader.setMat4("proyeccion", projection);
		cuboShader.setMat4("vista", view);
		for (unsigned int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			model = glm::translate(model, posiciones[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			cuboShader.setMat4("modelo", model);
			cubo.draw();
		}

		lightShader.usar();
		lightShader.setMat4("proyeccion", projection);
		lightShader.setMat4("vista", view);

		// we now draw as many light bulbs as we have point lights.
		for (unsigned int i = 0; i < 4; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, posiciones2[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lightShader.setMat4("modelo", model);
			light.draw();
		}
		
		// USANDO PROGRAMA DE LA FUENTE DE LUZ

		lightShader.usar();


		glfwSwapBuffers(ventana);
		glfwPollEvents();	
	}

	//librerando memoria
	cubo.freeMemory();
	cubo.~CModel();
	cuboShader.~CProgramaShaders();
	glfwTerminate();
	return 0;

}