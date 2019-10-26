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

// 1. Nog een bewegend dinkie
// 2. De theepot fixen/een ander object
// 3. Camera begin

//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

const int WIDTH = 1200, HEIGHT = 800;
const char* fragshader_name = "fragmentshader.fsh";
const char* vertexshader_name = "vertexshader.vsh";
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

glm::mat4 view, projection;
glm::mat4 mvp;

CameraMode cameraMode = CameraMode::Walk;
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
	switch (key)
	{
	case 27:
		glutExit();

	case 'c':
		switchCameraMode();
		break;
	}

	float lookAroundSpeed = 0.01f;

	if (cameraMode == CameraMode::Walk)
	{
		switch (key)
		{
		case 'w': cameraEyeDeltaZ = movementSpeed; break;
		case 's': cameraEyeDeltaZ = -movementSpeed; break;
		case 'd': cameraEyeDeltaX = movementSpeed; break;
		case 'a': cameraEyeDeltaX = -movementSpeed; break;
		case 'i': cameraCenterDeltaY = -lookAroundSpeed; break;
		case 'k': cameraCenterDeltaY = lookAroundSpeed; break;
		case 'l': cameraCenterDeltaX = lookAroundSpeed; break;
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
	if (cameraMode == CameraMode::Walk)
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
	camera->center.x = sin(calculatedAngleForCameraCenterX) * a;
	camera->center.y = -sin(calculatedAngleForCameraCenterY) * a;
	camera->center.z = -cos(calculatedAngleForCameraCenterX) * a;
	rightX = -camera->center.z;
	rightZ = camera->center.x;
}

void InitCameras() {

	// View camera definition, this one is static!
	glm::vec3 viewCameraEye = { 40.0f, 25.0f, 40.0f };
	glm::vec3 viewCameraCenter = { -40.0f, -25.0f, -40.0f };
	cameras[int(CameraMode::View)] = { viewCameraEye, viewCameraCenter };

	// Walk camera definition, this one is dynamic!
	glm::vec3 walkCameraEye = { -70.0f, 3.5f,11.0f };
	glm::vec3 walkCameraCenter = { 5.0f, -0.1f, 0.1f };
	cameras[int(CameraMode::Walk)] = { walkCameraEye, walkCameraCenter };

	camera = &cameras[int(cameraMode)];


	rightZ = -camera->center.z;
	rightX = camera->center.x;
}

void printCamera(Camera camera)
{
	cout << "Center: [" << camera.center.x << ", " << camera.center.y << ", " << camera.center.z << "], Eye: [" << camera.eye.x << ", " << camera.eye.y << ", " << camera.eye.z << "]" << endl;
}

void switchCameraMode()
{
	cameraMode = bool(cameraMode) ? CameraMode::View : CameraMode::Walk;
	const string cCstring = bool(cameraMode) ? "view" : "walk";
	cout << "Switch camera to [" << cCstring << "] mode." << endl;

	camera = &cameras[int(cameraMode)];
	printCamera(*camera);
}



//--------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------
float z = 15.0;
float offset = 0.5;
int cnt = 0;

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
		camera->eye + camera->center,
		camera->up
	);
	printCamera(*camera);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Send mvp
	glUseProgram(shader_id);

	// Do transformation
	for (int i = 0; i < models.size(); ++i)
	{

		if (i == 0){
			if(cnt < 100)
			{
				models[i].model = glm::translate(models[i].model, glm::vec3(0.0f, 0.2f * cnt, 0.0f));
			}else if(cnt >= 100 && cnt < 2000)
			{
				models[i].model = glm::translate(models[i].model, glm::vec3(0.0f, 0.0f, (0.3f*(cnt - 100.0f))));
			}
		}
		// if(i==0 || i==1 || i==2)
		// {
		// 	models[i].model = glm::translate(models[i].model, glm::vec3(0.0f, 0.0f, 0.1f));
		// }
		//

		models[i].mv = view * models[i].model;

		glUniform1i(uniform_apply_texture, models[i].material.materialId);
		glBindTexture(GL_TEXTURE_2D, models[i].textureID);

		glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(models[i].mv));
		glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(models[i].material.ambientColor));
		glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(models[i].material.diffuseColor));
		glUniform3fv(uniform_material_specular, 1, glm::value_ptr(models[i].material.specular));
		glUniform1f(uniform_material_power, models[i].material.power);

		glBindVertexArray(models[i].vao);
		glDrawArrays(GL_TRIANGLES, 0, models[i].vertices.size());
		glBindVertexArray(0);
	}

	cnt += 1;
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

void InitGlutGlew(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutSetOption(GLUT_MULTISAMPLE, 8);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Thijs van Dam - S1078671");
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
	char* vertexshader = glsl::readFile(vertexshader_name);
	GLuint vsh_id = glsl::makeVertexShader(vertexshader);

	char* fragshader = glsl::readFile(fragshader_name);
	GLuint fsh_id = glsl::makeFragmentShader(fragshader);

	shader_id = glsl::makeShaderProgram(vsh_id, fsh_id);
}


//------------------------------------------------------------
// void InitMatrices()
//------------------------------------------------------------

