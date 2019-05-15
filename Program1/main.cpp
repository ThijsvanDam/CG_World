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
#include "ModelHandler.h"
#include "Model.cpp"

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

//------------------------------------------------------------
// Variables for object
//
//           7----------6
//          /|         /|
//         / |        / |
//        /  4-------/--5               y
//       /  /       /  /                |
//      3----------2  /                 ----x
//      | /        | /                 /
//      |/         |/                  z
//      0----------1
//------------------------------------------------------------

const GLfloat vertices[] = {
    // front
    -1.0, -1.0, 1.0,
    1.0, -1.0, 1.0,
    1.0, 1.0, 1.0,
    -1.0, 1.0, 1.0,
    // back
    -1.0, -1.0, -1.0,
    1.0, -1.0, -1.0,
    1.0, 1.0, -1.0,
    -1.0, 1.0, -1.0,
};

const GLfloat colors[] = {
    // front colors
    1.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
    1.0, 1.0, 1.0,
    // back colors
    0.0, 1.0, 1.0,
    1.0, 0.0, 1.0,
    1.0, 0.0, 0.0,
    1.0, 1.0, 0.0,
};

GLushort cube_elements[] = {
    0,1,1,2,2,3,3,0,  // front
    0,4,1,5,3,7,2,6,  // front to back
    4,5,5,6,6,7,7,4   //back
};


/// These variables will change if the mouse moves
float mouseOriginX = -1;
float mouseOriginY = -1;

float mouseDeltaX = 0.0f;
float mouseDeltaY = 0.0f;

// These variables will change if the key state changes
float cameraEyeDeltaX = 0.0f, cameraEyeDeltaZ = 0.0f, cameraCenterDeltaY = 0.0f, cameraCenterDeltaX = 0.0f;

float mouseMovementSpeed = 0.2f;
float movementSpeed = 0.5f;
float lookAroundSpeed = .1f;

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
GLuint uniform_mvp;

glm::mat4 model, view, projection;
glm::mat4 mvp;

CameraMode cameraMode = CameraMode::View;
Camera camera;
LightSource light;
ModelHandler model_handler = ModelHandler();

float rightX = -camera.center.z;
float rightZ = camera.center.x;


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
		cout << "Switch camera's " << endl;
		switchCameraMode();
		break;
	}

	if(cameraMode == CameraMode::Walk)
	{
		switch (key)
		{
		case 'w': cameraEyeDeltaZ =  movementSpeed; break;
		case 's': cameraEyeDeltaZ = -movementSpeed; break;
		case 'd': cameraEyeDeltaX =  movementSpeed; break;
		case 'a': cameraEyeDeltaX = -movementSpeed; break;
		case 'i': cameraCenterDeltaY =  lookAroundSpeed; break;
		case 'k': cameraCenterDeltaY = -lookAroundSpeed; break;
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
			mouseDeltaY = (y - mouseOriginY) * -mouseMovementSpeed;
		}
		mouseOriginX = x;
		mouseOriginY = y;
	}
}

//--------------------------------------------------------------------------------
// Position and Camera Handling
//--------------------------------------------------------------------------------
void calculateCameraEye(float cameraEyeDeltaX, float cameraEyeDeltaZ) {
	// Only the X and the Y will be used for 
	camera.eye.x += (cameraEyeDeltaZ * camera.center.x * 0.2f) + (cameraEyeDeltaX * rightX * 0.2f);
	camera.eye.z += (cameraEyeDeltaZ * camera.center.z * 0.2f) + (cameraEyeDeltaX * rightZ * 0.2f);
}

float calculatedAngleForCameraCenterX = 0.0f;
float calculatedAngleForCameraCenterY = 0.0f;

void calculateCameraCenter(float cameraCenterDeltaX, float cameraCenterDeltaY) {
	calculatedAngleForCameraCenterX += cameraCenterDeltaX + mouseDeltaX;
	calculatedAngleForCameraCenterY += cameraCenterDeltaY + mouseDeltaY;
	mouseDeltaX = 0;
	mouseDeltaY = 0;
	camera.center.x =  sin(calculatedAngleForCameraCenterX);
	camera.center.y =  sin(calculatedAngleForCameraCenterY);
	camera.center.z = -cos(calculatedAngleForCameraCenterX);
	rightX = -camera.center.z;
	rightZ = camera.center.x;
}

