#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

enum class Tipo{Traslacion, Rotacion, Escalamiento, Perspectiva, View};

class Transformacion
{
	Tipo tipo;
	vec3 vector;
	float angulo;

public:
	Transformacion(Tipo tipo, vec3 vector, float angulo = 0) {
		this->tipo = tipo;
		this->vector = vector;
		this->angulo = angulo;
	};		
	~Transformacion(){};


	//Creando la matriz homografica
	static mat4 getView(float aspecto, float zNear, float zFar) {
		vec3 eye = vec3(4, 3, -3);
		vec3 center = vec3(0, 0, 0);
		vec3 up = vec3(0, 1, 0);
		float fovy = 45.0f;
		mat4 view = perspective(fovy, aspecto, zNear, zFar) * lookAt(eye, center, up);
		return view;
	}

	mat4 transformar() {
		switch (this->tipo)
		{
		case Tipo::Traslacion: return translate(mat4(1.0f), this->vector);
		case Tipo::Rotacion: return rotate(mat4(1.0f), this->angulo * float(glfwGetTime()) , this->vector);
		case Tipo::Escalamiento: return scale(mat4(1.0f), this->vector);		
		}
	}
	
};