void InitMatrices()
{

	InitCameras();


	view = glm::lookAt( // #TODO: Zet deze naar iets leuks
		glm::vec3(0.0, 2.0, 6.0),
		glm::vec3(1.5, 0.5, 0.0),
		glm::vec3(0.0, 1.0, 0.0)
	);
	projection = glm::perspective(
		glm::radians(45.0f),
		1.0f * WIDTH / HEIGHT, 0.1f,
		4000.0f);


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



	// o Some basic geometries are already present on ELO: box.obj, cylinder18.obj, cylinder32.obj, sphere.obj, torus.obj
	//	- Besides the already present.obj - files, use at least


	Material shiny = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(10.0f),
		128,
		1
	};

	Material matte = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		// glm::vec3(-0.3f, -0.3f, -0.3f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(0.0),
		128,
		0
	};

	// TEAPOT
	glm::mat4 helicopter = glm::mat4();
	float scaleFactor = .025f;
	helicopter = glm::translate(helicopter, glm::vec3(45.0f, 4.7f, 53.0f));
	helicopter = glm::scale(helicopter, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
	helicopter = glm::rotate(helicopter, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	models.emplace_back("objects/MI28.obj", "textures/MI28/mi28.bmp", matte, helicopter);

	// FLOOR
	glm::mat4 floor = glm::mat4();
	floor = glm::translate(floor, glm::vec3(0.0f, -1.0f, 0.0f));
	floor = glm::scale(floor, glm::vec3(200.0f, 1.0f, 200.0f));
	models.emplace_back("objects/box.obj", "textures/grass.bmp", matte, floor);

	// CREATED_MODEL_FLAT
	glm::mat4 flat = glm::mat4();
	flat = glm::rotate(flat, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	flat = glm::translate(flat, glm::vec3(-5.0f, 0.1f, 25.0f));
	// flat = glm::scale(flat, glm::vec3(.03f, .03f, .0f));
	models.emplace_back("objects/Uv-mapped-3ds-house-v2.obj", "textures/Uv-template-3ds-house_001.bmp", matte, flat);

	// LAKE
	glm::mat4 lake = glm::mat4();
	lake = glm::translate(lake, glm::vec3(-30.0f, -65.4667f, 90.0f));
	lake = glm::scale(lake, glm::vec3(0.03, 10.1f, 0.03));
	models.emplace_back("objects/lake/lake.obj", "textures/lake.bmp", shiny, lake);


	glm::mat4 skybox = glm::mat4();
	skybox = glm::translate(skybox, glm::vec3(0.0f, -30.0f, 0.0f));
	skybox = glm::scale(skybox, glm::vec3(1.5f, 1.5f, 1.5f));
	models.emplace_back("objects/skybox.obj", "textures/skybox_4.bmp", matte, skybox);


#pragma region Lamps_and_highway
	glm::mat4 lamp_and_planes_starting_point = glm::mat4();
	lamp_and_planes_starting_point = glm::translate(lamp_and_planes_starting_point, glm::vec3(-69.0f, 0.0f, 0.0f));

	int cnt = 22;

	int lamp_count = 1.75 * cnt;
	glm::mat4 lamp_starting_point = lamp_and_planes_starting_point;
	lamp_starting_point = glm::translate(lamp_starting_point, glm::vec3(-30.0f, 0.0f, 0.0f));

	int plane_count = cnt;
	glm::mat4 plane_starting_point = lamp_and_planes_starting_point;


	// STREET_LAMPS
#pragma region STREET_LAMPS
	glm::mat4 street_lamp = lamp_starting_point;
	float sf = 100.0f;
	// street_lamp = glm::translate(street_lamp, glm::vec3(0.0f, 0.0f, -1000.0f));

	street_lamp = glm::rotate(street_lamp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	street_lamp = glm::scale(street_lamp, glm::vec3(sf, sf, sf));
	for (float i = 0; i < lamp_count; i += 2)
	{

		street_lamp = glm::translate(street_lamp, glm::vec3(0.05 * 2, 0.0f, 0.0f));
		models.emplace_back("objects/obj_pack/svet/svet_11.obj", "textures/XOndergrond.bmp", shiny, street_lamp);
	}

	street_lamp = lamp_starting_point;
	street_lamp = glm::rotate(street_lamp, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	street_lamp = glm::rotate(street_lamp, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	street_lamp = glm::translate(street_lamp, glm::vec3(-5.0f + (lamp_count * -5), 30.0f, 0.0f));
	street_lamp = glm::scale(street_lamp, glm::vec3(sf, sf, sf));

	for (float i = 0; i < lamp_count; i += 2)
	{

		street_lamp = glm::translate(street_lamp, glm::vec3(0.05 * 2, 0.0f, 0.0f));
		models.emplace_back("objects/obj_pack/svet/svet_11.obj", "textures/XOndergrond.bmp", shiny, street_lamp);
	}
#pragma endregion

	// // HIGHWAY
#pragma region HIGHWAY
	glm::mat4 plane = plane_starting_point;
	plane = glm::translate(plane, glm::vec3(-30.0f, 0.1f, 29.5f));
	plane = glm::rotate(plane, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	plane = glm::scale(plane, glm::vec3(2.4f, 1.0f, 2.4f));
	for (int i = 0; i < plane_count; ++i)
	{
		plane = glm::translate(plane, glm::vec3(0.0f, 0.0f, 3.75f));
		models.emplace_back("objects/plane/plane.obj", "objects/plane/Material_2.bmp", matte, plane);
	}
#pragma endregion
#pragma endregion
}

int main(int argc, char** argv)
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