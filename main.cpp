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
#include <map>

#include "utils/mesh.h"
#include "utils/shaders.h"
#include "utils/vbo.h"
#include "main.h"


#include <numeric>


/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
#define MESH_BODY "models/bodyAndhead.dae"
#define MESH_UPPER_ARM "models/upperArm.dae"
#define MESH_LOWER_ARM "models/lowerArm.dae"
#define MESH_HAND "models/hand.dae"
#define MESH_TARGET "models/target.dae"

/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/



using namespace std;

GLuint shaderProgramID;

unsigned int mesh_vao = 0;
int width = 1350;
int height = 1350;


GLfloat rotate_x = 0.0f;
GLfloat rotate_y = 0.0f;
GLfloat rotate_z = 0.0f;

float upper_arm_length = 1.5f;
float lower_arm_length = 1.0f;
float total_arm_length = 2.5f;

glm::vec3 translate_upper_arm = glm::vec3(-1.0f, 0.0f, 0.0f);
glm::vec3 translate_lower_arm1 = glm::vec3(-1.5f, 0.0f, 0.0f);
glm::vec3 translate_lower_arm2 = glm::vec3(-1.0f, 0.0f, 0.0f);
glm::vec3 translate_hand = glm::vec3(-1.0f, 0.0f, 0.0f);
glm::vec3 translate_end_of_chain = glm::vec3(-0.8f, 0.0f, 0.0f);




glm::vec3 rotate_upper_arm = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 rotate_lower_arm1 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 rotate_lower_arm2 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 rotate_hand = glm::vec3(0.0f, 0.0f, 0.0f);


glm::vec3 transform_target = glm::vec3(-5.0f, 0.0f, 0.0f);


ModelData mesh_data;

int CCD_mode = 0;
int frame_count = 0;

glm::mat4 modelUpperArm;
glm::mat4 modelLowerArm1;
glm::mat4 modelLowerArm2;
glm::mat4 modelHand;
glm::mat4 modelEndOfChain;

void loadUpperArm(glm::mat4& modelUpperArm, int matrix_location)
{
	mesh_data = generateObjectBufferMesh(MESH_UPPER_ARM, shaderProgramID);
	// Set up the child matrix
	modelUpperArm = glm::mat4(1.0f);
	modelUpperArm = glm::translate(modelUpperArm, translate_upper_arm);

	modelUpperArm = glm::rotate(modelUpperArm, rotate_upper_arm.z, glm::vec3(0, 0, 1));



	// Update the appropriate uniform and draw the mesh again
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(modelUpperArm));
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);
}

void loadLowerArm(glm::mat4& modelParent, glm::mat4& modelLowerArm, int matrix_location, glm::vec3 translate_lower, glm::vec3 rotate_lower) {
	mesh_data = generateObjectBufferMesh(MESH_LOWER_ARM, shaderProgramID);
	// Set up the child matrix

	modelLowerArm = glm::mat4(1.0f);

	modelLowerArm = glm::translate(modelLowerArm, translate_lower);
	modelLowerArm = glm::rotate(modelLowerArm, (rotate_lower.z), glm::vec3(0, 0, 1));


	//modelUpperArm = glm::rotate(modelUpperArm, glm::radians(10.f), glm::vec3(0, 0, 1));

	// Apply the root matrix to the child matrix
	modelLowerArm = modelParent * modelLowerArm;

	// Update the appropriate uniform and draw the mesh again
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(modelLowerArm));
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);
}

void loadHand(glm::mat4& modelLowerArm, glm::mat4& modelHand, int matrix_location) {
	mesh_data = generateObjectBufferMesh(MESH_HAND, shaderProgramID);
	// Set up the child matrix

	modelHand = glm::mat4(1.0f);
	modelEndOfChain = glm::mat4(1.0f);

	modelHand = glm::translate(modelHand, translate_hand);
	modelHand = glm::rotate(modelHand, (rotate_hand.z), glm::vec3(0, 0, 1));


	//modelUpperArm = glm::rotate(modelUpperArm, glm::radians(10.f), glm::vec3(0, 0, 1));

	// Apply the root matrix to the child matrix
	modelHand = modelLowerArm * modelHand;

	modelEndOfChain = glm::translate(modelEndOfChain, translate_end_of_chain);
	modelEndOfChain = modelHand * modelEndOfChain;


	// Update the appropriate uniform and draw the mesh again
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(modelHand));
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);

}

