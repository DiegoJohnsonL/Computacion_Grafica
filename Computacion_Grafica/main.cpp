#include "Camera.h" 
#include "model.h"
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
	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	

	//glEnable(GL_FRAMEBUFFER_SRGB);
	//Cubo
	Shape cubo("OFF/cuboNT.off"); // iniciando y cargando el off
	cubo.setBuffers(); // definiendo los atributos de la figura
	cubo.loadTextures("container2.png", "container2_specular.png"); // Cargar Texturas

	//Piso
	Shape piso("OFF/cuadradoT.off");
	piso.setBuffers();
	piso.loadTextures("piso.jpg");

	//techo
	Shape techo("OFF/cuadradoT.off");
	techo.setBuffers();
	techo.loadTextures("techo.jpg");

	//paredes
	Shape pared("OFF/cuadradoT.off");
	pared.setBuffers();
	pared.loadTextures("wall.jpg");

	// Luz- Lampara
	Shape luz("OFF/esfera.off");
	luz.setBuffers();

	//Modelo 1 - mochila
	Model mochila("Modelos/backpack/backpack.obj");


	// Arreglo de posiciones de los cubos
	glm::vec3 posicionesCubo[] = {
		glm::vec3(0.0,  -0.5,  0.0),
	};
	cubo.posicionesTotal = sizeof(posicionesCubo) / sizeof(posicionesCubo[0]);

	// Arreglo de posiciones de las lamparas
	glm::vec3 pointLightPositions[] = {
	   glm::vec3(0.7f,  0.5f,  1.0f),
	};
	luz.posicionesTotal = sizeof(pointLightPositions) / sizeof(pointLightPositions[0]);

	glm::vec3 pointLightColors[] = {
	   glm::vec3(0.9f, 0.1f, 0.9f),
	};
	luz.coloresLuzTotal = sizeof(pointLightColors) / sizeof(pointLightColors[0]);

	glm::vec3 posicionesParedes[] = {
	   glm::vec3(8.0f, 2.0f, 0.0f), 
	   glm::vec3(-8.0f, 2.0f, 0.0f), 
	   glm::vec3(0.0f, 2.0f, 8.0f), 
	   glm::vec3(0.0f, 2.0f, -8.0f)	
	};
	pared.posicionesTotal = sizeof(posicionesParedes) / sizeof(posicionesParedes[0]);


	// Iniciando programa Shader con el vs y fs de la figura
	CProgramaShaders cuboShader(cubo.vertexShader(), cubo.fragmentShader());

	// Iniciando programa Shader con el vs y fs de la luz
	CProgramaShaders luzShader(luz.vertexShader(), luz.fragmentShader());

	// Iniciando programa Shader con el vs y fs de la piso
	CProgramaShaders roomShader(piso.vertexShader(), piso.fragmentShader());

	// Iniciando programa Shader con el vs y fs de los modelos
	CProgramaShaders ourShader("GLSL/model.vs", "GLSL/model.fs");


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
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// USANDO PROGRAMA DEL CUBO
		cuboShader.usar();
		cuboShader.setVec3("viewPos", camera.Position);
		cuboShader.setFloat("material.shininess", 32.0f);

		// directional light
		cuboShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		cuboShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
		cuboShader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05f);
		cuboShader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);
		// point light 1
		cuboShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		cuboShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
		cuboShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
		cuboShader.setVec3("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
		cuboShader.setFloat("pointLights[0].constant", 1.0f);
		cuboShader.setFloat("pointLights[0].linear", 0.14);
		cuboShader.setFloat("pointLights[0].quadratic", 0.07);
		// spotLight
		cuboShader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);
		cuboShader.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);
		cuboShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		cuboShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		cuboShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		cuboShader.setFloat("spotLight.constant", 1.0f);
		cuboShader.setFloat("spotLight.linear", 0.09);
		cuboShader.setFloat("spotLight.quadratic", 0.032);
		cuboShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
		cuboShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)ANCHO / (float)ALTO, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		cubo.proyeccion = projection;
		cubo.vista = view;
		for (unsigned int i = 0; i < cubo.posicionesTotal; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			cubo.modelo = mat4(1.0f); // make sure to initialize matrix to identity matrix first
			cubo.modelo = scale(cubo.modelo, vec3(0.5f,0.5f,0.5f));
			cubo.modelo = translate(cubo.modelo, posicionesCubo[i]);
			float angle = 20.0f * i;
			//cubo.modelo = rotate(cubo.modelo, radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			cubo.draw(cuboShader);
		}

		// USANDO EL PROGRAMA DEl PISO
		roomShader.usar();

		piso.proyeccion = projection;
		piso.vista = view;
		piso.modelo = glm::mat4(1.0f);
		piso.modelo = glm::translate(piso.modelo, glm::vec3(0.0f, 0.0f, 0.0f));
		piso.modelo = glm::scale(piso.modelo, vec3(2.0f, 1.0f, 2.0f));			
		piso.draw(roomShader);
		
		techo.proyeccion = projection;
		techo.vista = view;
		techo.modelo = glm::mat4(1.0f);
		techo.modelo = glm::translate(techo.modelo, glm::vec3(0.0f, 6.0f, 0.0f));
		techo.modelo = glm::scale(techo.modelo, vec3(2.0f, 1.0f, 2.0f));
		techo.draw(roomShader);

		pared.proyeccion = projection;
		pared.vista = view;
		for (unsigned int i = 0; i < pared.posicionesTotal; i++)
		{
			pared.modelo = glm::mat4(1.0f);
			pared.modelo = glm::translate(pared.modelo, posicionesParedes[i]);
			if (i < 2) {
				pared.modelo = rotate(pared.modelo, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
				pared.modelo = glm::scale(pared.modelo, vec3(1.0f, 0.0f, 2.0f));
			}
			else {
				pared.modelo = rotate(pared.modelo, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
				pared.modelo = glm::scale(pared.modelo, vec3(2.0f, 0.0f, 1.0f));
			}
			pared.draw(roomShader);
		}
				

		// USANDO EL PROGRAMA DE FUNENTE DE LUZ
		luzShader.usar();
		luz.proyeccion = projection;
		luz.vista = view;		
		for (unsigned int i = 0; i < luz.coloresLuzTotal; i++)
		{
			luz.modelo = glm::mat4(1.0f);
			luz.modelo = glm::translate(luz.modelo, pointLightPositions[i]);
			luz.modelo = glm::scale(luz.modelo, vec3(0.12f)); // Make it a smaller cube
			luz.colores = pointLightColors[i];
			luz.draw(luzShader);
		}


		//  USANDO EL PROGRAMA DE LOS MODELOS
		ourShader.usar();
		// view/projection transformations
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);
		// render the loaded model
		mat4 model = mat4(1.0f);
		model = scale(model, glm::vec3(0.1f));// it's a bit too big for our scene, so scale it down
		model = translate(model, glm::vec3(0.0f, 1.0f, 0.0f)); // translate it down so it's at the center of the scene			
		model = rotate(model, radians(90.0f), vec3(-1.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);
		mochila.Draw(ourShader);



		glfwSwapBuffers(ventana);
		glfwPollEvents();	
	}

	//librerando memoria
	cubo.freeMemory();
	cubo.~Shape();
	cuboShader.~CProgramaShaders();
	glfwTerminate();
	return 0;

}