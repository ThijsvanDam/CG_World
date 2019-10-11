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

const int WIDTH = 1200, HEIGHT = 800;
const char * fragshader_name = "fragmentshader.fsh";
const char * vertexshader_name = "vertexshader.vsh";
unsigned const int DELTA = 10;

/// These variables will change if the mouse moves
int mouseOriginX = 0;
int mouseOriginY = 0;

float mouseDeltaX = 0.0f;
float mouseDeltaY = 0.0f;

// These variables will change if the key state changes
float cameraEyeDeltaX = 0.0f, cameraEyeDeltaZ = 0.0f, cameraCenterDeltaY = 0.0f, cameraCenterDeltaX = 0.0f;

float mouseMovementSpeed = 0.01f;
float movementSpeed = 2.0f;

void switchCameraMode();

//--------------------------------------------------------------------------------
// Type definitions
//--------------------------------------------------------------------------------

struct LightSource {
	glm::vec3 position;
};

struct Camera
{
	glm::vec3 eye;
	glm::vec3 center;
	glm::vec3 up = { 0.0, 0.1, 0.0 };
} cameras[2];

enum class CameraMode
{
	View = 0, Walk = 1
};

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

CameraMode cameraMode = CameraMode::View;
Camera* camera;
LightSource light;
vector<Model> models;

float rightX;
float rightZ;


//--------------------------------------------------------------------------------
// Keyboard and mouse handling
//--------------------------------------------------------------------------------

void pressKeyHandler(unsigned char key, int a, int b)
{
	switch(key)
	{
	case 27:
		glutExit();

	case 'c':
		switchCameraMode();
		break;
	}

	float lookAroundSpeed = 0.01f;
	
	if(cameraMode == CameraMode::Walk)
	{
		switch (key)
		{
		case 'w': cameraEyeDeltaZ =  movementSpeed; break;
		case 's': cameraEyeDeltaZ = -movementSpeed; break;
		case 'd': cameraEyeDeltaX =  movementSpeed; break;
		case 'a': cameraEyeDeltaX = -movementSpeed; break;
		case 'i': cameraCenterDeltaY = -lookAroundSpeed; break;
		case 'k': cameraCenterDeltaY =  lookAroundSpeed; break;
		case 'l': cameraCenterDeltaX =  lookAroundSpeed; break;
		case 'j': cameraCenterDeltaX = -lookAroundSpeed; break;
		}
	}
}

void releaseKeyHandler(unsigned char key, int a, int b)
{
	switch (key) {
	case 'w':
	case 's': cameraEyeDeltaZ = 0.0f; 
		break;
	case 'd':
	case 'a': cameraEyeDeltaX = 0.0f; 
		break;
	case 'i':
	case 'k': cameraCenterDeltaY = 0.0f; 
		break;
	case 'l':
	case 'j': cameraCenterDeltaX = 0.0f; 
		break;
	}
}

void mouseHandler(int x, int y)
{
	if(cameraMode == CameraMode::Walk)
	{
		if (mouseOriginX >= 0)
		{
			mouseDeltaX = (x - mouseOriginX) * mouseMovementSpeed;
		}

		if (mouseOriginY >= 0)
		{
			mouseDeltaY = (y - mouseOriginY) * mouseMovementSpeed;
		}
		mouseOriginX = x;
		mouseOriginY = y;
	}
}

//--------------------------------------------------------------------------------
// Position and Camera Handling
//--------------------------------------------------------------------------------
void calculateCameraEye(float cameraEyeDeltaX, float cameraEyeDeltaZ) {
	float walkingSpeed = 0.1f;
	camera->eye.x += (cameraEyeDeltaZ * camera->center.x * walkingSpeed) + (cameraEyeDeltaX * rightX * walkingSpeed);
	camera->eye.z += (cameraEyeDeltaZ * camera->center.z * walkingSpeed) + (cameraEyeDeltaX * rightZ * walkingSpeed);
}

float calculatedAngleForCameraCenterX = 0.0f;
float calculatedAngleForCameraCenterY = 0.0f;

