// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "glm/gtx/string_cast.hpp"

#include <cmath>

#include "utils/mesh.h"
#include "utils/shaders.h"
#include "utils/vbo.h"



/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
#define MESH_BODY "models/bodyAndhead.dae"
#define MESH_UPPER_ARM "models/upperArm.dae"
#define MESH_LOWER_ARM "models/lowerArm.dae"
#define MESH_TARGET "models/target.dae"

/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/



using namespace std;

GLuint shaderProgramID;

unsigned int mesh_vao = 0;
int width = 800;
int height = 600;


GLfloat rotate_x = 0.0f;
GLfloat rotate_y = 0.0f;
GLfloat rotate_z = 0.0f;

float upper_arm_length = 1.5f;
float lower_arm_length = 1.0f;
float total_arm_length = 2.5f;

glm::vec3 translate_upper_arm = glm::vec3(-1.0f, 1.0f, 0.0f);
glm::vec3 translate_lower_arm = glm::vec3(-1.5f, 0.0f, 0.0f);


glm::vec3 rotate_upper_arm = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 rotate_lower_arm = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 translate_target = glm::vec3(-4.0f, 1.0f, 0.0f);


ModelData mesh_data;

void loadUpperArm(glm::mat4& modelUpperArm, glm::mat4& modelBody, int matrix_location)
{
	mesh_data = generateObjectBufferMesh(MESH_UPPER_ARM, shaderProgramID);
	// Set up the child matrix
	modelUpperArm = glm::mat4(1.0f);
	modelUpperArm = glm::translate(modelUpperArm, translate_upper_arm);

	modelUpperArm = glm::rotate(modelUpperArm, rotate_upper_arm.z, glm::vec3(0, 0, 1));



	// Apply the root matrix to the child matrix
	modelUpperArm = modelBody * modelUpperArm;

	// Update the appropriate uniform and draw the mesh again
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(modelUpperArm));
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);
}

void loadLowerArm(glm::mat4& modelUpperArm, int matrix_location) {
	mesh_data = generateObjectBufferMesh(MESH_LOWER_ARM, shaderProgramID);
	// Set up the child matrix
	glm::mat4 modelLowerArm = glm::mat4(1.0f);
	

	modelLowerArm = glm::translate(modelLowerArm, translate_lower_arm);
	modelLowerArm = glm::rotate(modelLowerArm, (rotate_lower_arm.z), glm::vec3(0, 0, 1));


	//modelUpperArm = glm::rotate(modelUpperArm, glm::radians(10.f), glm::vec3(0, 0, 1));

	// Apply the root matrix to the child matrix
	modelLowerArm = modelUpperArm * modelLowerArm;

	// Update the appropriate uniform and draw the mesh again
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(modelLowerArm));
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);
}

void loadBody(glm::mat4& modelBody, int matrix_location)
{
	mesh_data = generateObjectBufferMesh(MESH_BODY, shaderProgramID);

	modelBody = glm::mat4(1.0f);
	
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(modelBody));
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);
}

void loadTarget(int matrix_location)
{
	mesh_data = generateObjectBufferMesh(MESH_TARGET, shaderProgramID);

	glm::mat4 modelTarget = glm::mat4(1.0f);

	modelTarget = glm::translate(modelTarget, translate_target);

	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(modelTarget));
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);
}

void display() {

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	// load mesh into a vertex buffer array

	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");


	// Root of the Hierarchy
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 persp_proj = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	
	view = glm::translate(view, glm::vec3(2.0f, 0.0, -7.0f));

	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, glm::value_ptr(persp_proj));
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, glm::value_ptr(view));

	loadTarget(matrix_location);

	glm::mat4 modelPlane;
	loadBody(modelPlane, matrix_location);

	glm::mat4 modelUpperArm;
	loadUpperArm(modelUpperArm, modelPlane, matrix_location);

	loadLowerArm(modelUpperArm, matrix_location);


	glutSwapBuffers();
}
int i = 0;
void calcIKUnreachable() {
	
	rotate_lower_arm.z = 0.0f;
	rotate_upper_arm.z = glm::acos((translate_upper_arm.x-translate_target.x)/ glm::distance(translate_upper_arm, translate_target));

	
	if (i % 100 == 0) {
		std::cout << glm::distance(translate_upper_arm, translate_target) << std::endl;
	}
}

void calcIKReachable() {
	float x = translate_upper_arm.x - translate_target.x;
	float y = translate_upper_arm.y - translate_target.y;
	float dist = glm::distance(translate_target, translate_upper_arm);
	float thetaT = glm::acos(x / dist);

	//upper arm angle
	float numerator = (pow(upper_arm_length, 2) + pow(x, 2) + pow(y, 2) - pow(lower_arm_length, 2));
	float denominator = (2.0f * upper_arm_length * dist);

	rotate_upper_arm.z = glm::acos(numerator / denominator) + thetaT;

	//lower arm angle
	numerator = pow(upper_arm_length, 2) + pow(lower_arm_length, 2) - pow(x, 2) - pow(y, 2);
	denominator = 2 * upper_arm_length * lower_arm_length;

	rotate_lower_arm.z = glm::acos(numerator/denominator) + glm::radians(180.0f);
	
	if (i % 100 == 0) {
		std::cout << rotate_upper_arm.z << std::endl;
	}
}

float direction = -1.0f;
void updateScene() {

	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	if (glm::distance(translate_upper_arm, translate_target) >= total_arm_length) {
		calcIKUnreachable();
	}
	else {
		calcIKReachable();
	}

	i++;
	// Draw the next frame
	glutPostRedisplay();
}




void init()
{
	// Set up the shaders
	shaderProgramID = CompileShaders();
	
}

// Placeholder code for the keypress
float translate_speed = 0.1f;
void keypress(unsigned char key, int x, int y) {
	if (key == 'w') {
		translate_target.y += translate_speed;
	}
	if (key == 'a') {
		translate_target.x -= translate_speed;
	}
	if (key == 's') {
		translate_target.y -= translate_speed;
	}
	if (key == 'd') {
		translate_target.x += translate_speed;
	}
	
}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Inverse Kinematics");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}
