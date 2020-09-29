#include "modelo.h"
#include "pshader.h"

using namespace std;
using namespace glm;

const unsigned int ANCHO = 800;
const unsigned int ALTO = 600;


void framebuffer_size_callback(GLFWwindow* ventana, int alto, int ancho) {
	glViewport(0, 0, ancho, alto); //actualiza el view port dependiendo de la ventana
}
void procesarEntrada(GLFWwindow* ventana) {
	if (glfwGetKey(ventana, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(ventana, true);
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

	//Cargar Glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Problemas al cargar GLAD\n";
		return -1;
	}

	CModel modelo = CModel("OFF/triangle.off");
	modelo.setBuffers();
	string vsFile, fsFile;
	//Selecionando vector shader y fragment shader a usar
	if (modelo.tipoArchivo == TipoArchivo::Color) {
		vsFile = "GLSL/codigoC.vs";
		fsFile = "GLSL/codigoC.fs";
	}
	else if (modelo.tipoArchivo == TipoArchivo::Textura) {
		vsFile = "GLSL/codigoT.vs";
		fsFile = "GLSL/codigoT.fs";
		modelo.setTextures("wall.jpg");
	}
	else {
		vsFile = "GLSL/codigo.vs";
		fsFile = "GLSL/codigo.fs";
	}


	CProgramaShaders programa_shaders = CProgramaShaders(vsFile, fsFile);
	while (!glfwWindowShouldClose(ventana)) {

		procesarEntrada(ventana);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		programa_shaders.usar();

		glm::mat4 Model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 Projection = glm::perspective(45.0f, 1.0f * ANCHO / ALTO, 0.1f, 100.0f);
		glm::mat4 View = glm::lookAt(glm::vec3(4, 3, -3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		glm::mat4 transformacion = Projection * View * Model;
		transformacion = glm::scale(transformacion, glm::vec3(1.0, 1.0, 1.0));
		programa_shaders.setMat4("transformacion", transformacion);
		//mat4 transformacion = mat4(1.0);			
		//mat4 view = Transformacion::getView(1.0f * ANCHO / ALTO, 0.5f, 100.0f);	//Retorna una matriz homografica	 
		//programa_shaders.setMat4("view", view);
		//mat4 rotacion = rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		//programa_shaders.setMat4("transformacion", rotacion);
		//transformacion = translate(transformacion, vec3(x, y, 0.0));		
		//transformacion = rotate(transformacion, (float)glfwGetTime(), vec3(0, 0, 1));
		//programa_shaders.setVec3("colors", vec3(1.0, 0.5, 0.0));
		//glDrawArrays(GL_TRIANGLES, 0, 6); 
		//Dibuja con los elementos  marcados por los indices

		//glDrawArrays(GL_TRIANGLES, 0, 6); 
		//Dibuja con los elementos  marcados por los indices
		modelo.draw();
		glfwSwapBuffers(ventana);
		glfwPollEvents();

	}

	//librerando memoria
	modelo.~CModel();
	programa_shaders.~CProgramaShaders();
	glfwTerminate();
	return 0;

}