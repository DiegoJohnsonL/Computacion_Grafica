#include "modelo.h"
#include "Camera.h" 
using namespace std;
using namespace glm;

const unsigned int ANCHO = 900;
const unsigned int ALTO = 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = ANCHO / 2.0f;
float lastY = ALTO / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

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
		glm::vec3(0.0,  1.5,  0.0),
		glm::vec3(2.0,  0.0, 1.0),
		glm::vec3(-1.0, 0.0, 2.0)
	};

	// Arreglo de posiciones de las lamparas
	glm::vec3 pointLightPositions[] = {
		glm::vec3(-2.0f, 4.0f, -1.0f)
	};
	glm::vec3 pointLightColors[] = {
		glm::vec3(0.9f, 0.9f, 0.9f)
	};

	//Cubo
	CModel cubo("OFF/cuboNT.off"); // iniciando y cargando el off// definiendo los atributos de la figura
	cubo.setBuffers();
	cubo.loadTextures("textures/ilusion.jpg"); // Cargar Texturas	
	cubo.loadArray("posiciones", posiciones);

	//Cubo
	CModel piso("OFF/cuadradoNT.off"); // iniciando y cargando el off// definiendo los atributos de la figura
	piso.setBuffers();
	piso.loadTextures("textures/grass.png"); // Cargar Texturas	
	piso.normales = true;
	piso.tipo = Tipo::normales;
	if (piso.flotantesTotal == 8) piso.tipo = Tipo::normalesTextura;

	// Figura 2 - Lampara
	CModel light("OFF/esfera.off");
	light.setBuffers();

	// Armando Shaders
	CProgramaShaders shader("GLSL/sombra.vs", "GLSL/sombra.fs");
	CProgramaShaders depthShader("GLSL/profundidad.vs", "GLSL/profundidad.fs");
	CProgramaShaders lightShader(light.vertexShader(), light.fragmentShader());
	// configure depth map FBO
	// -----------------------
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shader.usar();
	shader.setInt("diffuseTexture", 0);
	shader.setInt("shadowMap", 1);

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(ventana)) {
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//---------------------

		procesarEntrada(ventana);
		// change light position over time
        pointLightPositions[0].x = sin(glfwGetTime()) * 3.0f;
		pointLightPositions[0].z = cos(glfwGetTime()) * 2.0f;
		pointLightPositions[0].y = 5.0 + cos(glfwGetTime()) * 1.0f;

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cubo.bindTexture();
	
		// lightSpaceMatrix
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 7.5f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(pointLightPositions[0], glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		depthShader.usar();
		depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, piso.texture1);

		piso.modelo = glm::mat4(1.0);
		piso.draw(depthShader);
		for (unsigned int i = 0; i < 3; i++) {
			cubo.modelo = glm::mat4(1.0);
			cubo.modelo = translate(cubo.modelo, posiciones[i]);
			cubo.modelo = scale(cubo.modelo, glm::vec3(0.5));
			cubo.draw(depthShader);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, ANCHO, ALTO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 2. render scene as normal using the generated depth/shadow map  
		// --------------------------------------------------------------
		glViewport(0, 0, ANCHO, ALTO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.usar();
		// set light uniforms
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)ALTO / (float)ANCHO, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("proyeccion", projection);
		shader.setMat4("vista", view);
		// set light uniforms
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("lightPos", lightPos);
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, piso.texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		mat4 proyeccion = glm::perspective(glm::radians(camera.Zoom), (float)ALTO/ (float)ANCHO, 0.1f, 100.0f);
		mat4 vista = camera.GetViewMatrix();
		piso.proyeccion = proyeccion;
		piso.vista = vista;
		piso.modelo = mat4(1.0);
		piso.draw(shader);
		cubo.proyeccion = proyeccion;
		cubo.vista = vista;
		for (unsigned int i = 0; i < 3; i++) {
			cubo.modelo = glm::mat4(1.0);
			cubo.modelo = translate(cubo.modelo, posiciones[i]);
			cubo.modelo = scale(cubo.modelo, glm::vec3(0.50));
			cubo.draw(shader);
		}

		lightShader.usar();
		light.proyeccion = proyeccion;
		light.vista = vista;
		light.modelo = glm::mat4(1.0);
		light.modelo = translate(light.modelo, pointLightPositions[0]);
		light.modelo = glm::scale(light.modelo, vec3(0.25));
		light.colorInterno = false;
		light.colores = pointLightColors[0];
		light.draw(lightShader);

		glfwSwapBuffers(ventana);
		glfwPollEvents();	
	}
	//librerando memoria
	cubo.freeMemory();
	cubo.~CModel();
	glfwTerminate();
	return 0;
}