void calculateCameraCenter(float cameraCenterDeltaX, float cameraCenterDeltaY) {
	calculatedAngleForCameraCenterX += cameraCenterDeltaX + mouseDeltaX;
	calculatedAngleForCameraCenterY += cameraCenterDeltaY + mouseDeltaY;

	if (calculatedAngleForCameraCenterY >= 1.57f) calculatedAngleForCameraCenterY = 1.57f;
	if (calculatedAngleForCameraCenterY <= -1.57f) calculatedAngleForCameraCenterY = -1.57f;
	
	mouseDeltaX = 0;
	mouseDeltaY = 0;
	int a = 5;
	camera->center.x =  sin(calculatedAngleForCameraCenterX) * a;
	camera->center.y = -sin(calculatedAngleForCameraCenterY) * a;
	camera->center.z = -cos(calculatedAngleForCameraCenterX) * a;
	rightX = -camera->center.z;
	rightZ = camera->center.x;
}

void InitCameras() {

	// View camera definition, this one is static!
	glm::vec3  viewCameraPosition = { 20.0f, 20.0f, 80.0f };
	glm::vec3 viewCameraAngle = { 0.0f, -1.0f, -0.0f };
	cameras[int(CameraMode::View)] = { viewCameraPosition, viewCameraAngle };

	// Walk camera definition, this one is dynamic!
	glm::vec3 walkCameraPosition = { 0.0f, 2.0f, 6.0f };
	glm::vec3 walkCameraAngle = { 0.0f, -1.0f, 0.0f };
	cameras[int(CameraMode::Walk)] = { walkCameraPosition, walkCameraAngle };

	
	camera = &cameras[int(cameraMode)];


	rightZ= -camera->center.z;
	rightX= camera->center.x;
}

void switchCameraMode()
{
	cameraMode = bool(cameraMode) ? CameraMode::View : CameraMode::Walk;
	const string cCstring = bool(cameraMode) ? "view" : "walk";
	cout << "Switch camera to [" << cCstring << "] mode." << endl;
	camera = &cameras[int(cameraMode)];
} 



//--------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------
float z = 15.0;
float offset = 0.5;

