#pragma once
#include <iostream>
#include <vector>
#include <map>

#include <GL/glew.h>
#include <GL/freeglut.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "glsl.h"
#include "objloader.hpp"
#include "texture.hpp"
#include "Model.h"

using namespace std;

//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

const int WIDTH = 800, HEIGHT = 600;
const char * fragshader_name = "fragmentshader.fsh";
const char * vertexshader_name = "vertexshader.vsh";
unsigned const int DELTA = 10;

//--------------------------------------------------------------------------------
// Mesh variables
//--------------------------------------------------------------------------------


/// These variables will change if the mouse moves
float mouseOriginX = -1;
float mouseOriginY = -1;

float mouseDeltaX = 0.0f;
float mouseDeltaY = 0.0f;

// These variables will change if the key state changes
float cameraEyeDeltaX = 0.0f, cameraEyeDeltaZ = 0.0f, cameraCenterDeltaY = 0.0f, cameraCenterDeltaX = 0.0f;

float mouseMovementSpeed = 1.0f;
float movementSpeed = 0.5f;
float lookAroundSpeed = .1f;

//--------------------------------------------------------------------------------
// Type definitions
//--------------------------------------------------------------------------------

struct LightSource {
	glm::vec3 position;
};

// angle (in PI) of rotation for the camera direction
float angleX = 0.0f;
float angleY = 0.0f;

// actual vector representing the camera's direction
float lx = 0.0f, lz = -1.0f, ly = 0.0f;
float rightX = -lz;
float rightZ = lx;

// XYZ position of the camera
float x = 0.0f, z = 5.0f, y = 1.75f;


float mousedeltaAngleX = 0.0f;
float mousedeltaAngleY = 0.0f;
float deltaAngleX = 0.0f;
float deltaAngleY = 0.0f;
float deltaMoveX = 0;
float deltaMoveZ = 0;
int xOrigin = 0;
int yOrigin = 0;
bool direction = false;
bool movableCamera = true;

float tempx, templx, temply, tempz, templz, tempy;

float moveSpeed = 0.2f;
//--------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------

GLuint shader_id;
GLuint vao;

// GLuint uniform_mvp;
GLuint uniform_mv;
GLuint uniform_apply_texture;
GLuint uniform_material_ambient;
GLuint uniform_material_diffuse;
GLuint uniform_material_specular;
GLuint uniform_material_power;

glm::mat4 model, view, projection;
glm::mat4 mvp;

LightSource light;
vector<Model> models;


//--------------------------------------------------------------------------------
// Keyboard and mouse handling
//--------------------------------------------------------------------------------


void mouseHandler(int x, int y)
{
	if (movableCamera) {
		if (xOrigin >= 0) {

			// update deltaAngle
			mousedeltaAngleX = (x - xOrigin) * lookAroundSpeed;
		}
		if (yOrigin >= 0) {

			// update deltaAngle
			mousedeltaAngleY = (y - yOrigin) * lookAroundSpeed;
		}
		xOrigin = x;
		yOrigin = y;
	}
}


void switchCamera() {
	if (movableCamera == true) {
		tempx = x; tempy = y; tempz = z; templx = lx; temply = ly; templz = lz;
		movableCamera = false;
		x = 94, y = 40, z = 0;
		lx = -30, ly = 10, lz = 0;
	}
	else if (movableCamera == false) {
		x = tempx; y = tempy; z = tempz; lx = templx; ly = temply; lz = templz;
		movableCamera = true;
	}
}


void pressKeyHandler(unsigned char key, int a, int b)
{
	switch (key)
	{
	case 27:
		glutExit();

	case 'c':
		switchCamera();
		break;
	}

	if (movableCamera)
	{
		switch (key)
		{
		case 'w': deltaMoveZ = movementSpeed; break;
		case 's': deltaMoveZ = -movementSpeed; break;
		case 'd': deltaMoveX = movementSpeed; break;
		case 'a': deltaMoveX = -movementSpeed; break;
		case 'i': deltaAngleY = lookAroundSpeed; break;
		case 'k': deltaAngleY = -lookAroundSpeed; break;
		case 'l': deltaAngleX = lookAroundSpeed; break;
		case 'j': deltaAngleX = -lookAroundSpeed; break;
		}
	}
}

