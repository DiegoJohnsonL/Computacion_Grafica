#pragma once
#include "Camera.h" 
#include "model.h"
#include <iostream>

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
	stbi_set_flip_vertically_on_load(true);

	// Arreglo de posiciones de los cubos
	glm::vec3 posiciones[] = {
		glm::vec3(1.0,  0.1, -1.0),
	};

	// Arreglo de posiciones de las lamparas
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  4.0f,  2.0f),
	};

	glm::vec3 pointLightColors[] = {
		glm::vec3(0.9f, 0.1f, 0.9f),
	};

	//Cubo
	Shape cubo("OFF/cuboNT.off"); // iniciando y cargando el off
	cubo.setBuffers(); // definiendo los atributos de la figura
	cubo.loadTextures("container2.png", "container2_specular.png"); // Cargar Texturas

	//Lampara
	Shape light("OFF/cubo2.off");
	light.setBuffers();

	//Suelo
	Shape suelo("OFF/sueloNT.off");
	suelo.setBuffers();
	suelo.loadTextures("wall.jpg");
	suelo.normales = true;
	suelo.tipo = Tipo::normales;
	if (suelo.flotantesTotal == 8) suelo.tipo = Tipo::normalesTextura;

	//Modelo
	Model ourModel("Models/adamHead/adamHead.gltf");

	// Iniciando programa Shader con el vs y fs de los shapes
	CProgramaShaders shapeShader(cubo.vertexShader(), cubo.fragmentShader());

	// Iniciando programa Shader con el vs y fs de la luz
	CProgramaShaders lightShader(light.vertexShader(), light.fragmentShader());


	// Iniciando programa Shader con el vs y fs del modelo
	CProgramaShaders modelShader("GLSL/model.fs", "GLSL/model.vs");

	// Configuracion del Shader para las texturas de la figura
	shapeShader.usar();
	cubo.shaderConfiguration(shapeShader);

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(ventana)) {
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//---------------------

		procesarEntrada(ventana);
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// USANDO PROGRAMA DEL CUBO
		shapeShader.usar();

		shapeShader.setVec3("viewPos", camera.Position);
		shapeShader.setFloat("material.shininess", 64.0f);

		/*
		   Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		   the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		   by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		   by using 'Uniform buffer objects', but tahat is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/
		// directional light
		shapeShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		shapeShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
		shapeShader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05);
		shapeShader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);
		// point light 1
		shapeShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		shapeShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
		shapeShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
		shapeShader.setVec3("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
		shapeShader.setFloat("pointLights[0].constant", 1.0f);
		shapeShader.setFloat("pointLights[0].linear", 0.14);
		shapeShader.setFloat("pointLights[0].quadratic", 0.07);
			// spotLight
		shapeShader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);
		shapeShader.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);
		shapeShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		shapeShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		shapeShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		shapeShader.setFloat("spotLight.constant", 1.0f);
		shapeShader.setFloat("spotLight.linear", 0.09);
		shapeShader.setFloat("spotLight.quadratic", 0.032);
		shapeShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
		shapeShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)ANCHO / (float)ALTO, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shapeShader.setMat4("proyeccion", projection);
		shapeShader.setMat4("vista", view);

		for (unsigned int i = 0; i < 1; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			model = glm::translate(model, posiciones[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
			shapeShader.setMat4("modelo", model);
			cubo.draw(shapeShader);
		}

		// USANDO PROGRAMA DEL MODELO
		modelShader.usar();

		projection = glm::perspective(glm::radians(camera.Zoom), (float)ANCHO / (float)ALTO, 0.1f, 100.0f);
		view = camera.GetViewMatrix();
		modelShader.setMat4("projection", projection);
		modelShader.setMat4("view", view);
		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		modelShader.setMat4("model", model);
		ourModel.Draw(modelShader);


		// USANDO PROGRAMA DE LA FUENTE DE LUZ
		lightShader.usar();
		lightShader.setMat4("proyeccion", projection);
		lightShader.setMat4("vista", view);

		// we now draw as many light bulbs as we have point lights.
		for (unsigned int i = 0; i < 1; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lightShader.setMat4("modelo", model);
			lightShader.setVec3("colores", pointLightColors[i]);
			light.draw(shapeShader);
		}		


		glfwSwapBuffers(ventana);
		glfwPollEvents();	
	}

	//librerando memoria
	cubo.freeMemory();
	cubo.~Shape();
	shapeShader.~CProgramaShaders();
	glfwTerminate();
	return 0;

}