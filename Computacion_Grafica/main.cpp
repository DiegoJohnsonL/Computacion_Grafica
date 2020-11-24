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
		glm::vec3(0.6f,  4.0f,  2.0f),
	};

	glm::vec3 pointLightColors[] = {
		glm::vec3(0.9f, 0.9f, 0.1f),
	};

	//Cubo
	Shape cubo("OFF/cuboNT.off"); // iniciando y cargando el off
	cubo.setBuffers(); // definiendo los atributos de la figura
	cubo.loadTextures("container2.png", "container2_specular.png"); // Cargar Texturas

	//Lampara
	Shape light("OFF/esfera.off");
	light.setBuffers();

	//Suelo
	Shape suelo("OFF/sueloNT.off");
	suelo.setBuffers();
	suelo.loadTextures("container2.png");
	suelo.normales = true;
	suelo.tipo = Tipo::normales;
	if (suelo.flotantesTotal == 8) suelo.tipo = Tipo::normalesTextura;

	//Modelo
	Model ourModel("Models/backpack/backpack.obj");

	// Iniciando programa Shader con el vs y fs de los shapes
	CProgramaShaders shaderCubo(cubo.vertexShader(), cubo.fragmentShader());
	CProgramaShaders shaderSuelo(suelo.vertexShader(), suelo.fragmentShader());

	// Iniciando programa Shader con el vs y fs de la luz
	CProgramaShaders shaderIluminacion(light.vertexShader(), light.fragmentShader());

	// Iniciando programa Shader con el vs y fs del modelo
	CProgramaShaders shaderModelo("GLSL/model.fs", "GLSL/model.vs");

	// Configuracion del Shader para las texturas de la figura
	shaderCubo.usar();
	cubo.shaderConfiguration(shaderCubo);

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
		shaderCubo.usar();
		shaderCubo.setVec3("viewPos", camera.Position);
		shaderCubo.setFloat("material.shininess", 64.0f);	

		// directional light
		shaderCubo.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		shaderCubo.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
		shaderCubo.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05);
		shaderCubo.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);
		// point light 1
		shaderCubo.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		shaderCubo.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
		shaderCubo.setVec3("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
		shaderCubo.setVec3("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
		shaderCubo.setFloat("pointLights[0].constant", 1.0f);
		shaderCubo.setFloat("pointLights[0].linear", 0.14);
		shaderCubo.setFloat("pointLights[0].quadratic", 0.07);
		// spotLight
		shaderCubo.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);
		shaderCubo.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);
		shaderCubo.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		shaderCubo.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		shaderCubo.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		shaderCubo.setFloat("spotLight.constant", 1.0f);
		shaderCubo.setFloat("spotLight.linear", 0.09);
		shaderCubo.setFloat("spotLight.quadratic", 0.032);
		shaderCubo.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
		shaderCubo.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		// view/projection transformations
		glm::mat4 proyeccion = glm::perspective(glm::radians(camera.Zoom), (float)ANCHO / (float)ALTO, 0.1f, 100.0f);
		glm::mat4 vista = camera.GetViewMatrix();
		glm::mat4 model = mat4(1.0f);
		shaderCubo.setMat4("proyeccion", proyeccion);
		shaderCubo.setMat4("vista", vista);

		for (unsigned int i = 0; i < 1; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			model = glm::translate(model, posiciones[i]);
			float angle = 20.0f * i;
		    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
			shaderCubo.setMat4("modelo", model);
			cubo.draw(shaderCubo);
		}

		shaderSuelo.usar();
		shaderSuelo.setMat4("proyeccion", proyeccion);
		shaderSuelo.setMat4("vista", vista);
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.3f, 0.0f));
		shaderSuelo.setMat4("model", model);
		suelo.draw(shaderSuelo);


		//// USANDO PROGRAMA DEL MODELO
		shaderModelo.usar();
		shaderModelo.setMat4("projection", proyeccion);
		shaderModelo.setMat4("view", vista);
		// render the loaded model
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.001f));	// it's a bit too big for our scene, so scale it down
		shaderModelo.setMat4("model", model);
		ourModel.Draw(shaderModelo);


		// USANDO PROGRAMA DE LA FUENTE DE LUZ
		shaderIluminacion.usar();
		shaderIluminacion.setMat4("proyeccion", proyeccion);
		shaderIluminacion.setMat4("vista", vista);
		// we now draw as many light bulbs as we have point lights.
		for (unsigned int i = 0; i < 1; i++)
		{
			pointLightPositions[i].x = sin(glfwGetTime()) * 3.0f;
			pointLightPositions[i].z = cos(glfwGetTime()) * 2.0f;
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			shaderIluminacion.setMat4("modelo", model);
			shaderIluminacion.setVec3("colores", pointLightColors[i]);
			light.draw(shaderCubo);
		}		

		glfwSwapBuffers(ventana);
		glfwPollEvents();	
	}

	//librerando memoria
	cubo.freeMemory();
	cubo.~Shape();
	shaderCubo.~CProgramaShaders();
	glfwTerminate();
	return 0;

}