void releaseKeyHandler(unsigned char key, int a, int b)
{
	switch (key) {
	case 'w':
	case 's': deltaMoveZ = 0.0f;
		break;
	case 'd':
	case 'a': deltaMoveX = 0.0f;
		break;
	case 'i':
	case 'k': deltaAngleY = 0.0f;
		break;
	case 'l':
	case 'j': deltaAngleX = 0.0f;
		break;
	}
}
//--------------------------------------------------------------------------------
// Position and Camera Handling
//--------------------------------------------------------------------------------
void calculateCameraEye(float deltaMoveX, float cameraEydeltaMoveZeDeltaZ, float moveSpeed) {

	x += deltaMoveZ * lx * moveSpeed;
	z += deltaMoveZ * lz * moveSpeed;
	x += deltaMoveX * rightX * moveSpeed;
	z += deltaMoveX * rightZ * moveSpeed;
}

float calculatedAngleForCameraCenterX = 0.0f;
float calculatedAngleForCameraCenterY = 0.0f;

void calculateCameraCenter(float internaldeltaAngleX, float internaldeltaAngleY) {
	angleX += internaldeltaAngleX + mousedeltaAngleX;
	angleY += internaldeltaAngleY + mousedeltaAngleY;

	if (angleY >= 1.57f) angleY = 1.57f;
	if (angleY <= -1.57f) angleY = -1.57f;

	mousedeltaAngleX = 0;
	mousedeltaAngleY = 0;
	lx = sin(angleX);
	ly = -sin(angleY);
	lz = -cos(angleX);
	rightX = -lz;
	rightZ = lx;
}

//--------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------

void Render()
{
	if (deltaMoveX != 0.0f || deltaMoveZ != 0.0f)
		calculateCameraEye(deltaMoveX, deltaMoveZ, moveSpeed); // #TODO: Movementspeed?

	if (deltaAngleX || deltaAngleY || mousedeltaAngleX || mousedeltaAngleY)
			calculateCameraCenter(deltaAngleX, deltaAngleY);

	// Reset transformations
	glLoadIdentity();

	
	// Set the camera
	view = glm::lookAt(
		glm::vec3(x, y, z),
		glm::vec3(x + lx, 1.75f + ly, z + lz),
		glm::vec3(0.0, 1.0, 0.0)
	);
	
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Send mvp
	glUseProgram(shader_id);

    // Do transformation

	for (int i = 0; i < models.size(); ++i)
	{

		if( i == 0)
			models[i].model = glm::rotate(models[i].model, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
		
		models[i].mv = view * models[i].model;

		// if(models[i].material.applied)
		// {
			glUniform1i(uniform_apply_texture, 1);
			glBindTexture(GL_TEXTURE_2D, models[i].textureID);
		// }
		// else
			// glUniform1i(uniform_apply_texture, 0);

		glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(models[i].mv));
		glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(models[i].material.ambientColor));
		glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(models[i].material.diffuseColor));
		glUniform3fv(uniform_material_specular, 1, glm::value_ptr(models[i].material.specular));
		glUniform1f(uniform_material_power, models[i].material.power);

		glBindVertexArray(models[i].vao);
		glDrawArrays(GL_TRIANGLES, 0, models[i].vertices.size());
		glBindVertexArray(0);
	}
	glutSwapBuffers();
	//
	// // Send vao
	// glBindVertexArray(vao);
	// glDrawElements(GL_LINES, sizeof(cube_elements) / sizeof(GLushort),
	// 	GL_UNSIGNED_SHORT, 0);
	// glBindVertexArray(0);
}



//------------------------------------------------------------
// void Render(int n)
// Render method that is called by the timer function
//------------------------------------------------------------

void Render(int n)
{
    Render();
    glutTimerFunc(DELTA, Render, 0);
}


//------------------------------------------------------------
// void InitGlutGlew(int argc, char **argv)
// Initializes Glut and Glew
//------------------------------------------------------------