void InitCameras() {

	// View camera definition, this one is static!
	glm::vec3  viewCameraPosition = { 2.0f, 2.0f, 8.0f };
	glm::vec3 viewCameraAngle = { 0.0f, -0.0f, -0.0f };
	cameras[int(CameraMode::View)] = { viewCameraPosition, viewCameraAngle };

	// Walk camera definition, this one is dynamic!
	glm::vec3 walkCameraPosition = { 0.0f, 0.0f, 0.0f };
	glm::vec3 walkCameraAngle = { 0.0f, 0.0f, 1.75f };
	cameras[int(CameraMode::Walk)] = { walkCameraPosition, walkCameraAngle };

	camera = cameras[int(cameraMode)];
}

void switchCameraMode()
{
	cameraMode = bool(cameraMode) ? CameraMode::View : CameraMode::Walk;
	camera = cameras[int(cameraMode)];
} 



//--------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------

float z = 15.0;
float offset = 0.5;
void Render()
{
	if (cameraEyeDeltaX || cameraEyeDeltaZ)
		calculateCameraEye(cameraEyeDeltaX, cameraEyeDeltaZ);

	if (cameraCenterDeltaX || cameraCenterDeltaY || mouseDeltaX || mouseDeltaY)
			calculateCameraCenter(cameraCenterDeltaX, cameraCenterDeltaY);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Send vao
    glBindVertexArray(vao);
    glDrawElements(GL_LINES, sizeof(cube_elements) / sizeof(GLushort),
        GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

	view = glm::lookAt(
		camera.eye,
		glm::vec3(camera.eye.x + camera.center.x, camera.center.y, camera.eye.z + camera.center.z),
		camera.up
	);


    // Do transformation
    model = glm::rotate(model, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
    mvp = projection * view * model;

    // Send mvp
    glUseProgram(shader_id);
    glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

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
    model = glm::mat4();
	/*
    view = glm::lookAt(
        glm::vec3(2.0, 2.0, 7.0),
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0));*/

    projection = glm::perspective(
        glm::radians(45.0f),
        1.0f * WIDTH / HEIGHT, 0.1f,
        20.0f);
    mvp = projection * view * model;
}


//------------------------------------------------------------
// void InitBuffers()
// Allocates and fills buffers
//------------------------------------------------------------
GLuint uniform_mv;
GLuint uniform_apply_texture;
GLuint uniform_material_ambient;
GLuint uniform_material_diffuse;
GLuint uniform_material_specular;
GLuint uniform_material_power;

void InitBuffers()
{
	GLuint vbo_vertices, vbo_normals, vbo_uvs;

	GLuint position_id = glGetAttribLocation(shader_id, "position");
	GLuint normal_id = glGetAttribLocation(shader_id, "normal");
	GLuint uv_id = glGetAttribLocation(shader_id, "uv");

	// Attrach the program from shader_id so we can set uniform vars
	glUseProgram(shader_id);

	// Create and set UVs (uniform vars)
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

	for(std::map<string, Model>::iterator modelMap = model_handler.getModelsIterator(); 
		modelMap != model_handler.getLastModelIterator();
		modelMap++)
	{
		Model model = modelMap->second;
		// vbo for vertices
		glGenBuffers(1, &vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, model_handler.getModelCount() * sizeof(glm::vec3),
			&model.vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vbo for normals
		glGenBuffers(1, &vbo_normals);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
		glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(glm::vec3),
			&model.normals[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vbo for uvs
		glGenBuffers(1, &vbo_uvs);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
		glBufferData(GL_ARRAY_BUFFER, model.uvs.size() * sizeof(glm::vec2),
			&model.uvs[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &model.vao);
		glBindVertexArray(model.vao);

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
	light.position = glm::vec3(120.0, 120.0, 120.0);

	string oc = "Old_cottage";
	model_handler.initModel(oc);
	/*Model* m = model_handler.getModel(oc);
	string a = "objects/sphere.obj";
	bool res = loadOBJ(a.c_str(), m->vertices, m->uvs, m->normals);*/

	model_handler.loadObject(oc, "objects/teapot.obj", "textures/Yellobrk.bmp");
	model_handler.loadMaterialsLight(
		oc,
		glm::vec3(0.6, 0.6, 0.6),
		glm::vec3(0.6, 0.6, 0.6),
		glm::vec3(0.5),
		128,
		true
	);
}

int main(int argc, char ** argv)
{
    InitGlutGlew(argc, argv);
    InitShaders();
    InitMatrices();
	InitObjects();
	InitCameras(); // Init cameras
    InitBuffers();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_SHOW);

    glutMainLoop();

    return 0;
}