void loadBody(glm::mat4& modelBody, int matrix_location)
{
	mesh_data = generateObjectBufferMesh(MESH_BODY, shaderProgramID);

	modelBody = glm::mat4(1.0f);
	modelBody = glm::translate(modelBody, glm::vec3(0.0f, -1.0f, 0.0f));

	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(modelBody));
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);
}

void loadTarget(int matrix_location)
{
	mesh_data = generateObjectBufferMesh(MESH_TARGET, shaderProgramID);

	glm::mat4 modelTarget = glm::mat4(1.0f);

	modelTarget = glm::translate(modelTarget, transform_target);

	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(modelTarget));
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);
}



glm::vec3 view_translate = glm::vec3(1.0f, 0.0, -8.0f);
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



	view = glm::translate(view, glm::vec3(1.0f, 0.0, -9.5f));

	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, glm::value_ptr(persp_proj));
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, glm::value_ptr(view));

	loadTarget(matrix_location);

	if (CCD_mode == 0) {
		glm::mat4 modelBody;
		loadBody(modelBody, matrix_location);
	}



	loadUpperArm(modelUpperArm, matrix_location);

	loadLowerArm(modelUpperArm, modelLowerArm1, matrix_location, translate_lower_arm1, rotate_lower_arm1);

	if (CCD_mode == 2) {

		loadLowerArm(modelLowerArm1, modelLowerArm2, matrix_location, translate_lower_arm2, rotate_lower_arm2);
		loadHand(modelLowerArm2, modelHand, matrix_location);

	}

	if (CCD_mode == 1) {
		loadHand(modelLowerArm1, modelHand, matrix_location);
	}


	glutSwapBuffers();
}
int i = 0;
void calcIKUnreachable() {
	rotate_hand.z = 0.0f;
	rotate_lower_arm1.z = 0.0f;



	if (transform_target.y > translate_upper_arm.y) {
		rotate_upper_arm.z = -glm::acos((translate_upper_arm.x - transform_target.x) / glm::distance(translate_upper_arm, transform_target));

	}
	else {
		rotate_upper_arm.z = glm::acos((translate_upper_arm.x - transform_target.x) / glm::distance(translate_upper_arm, transform_target));

	}

}

void calcIKAnalytical() {
	float x = translate_upper_arm.x - transform_target.x;
	float y = translate_upper_arm.y - transform_target.y;
	float dist = glm::distance(transform_target, translate_upper_arm);
	float thetaT = glm::acos(x / dist);

	//upper arm angle
	float numerator = (pow(upper_arm_length, 2) + pow(x, 2) + pow(y, 2) - pow(lower_arm_length, 2));
	float denominator = (2.0f * upper_arm_length * dist);

	rotate_upper_arm.z = glm::acos(numerator / denominator) + thetaT;

	//lower arm angle
	numerator = pow(upper_arm_length, 2) + pow(lower_arm_length, 2) - pow(x, 2) - pow(y, 2);
	denominator = 2 * upper_arm_length * lower_arm_length;

	rotate_lower_arm1.z = glm::acos(numerator / denominator) + glm::radians(180.0f);

}


void calcAngle(float& rotation, glm::vec3& transformStart, glm::vec3& transformEnd)
{
	float dist = glm::distance(transformStart, transformEnd);
	rotation = glm::acos((transformStart.x - transformEnd.x) / dist);

	if (transformStart.y < transformEnd.y) {
		rotation *= -1.0f;
	}

}

float handLength = 0.8f;