void InitGlutGlew(int argc, char **argv)
{
    glutInit(&argc, argv);

	glutSetOption(GLUT_MULTISAMPLE, 8);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Hello OpenGL");
    glutDisplayFunc(Render);

	// Mouse and keyboard handlers
    glutKeyboardFunc(pressKeyHandler);
	glutPassiveMotionFunc(mouseHandler);
	glutIgnoreKeyRepeat(1);
	glutKeyboardUpFunc(releaseKeyHandler);
    glutTimerFunc(DELTA, Render, 0);

	glEnable(GLUT_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

    glewInit();
}


//------------------------------------------------------------
// void InitShaders()
// Initializes the fragmentshader and vertexshader
//------------------------------------------------------------

void InitShaders()
{
    char * vertexshader = glsl::readFile(vertexshader_name);
    GLuint vsh_id = glsl::makeVertexShader(vertexshader);

    char * fragshader = glsl::readFile(fragshader_name);
    GLuint fsh_id = glsl::makeFragmentShader(fragshader);

    shader_id = glsl::makeShaderProgram(vsh_id, fsh_id);
}


//------------------------------------------------------------
// void InitMatrices()
//------------------------------------------------------------

void InitMatrices()
{
    /*
	models[1].model = glm::translate(glm::mat4(), glm::vec3(40, -2.5, -15));
	models[1].model = glm::scale(models[1].model, glm::vec3(144, 1, 48));*/


	//ignore = glm::translate(ignore, glm::vec3(1000.0f, 0.0f, 0.0f));

	//
	// view = glm::lookAt(
	// 	camera.eye,
	// 	glm::vec3(camera.eye.x + camera.center.x, 1.75f + camera.center.y, camera.eye.z + camera.center.z),
	// 	// camera.center,
	// 	camera.up
	// );

	view = glm::lookAt(
		glm::vec3(0.0, 2.0, 6.0),
		glm::vec3(1.5, 0.5, 0.0),
		glm::vec3(0.0, 1.0, 0.0)
	);

    projection = glm::perspective(
        glm::radians(45.0f),
        1.0f * WIDTH / HEIGHT, 0.1f,
        400.0f);

	for (int i = 0; i < models.size(); i++)
	{
		models[i].mv = view * models[i].model;
	}
}

bool checkModelComplete(Model model, int i)
{
	//Model test = empty_model;

	//if (model.vertices == empty_model.vertices ||
	//	model.normals == empty_model.normals ||
	//	model.uvs == empty_model.uvs ||
	//	(
	//		model.material.ambientColor == empty_model.material.ambientColor &&
	//		model.material.diffuseColor == empty_model.material.diffuseColor &&
	//		model.material.specular == empty_model.material.specular &&
	//		model.material.power == empty_model.material.power &&
	//		model.material.applied == empty_model.material.applied
	//		) || model.textureID == empty_model.textureID)
	//	// model.model == empty_model.model  ||
	//	// model.mv    == empty_model.mv     ||  These are not included because they will be changed by OpenGL while rendering.
	//	// model.vao   == empty_model.vao    ||
	//{
	//	cout << name << " model is incomplete." << endl;
	//	return false;
	//}
	//cout << "Model " << name << " is complete" << endl;
	return true;
}

//------------------------------------------------------------
// void InitBuffers()
// Allocates and fills buffers
//------------------------------------------------------------

void InitBuffers()
{
	GLuint vbo_vertices, vbo_normals, vbo_uvs;

	GLuint position_id = glGetAttribLocation(shader_id, "position");
	GLuint normal_id = glGetAttribLocation(shader_id, "normal");
	GLuint uv_id = glGetAttribLocation(shader_id, "uv");

	// Attach to program (needed to send uniform vars)
	glUseProgram(shader_id);

	// Make uniform vars
	uniform_mv = glGetUniformLocation(shader_id, "mv");
	GLuint uniform_proj = glGetUniformLocation(shader_id, "projection");
	GLuint uniform_light_pos = glGetUniformLocation(shader_id, "lightPos");
	uniform_apply_texture = glGetUniformLocation(shader_id, "applyTexture");
	uniform_material_ambient = glGetUniformLocation(shader_id, "matAmbient");
	uniform_material_diffuse = glGetUniformLocation(shader_id, "matDiffuse");
	uniform_material_specular = glGetUniformLocation(shader_id, "matSpecular");
	uniform_material_power = glGetUniformLocation(shader_id, "matPower");

	// Fill uniform vars
	glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3fv(uniform_light_pos, 1, glm::value_ptr(light.position));

	for (unsigned int i = 0; i < models.size(); i++)
	{
		// vbo for vertices
		glGenBuffers(1, &vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, models[i].vertices.size() * sizeof(glm::vec3),
			&models[i].vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vbo for normals
		glGenBuffers(1, &vbo_normals);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
		glBufferData(GL_ARRAY_BUFFER, models[i].normals.size() * sizeof(glm::vec3),
			&models[i].normals[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vbo for uvs
		glGenBuffers(1, &vbo_uvs);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
		glBufferData(GL_ARRAY_BUFFER, models[i].uvs.size() * sizeof(glm::vec2),
			&models[i].uvs[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &(models[i].vao));
		glBindVertexArray(models[i].vao);
		
		// Bind vertices to vao
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(position_id);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
		glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(normal_id);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
		glVertexAttribPointer(uv_id, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(uv_id);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindVertexArray(0);
		
	}
	////**************************** old InitBuffer

	//GLuint vbo_colors;

 //   GLuint ibo_elements;

 //   // vbo for vertices
 //   glGenBuffers(1, &vbo_vertices);
 //   glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
 //   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 //   glBindBuffer(GL_ARRAY_BUFFER, 0);

 //   // vbo for colors
 //   glGenBuffers(1, &vbo_colors);
 //   glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
 //   glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
 //   glBindBuffer(GL_ARRAY_BUFFER, 0);

 //   // vbo for elements
 //   glGenBuffers(1, &ibo_elements);
 //   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
 //   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements),
 //       cube_elements, GL_STATIC_DRAW);
 //   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

 //   // Get vertex attributes
 //   position_id = glGetAttribLocation(shader_id, "position");
 //   color_id = glGetAttribLocation(shader_id, "color");

 //   // Allocate memory for vao
 //   glGenVertexArrays(1, &vao);

 //   // Bind to vao
 //   glBindVertexArray(vao);

 //   // Bind vertices to vao
 //   glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
 //   glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
 //   glEnableVertexAttribArray(position_id);
 //   glBindBuffer(GL_ARRAY_BUFFER, 0);

 //   // Bind colors to vao
 //   glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
 //   glVertexAttribPointer(color_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
 //   glEnableVertexAttribArray(color_id);
 //   glBindBuffer(GL_ARRAY_BUFFER, 0);

 //   // Bind elements to vao
 //   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
 //   
 //   // Stop bind to vao
 //   glBindVertexArray(0);

 //   // Make uniform var
 //   uniform_mvp = glGetUniformLocation(shader_id, "mvp");

 //   // Fill uniform var
 //   glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
}

void InitObjects()
{
	// LIGHT
	light.position = glm::vec3(4.0, 4.0, 4.0);


	// TEAPOT
	glm::mat4 teapotMatrix = glm::mat4();
	Material matte = {
		glm::vec3(0.3, 0.3, 0.3),
		glm::vec3(0.5, 0.5, 0.0),
		glm::vec3(1.0),
		256,
		false
	};
	teapotMatrix = glm::translate(teapotMatrix, glm::vec3(0.6f, 2.8f, 0.0f));
	float scaleFactor = 3.0f;
	teapotMatrix = glm::scale(teapotMatrix, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
	models.emplace_back("objects/teapot.obj", "textures/XOndergrond.bmp", matte, teapotMatrix);

	// FLOOR
	glm::mat4 floorMatrix = glm::mat4();
	Material floor = {
		glm::vec3(0.3, 0.3, 0.0),
		glm::vec3(0.5, 0.5, 0.0),
		glm::vec3(1.0),
		128,
		true
	};
	floorMatrix = glm::translate(floorMatrix, glm::vec3(50.0f, -1.0f, 0.0f));
	floorMatrix = glm::scale(floorMatrix, glm::vec3(100.0f, 1.0f, 100.0f));
	models.emplace_back("objects/box.obj", "textures/XOndergrond.bmp", floor, floorMatrix);
}

int main(int argc, char ** argv)
{
    InitGlutGlew(argc, argv);
    InitShaders();
	InitObjects();
	InitMatrices();
    InitBuffers();

/*
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
*/
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_SHOW);

    glutMainLoop();

    return 0;
}