void Render()
{
	if (cameraEyeDeltaX != 0.0f || cameraEyeDeltaZ != 0.0f)
		calculateCameraEye(cameraEyeDeltaX, cameraEyeDeltaZ); // #TODO: Movementspeed?

	if (cameraCenterDeltaX || cameraCenterDeltaY || mouseDeltaX || mouseDeltaY)
		calculateCameraCenter(cameraCenterDeltaX, cameraCenterDeltaY);

	// Reset transformations
	glLoadIdentity();

	
	// Set the camera
	view = glm::lookAt(
		camera->eye,
		// camera.center,
		glm::vec3(camera->eye.x + camera->center.x, camera->center.y, camera->eye.z + camera->center.z),
		camera->up
	);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Send mvp
	glUseProgram(shader_id);

    // Do transformation

	for (int i = 0; i < models.size(); ++i)
	{

		if (i == 0)
			models[i].model = glm::rotate(models[i].model, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
		// if(i==0 || i==1 || i==2)
		// {
		// 	models[i].model = glm::translate(models[i].model, glm::vec3(0.0f, 0.0f, 0.1f));
		// }
		//
	
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
	//
	// #TODO: DOES THIS EVEN MATTER THAT MUCH? DO WE EVEN NEED TO SET THIS?
	//
	// view = glm::lookAt(
	// 	camera->eye,
	// 	camera->center,
	// 	camera->up
	// );
	//
	//

	InitCameras();


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
	Material teapotMatrix_m = {
		glm::vec3(0.3, 0.3, 0.3),
		glm::vec3(0.5, 0.5, 0.0),
		glm::vec3(1.0),
		256,
		false
	};
	teapotMatrix = glm::translate(teapotMatrix, glm::vec3(0.6f, 2.8f, 0.0f));
	float scaleFactor = 3.0f;
	teapotMatrix = glm::scale(teapotMatrix, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
	models.emplace_back("objects/teapot.obj", "textures/XOndergrond.bmp", teapotMatrix_m, teapotMatrix);

	// FLOOR
	glm::mat4 floorMatrix = glm::mat4();
	Material floorMatrix_m = {
		glm::vec3(0.3, 0.3, 0.0),
		glm::vec3(0.5, 0.5, 0.0),
		glm::vec3(1.0),
		128,
		true
	};
	floorMatrix = glm::translate(floorMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
	floorMatrix = glm::scale(floorMatrix, glm::vec3(100.0f, 1.0f, 100.0f));
	models.emplace_back("objects/box.obj", "textures/Yellobrk.bmp", floorMatrix_m, floorMatrix);


	// BRAND_GATE
	glm::mat4 peperbus = glm::mat4();
	Material peperbus_m = {
		glm::vec3(0.3, 0.3, 0.3),
		glm::vec3(0.5, 0.5, 0.0),
		glm::vec3(1.0),
		128,
		false
	};
	float peperbus_s = 0.0001f;
	peperbus = glm::translate(peperbus, glm::vec3(0.0f, 10.8f, 0.0f));
	peperbus = glm::scale(floorMatrix, glm::vec3(peperbus_s, peperbus_s, peperbus_s));
	// models.emplace_back("objects/tower_house_design/Tower-House Design.obj", "textures/Yellobrk.bmp", brand_gate_m, brand_gate);
	models.emplace_back("objects/peperbus/peperbus.obj", "textures/XOndergrond.bmp", peperbus_m, peperbus);

	// // HIGHWAY
	glm::mat4 plane = glm::mat4();
	Material plane_m = {
		glm::vec3(0.3, 0.3, 0.3),
		glm::vec3(0.5, 0.5, 0.0),
		glm::vec3(1.0),
		128,
		false
	};
	plane = glm::translate(plane, glm::vec3(0.0f, 0.1f, 0.0f));
	plane = glm::rotate(plane, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	plane = glm::scale(plane, glm::vec3(1.0f, 1.0f, 10.0f));
	models.emplace_back("objects/plane/plane.obj", "objects/plane/Material_2.bmp", plane_m, plane);
	
	// STREET_LAMPS
	#pragma region STREET_LAMPS
	glm::mat4 lamp_starting_point = glm::mat4();
	lamp_starting_point = glm::translate(lamp_starting_point, glm::vec3(-30.0f, 0.0f, 0.0f));
	glm::mat4 street_lamp = lamp_starting_point;
	Material street_lamp_m = {
		glm::vec3(0.3, 0.3, 0.3),
		glm::vec3(0.5, 0.5, 0.0),
		glm::vec3(1.0),
		256,
		false
	};
	float sf = 100.0f;
	// street_lamp = glm::translate(street_lamp, glm::vec3(0.0f, 0.0f, -1000.0f));

	street_lamp = glm::rotate(street_lamp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	int lamp_count = 6;

	street_lamp = glm::scale(street_lamp, glm::vec3(sf, sf, sf));
	for (float i = 0; i < lamp_count; i++)
	{

		street_lamp = glm::translate(street_lamp, glm::vec3(0.05, 0.0f, 0.0f));
		models.emplace_back("objects/obj_pack/svet/svet_11.obj", "textures/XOndergrond.bmp", street_lamp_m, street_lamp);
	}

	street_lamp = lamp_starting_point;
	street_lamp = glm::rotate(street_lamp, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	street_lamp = glm::rotate(street_lamp, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	street_lamp = glm::translate(street_lamp, glm::vec3(-5.0f + (lamp_count * -5), 30.0f, 0.0f));
	street_lamp = glm::scale(street_lamp, glm::vec3(sf, sf, sf));
	
	for (float i = 0; i < lamp_count; i++)
	{

		street_lamp = glm::translate(street_lamp, glm::vec3(0.05, 0.0f, 0.0f));
		models.emplace_back("objects/obj_pack/svet/svet_11.obj", "textures/XOndergrond.bmp", street_lamp_m, street_lamp);
	}
	#pragma endregion
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