//based off whats seen in this video: https://www.youtube.com/shorts/MvuO9ZHGr6k
//each frame a different link angle is calculated, this way no 'for' or 'while' loop is necessary and it results in a interpolated effect

void calcCCD(int& frame_number, int number_of_links, glm::vec3* linkGlobalTransforms[], glm::vec3* linkLocalRotations[]) {
	glm::vec3 handTransform(modelHand[3]);
	glm::vec3 target_transform = transform_target;
	glm::vec3 endOfChainTransform(modelEndOfChain[3]);

	float endOfChainRotation = 0.0f;
	float rotateBy;

	float targetRotation;
	calcAngle(targetRotation, (*linkGlobalTransforms[frame_number]), target_transform);
	calcAngle(endOfChainRotation, (*linkGlobalTransforms[frame_number]), endOfChainTransform);
	rotateBy = targetRotation - endOfChainRotation;

	(*linkLocalRotations[frame_number]).z += rotateBy;

}

void incrementFrameNumber(int& frame_number, int number_of_links)
{
	frame_number++;
	if (frame_number == number_of_links) {
		frame_number = 0;
	}
}

bool turnoff = false;
int motionSpeed = 1;

int frame_number = 0;


void updateScene() {

	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	if (CCD_mode == 2 && transform_target.x == -5.0f) {
		view_translate.z = -9.5f;
		transform_target.x = -6.0f;
	}
	if (CCD_mode == 0 && transform_target.x == -6.0f) {
		view_translate.z = -8.0f;
		transform_target.x = -5.0f;
	}


	if (CCD_mode == 0) {
		if (glm::distance(translate_upper_arm, transform_target) >= total_arm_length) {
			calcIKUnreachable();
		}
		else {
			calcIKAnalytical();
		}
	}
	else {
		if (frame_count % motionSpeed == 0) {
			if (!turnoff) {
				glm::vec3 handTransform(modelHand[3]);
				glm::vec3 lowerArmTransform1(modelLowerArm1[3]);
				glm::vec3 lowerArmTransform2(modelLowerArm2[3]);
				glm::vec3 upperArmTransform(modelUpperArm[3]);


				if (CCD_mode == 1) {
					glm::vec3* linkGlobalTransforms[] = { &handTransform ,&lowerArmTransform1, &upperArmTransform };
					glm::vec3* linkLocalRotations[] = { &rotate_hand, &rotate_lower_arm1, &rotate_upper_arm };

					calcCCD(frame_number, 3, linkGlobalTransforms, linkLocalRotations);


				}
				if (CCD_mode == 2) {
					glm::vec3* linkGlobalTransforms[] = { &handTransform , &lowerArmTransform2, &lowerArmTransform1, &upperArmTransform };
					glm::vec3* linkLocalRotations[] = { &rotate_hand, &rotate_lower_arm2, &rotate_lower_arm1, &rotate_upper_arm };

					calcCCD(frame_number, 4, linkGlobalTransforms, linkLocalRotations);

				}
			}

			frame_number++;
			if (frame_number == CCD_mode + 2) {
				frame_number = 0;
			}
		}
		else {
			//add interpolations and target rotations here
		}


	}
	frame_count++;


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
		transform_target.y += translate_speed;
	}
	if (key == 'a') {
		transform_target.x -= translate_speed;
	}
	if (key == 's') {
		transform_target.y -= translate_speed;
	}
	if (key == 'd') {
		transform_target.x += translate_speed;
	}
	if (key == 'q') {
		transform_target.z -= translate_speed;
	}
	if (key == 'e') {
		transform_target.z += translate_speed;
	}

	if (key == ' ') {
		if (CCD_mode == 2) {
			CCD_mode = 0;
		}
		else {
			CCD_mode++;
		}
	}

	if (key == 'p') {
		if (!turnoff) {
			turnoff = true;
		}
		else {
			turnoff = false;
		}
	}

	if (key == 'm') {
		motionSpeed += 1.0f;
		std::cout << motionSpeed << std::endl;
	}
	if (key == 'n') {
		motionSpeed = 1.0f;
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