//                                                                                    TIPOS DE ILUMINACION



//// == ==============================================================================================
////       DESERT
//// == ==============================================================================================
//glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
//[...]
//glm::vec3 pointLightColors[] = {
//	glm::vec3(1.0f, 0.6f, 0.0f),
//	glm::vec3(1.0f, 0.0f, 0.0f),
//	glm::vec3(1.0f, 1.0, 0.0),
//	glm::vec3(0.2f, 0.2f, 1.0f)
//};
//[...]
//// Directional light
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.3f, 0.24f, 0.14f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.7f, 0.42f, 0.26f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
//// Point light 1
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032);
//// Point light 2
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032);
//// Point light 3
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032);
//// Point light 4
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032);
//// SpotLight
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.8f, 0.8f, 0.0f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.8f, 0.8f, 0.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(13.0f)));
//// == ==============================================================================================
////       FACTORY
//// == ==============================================================================================
//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//[...]
//glm::vec3 pointLightColors[] = {
//	glm::vec3(0.2f, 0.2f, 0.6f),
//	glm::vec3(0.3f, 0.3f, 0.7f),
//	glm::vec3(0.0f, 0.0f, 0.3f),
//	glm::vec3(0.4f, 0.4f, 0.4f)
//};
//[...]
//// Directional light
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.05f, 0.05f, 0.1f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.2f, 0.2f, 0.7);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.7f, 0.7f, 0.7f);
//// Point light 1
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032);
//// Point light 2
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032);
//// Point light 3
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032);
//// Point light 4
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032);
//// SpotLight
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.009);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.0032);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(10.0f)));
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(12.5f)));
//// == ==============================================================================================
////       HORROR
//// == ==============================================================================================
//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//[...]
//glm::vec3 pointLightColors[] = {
//	glm::vec3(0.1f, 0.1f, 0.1f),
//	glm::vec3(0.1f, 0.1f, 0.1f),
//	glm::vec3(0.1f, 0.1f, 0.1f),
//	glm::vec3(0.3f, 0.1f, 0.1f)
//};
//[...]
//// Directional light
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.0f, 0.0f, 0.0f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.05f, 0.05f, 0.05);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.2f, 0.2f, 0.2f);
//// Point light 1
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.14);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.07);
//// Point light 2
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.14);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.07);
//// Point light 3
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.22);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.20);
//// Point light 4
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.14);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.07);
//// SpotLight
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(10.0f)));
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));
//// == ==============================================================================================
////       BIOCHEMICAL LAB
//// == ==============================================================================================
//glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
//[...]
//glm::vec3 pointLightColors[] = {
//	glm::vec3(0.4f, 0.7f, 0.1f),
//	glm::vec3(0.4f, 0.7f, 0.1f),
//	glm::vec3(0.4f, 0.7f, 0.1f),
//	glm::vec3(0.4f, 0.7f, 0.1f)
//};
//[...]
//// Directional light
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.5f, 0.5f, 0.5f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 1.0f, 1.0f, 1.0f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 1.0f, 1.0f, 1.0f);
//// Point light 1
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.07);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.017);
//// Point light 2
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.07);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.017);
//// Point light 3
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.07);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.017);
//// Point light 4
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.07);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.017);
//// SpotLight
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 1.0f, 0.0f);
//glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 1.0f, 0.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.07);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.017);
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(7.0f)));
//glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(10.0f)));