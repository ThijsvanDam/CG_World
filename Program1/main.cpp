#include <iostream>
#include <vector>
#include <map>

#include <string.h> 

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "glsl.h"
#include "objloader.hpp"
#include "texture.hpp"

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

struct Material {
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specular;
	float power;
	bool applied;
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

// This has to be last because it uses another type def.
struct Model {
	// Mesh variables
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec2> uvs;

	Material material;
	glm::mat4 model;
	glm::mat4 mv;
	GLuint vao;
	GLuint textureID;
};


//--------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------

GLuint program_id;
GLuint vao;
GLuint uniform_mvp;

glm::mat4 model, view, projection;
glm::mat4 mvp;


CameraMode cameraMode = CameraMode::View;
Camera camera;

float rightX = -camera.center.z;
float rightZ = camera.center.x;

struct ModelNotFoundException : public exception {
	const char * what() const throw () {
		return "ModelNotFoundException";
	}
};

class ModelHandler {
	std::map<string, Model> models;
	//Model models[30];
	int number = 0;

	ModelHandler() {

	}

	void addModel(string name, vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> uvs) {
		this->addModel(name, { vertices, normals, uvs});
	}

	void addModel(string name, Model model) {
		this->models[name] = model;
	}

	// Should be const chars, else it will error.
	void loadObject(string name, string objPath, string texturePath) {
		if (checkModel(name)) {
			Model model = models[name];
			bool res = loadOBJ(name.c_str(), model.vertices, model.uvs, model.normals);
			model.textureID = loadBMP(texturePath.c_str());
		}
	}

	void loadMaterialsLight(string name, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specular, int power, bool applied) {
		if (checkModel(name)) {
			Model model = models[name];
			model.material.ambientColor = ambientColor;
			model.material.diffuseColor = diffuseColor;
			model.material.specular = specular;
			model.material.power = power;
			model.material.applied = applied;
		}
	}

	int getModelCount() {
		return number + 1;
	}

	void printAll() {
		for (auto item : models) {
			cout << item.first;
		}
		cout << endl;
	}
private:
	bool checkModel(string name) {
		if (models.find(name) == models.end())
			return true;
		throw new ModelNotFoundException();
	}
};

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
    glUseProgram(program_id);
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
    glutTimerFunc(DELTA, Render, 1);

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

    program_id = glsl::makeShaderProgram(vsh_id, fsh_id);
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

void InitBuffers()
{
    GLuint position_id, color_id;
    GLuint vbo_vertices, vbo_colors;
    GLuint ibo_elements;

    // vbo for vertices
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // vbo for colors
    glGenBuffers(1, &vbo_colors);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // vbo for elements
    glGenBuffers(1, &ibo_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements),
        cube_elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Get vertex attributes
    position_id = glGetAttribLocation(program_id, "position");
    color_id = glGetAttribLocation(program_id, "color");

    // Allocate memory for vao
    glGenVertexArrays(1, &vao);

    // Bind to vao
    glBindVertexArray(vao);

    // Bind vertices to vao
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position_id);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Bind colors to vao
    glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
    glVertexAttribPointer(color_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(color_id);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Bind elements to vao
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    
    // Stop bind to vao
    glBindVertexArray(0);

    // Make uniform var
    uniform_mvp = glGetUniformLocation(program_id, "mvp");

    // Fill uniform var
    glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
}


int main(int argc, char ** argv)
{
    InitGlutGlew(argc, argv);
    InitShaders();
    InitMatrices();
	InitCameras(); // Init cameras
    InitBuffers();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_SHOW);

    glutMainLoop();

    return 0;
}
