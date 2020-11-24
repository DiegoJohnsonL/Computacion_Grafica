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
	Shape piso("OFF/cuadradoNT.off");
	piso.setBuffers();
	piso.loadTextures("piso.jpg");
	piso.normales = true;
	piso.tipo = Tipo::normales;
	if (piso.flotantesTotal == 8) piso.tipo = Tipo::normalesTextura;


	//techo
	Shape techo("OFF/cuadradoNT.off");
	techo.setBuffers();
	techo.loadTextures("techo.jpg");
	techo.normales = true;
	techo.tipo = Tipo::normales;
	if (techo.flotantesTotal == 8) techo.tipo = Tipo::normalesTextura;

	//paredes
	Shape pared("OFF/cuadradoNT.off");
	pared.setBuffers();
	pared.loadTextures("wall.jpg");
	pared.normales = true;
	pared.tipo = Tipo::normales;
	if (pared.flotantesTotal == 8) pared.tipo = Tipo::normalesTextura;

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
	

	glm::vec3 colorluces[] = {
	    glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
	};
	luz.coloresLuzTotal = sizeof(colorluces) / sizeof(colorluces[0]);

	glm::vec3 posicionesLuz[] = {
		glm::vec3(-10.0f,  2.9f, -11.0f),
		glm::vec3(10.9f,  2.9f, 11.0f),
		glm::vec3(-10.0f, 2.9f, 11.0f),
		glm::vec3(10.0f,  2.9f, -11.0f)
	};
	luz.posicionesTotal = sizeof(posicionesLuz) / sizeof(posicionesLuz[0]);

	glm::vec3 posicionesParedes[] = {
	   glm::vec3(12.0f, 0.0f, 0.0f), 
	   glm::vec3(-12.0f, 0.0f, 0.0f), 
	   glm::vec3(0.0f, 0.0f, 12.0f), 
	   glm::vec3(0.0f, 0.0f, -12.0f)	
	};
	pared.posicionesTotal = sizeof(posicionesParedes) / sizeof(posicionesParedes[0]);


	// Iniciando programa Shader con el vs y fs de la figura
	CProgramaShaders shapeShader(cubo.vertexShader(), cubo.fragmentShader());

	// Iniciando programa Shader con el vs y fs de la luz
	CProgramaShaders luzShader(luz.vertexShader(), luz.fragmentShader());

	// Iniciando programa Shader con el vs y fs de los modelos
	CProgramaShaders ourShader("GLSL/model.vs", "GLSL/model.fs");


	// Configuracion del Shader para las texturas de la figura
	cubo.shaderConfiguration(shapeShader);
	
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
		shapeShader.usar();
		shapeShader.setVec3("viewPos", camera.Position);
		shapeShader.setFloat("material.shininess", 32.0f);

		// directional light
		shapeShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		shapeShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
		shapeShader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05);
		shapeShader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);
		// point light 1
		shapeShader.setVec3("pointLights[0].position", posicionesLuz[0].x, posicionesLuz[0].y, posicionesLuz[0].z);
		shapeShader.setVec3("pointLights[0].ambient", posicionesLuz[0].x * 0.1, posicionesLuz[0].y * 0.1, posicionesLuz[0].z * 0.1);
		shapeShader.setVec3("pointLights[0].diffuse", posicionesLuz[0].x, posicionesLuz[0].y, posicionesLuz[0].z);
		shapeShader.setVec3("pointLights[0].specular", posicionesLuz[0].x, posicionesLuz[0].y, posicionesLuz[0].z);
		shapeShader.setFloat("pointLights[0].constant", 1.0f);
		shapeShader.setFloat("pointLights[0].linear", 0.14);
		shapeShader.setFloat("pointLights[0].quadratic", 0.07);
		// point light 2
		shapeShader.setVec3("pointLights[1].position", posicionesLuz[1].x, posicionesLuz[1].y, posicionesLuz[1].z);
		shapeShader.setVec3("pointLights[1].ambient", posicionesLuz[1].x * 0.1, posicionesLuz[1].y * 0.1, posicionesLuz[1].z * 0.1);
		shapeShader.setVec3("pointLights[1].diffuse", posicionesLuz[1].x, posicionesLuz[1].y, posicionesLuz[1].z);
		shapeShader.setVec3("pointLights[1].specular", posicionesLuz[1].x, posicionesLuz[1].y, posicionesLuz[1].z);
		shapeShader.setFloat("pointLights[1].constant", 1.0f);
		shapeShader.setFloat("pointLights[1].linear", 0.14);
		shapeShader.setFloat("pointLights[1].quadratic", 0.07);
		// point light 3
		shapeShader.setVec3("pointLights[2].position", posicionesLuz[2].x, posicionesLuz[2].y, posicionesLuz[2].z);
		shapeShader.setVec3("pointLights[2].ambient", posicionesLuz[2].x * 0.1, posicionesLuz[2].y * 0.1, posicionesLuz[2].z * 0.1);
		shapeShader.setVec3("pointLights[2].diffuse", posicionesLuz[2].x, posicionesLuz[2].y, posicionesLuz[2].z);
		shapeShader.setVec3("pointLights[2].specular", posicionesLuz[2].x, posicionesLuz[2].y, posicionesLuz[2].z);
		shapeShader.setFloat("pointLights[2].constant", 1.0f);
		shapeShader.setFloat("pointLights[2].linear", 0.14);
		shapeShader.setFloat("pointLights[2].quadratic", 0.07);
		// point light 4
		shapeShader.setVec3("pointLights[3].position", posicionesLuz[3].x, posicionesLuz[3].y, posicionesLuz[3].z);
		shapeShader.setVec3("pointLights[3].ambient", posicionesLuz[3].x * 0.1, posicionesLuz[3].y * 0.1, posicionesLuz[3].z * 0.1);
		shapeShader.setVec3("pointLights[3].diffuse", posicionesLuz[3].x, posicionesLuz[3].y, posicionesLuz[3].z);
		shapeShader.setVec3("pointLights[3].specular", posicionesLuz[3].x, posicionesLuz[3].y, posicionesLuz[3].z);
		shapeShader.setFloat("pointLights[3].constant", 1.0f);
		shapeShader.setFloat("pointLights[3].linear", 0.14);
		shapeShader.setFloat("pointLights[3].quadratic", 0.07);
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
			cubo.draw(shapeShader);
		}

		piso.proyeccion = projection;
		piso.vista = view;
		piso.modelo = glm::mat4(1.0f);
		piso.modelo = glm::translate(piso.modelo, glm::vec3(0.0f, 0.0f, 0.0f));
		piso.draw(shapeShader);
		
		techo.proyeccion = projection;
		techo.vista = view;
		techo.modelo = glm::mat4(1.0f);
		techo.modelo = glm::translate(techo.modelo, glm::vec3(0.0f, 4.0f, 0.0f));
		techo.draw(shapeShader);

		pared.proyeccion = projection;
		pared.vista = view;
		for (unsigned int i = 0; i < pared.posicionesTotal; i++)
		{
			pared.modelo = glm::mat4(1.0f);
			pared.modelo = glm::translate(pared.modelo, posicionesParedes[i]);
			if (i < 2) {
				pared.modelo = rotate(pared.modelo, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
			}
			else {
				pared.modelo = rotate(pared.modelo, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
			}
			pared.draw(shapeShader);
		}
				

		// USANDO EL PROGRAMA DE FUNENTE DE LUZ
		luzShader.usar();
		luz.proyeccion = projection;
		luz.vista = view;		
		for (unsigned int i = 0; i < luz.posicionesTotal; i++)
		{
			luz.modelo = glm::mat4(1.0f);
			luz.modelo = glm::translate(luz.modelo, posicionesLuz[i]);
			luz.modelo = glm::scale(luz.modelo, vec3(0.12f)); // Make it a smaller cube
			luz.colores = colorluces[i];
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
	shapeShader.~CProgramaShaders();
	glfwTerminate();
	return 0;

}