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
#define MESH_PLANE "models/plane.dae"


/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/



using namespace std;

GLuint shaderProgramID;

unsigned int mesh_vao = 0;
int width = 1250;
int height = 1250;


GLfloat rotate_x = 0.0f;
GLfloat rotate_y = 0.0f;
GLfloat rotate_z = 0.0f;

float upper_arm_length = 1.5f;
float lower_arm_length = 1.0f;
float total_arm_length = 2.5f;

glm::vec3 translate_upper_arm = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 translate_lower_arm1 = glm::vec3(-1.5f, 0.0f, 0.0f);
glm::vec3 translate_lower_arm2 = glm::vec3(-1.0f, 0.0f, 0.0f);
glm::vec3 translate_hand = glm::vec3(-1.0f, 0.0f, 0.0f);
glm::vec3 translate_end_of_chain = glm::vec3(-0.8f, 0.0f, 0.0f);




glm::vec3 rotate_upper_arm = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 rotate_lower_arm1 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 rotate_lower_arm2 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 rotate_hand = glm::vec3(0.0f, 0.0f, 0.0f);


glm::vec3 transform_target = glm::vec3(-4.0f, 0.0f, 0.0f);


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

	glm::quat quaternion = glm::quat(rotate_upper_arm);
	modelUpperArm = modelUpperArm * glm::toMat4(quaternion);




	// Update the appropriate uniform and draw the mesh again
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(modelUpperArm));
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);
}

void loadLowerArm(glm::mat4& modelParent, glm::mat4& modelLowerArm, int matrix_location, glm::vec3 translate_lower, glm::vec3 rotate_lower) {
	mesh_data = generateObjectBufferMesh(MESH_LOWER_ARM, shaderProgramID);
	// Set up the child matrix

	modelLowerArm = glm::mat4(1.0f);

	modelLowerArm = glm::translate(modelLowerArm, translate_lower);
	glm::quat quaternion = glm::quat(rotate_lower);
	modelLowerArm = modelLowerArm * glm::toMat4(quaternion);



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

	//modelHand = glm::rotate(modelHand, (rotate_hand.z), glm::vec3(0, 0, 1));
	//modelHand = glm::rotate(modelHand, (rotate_hand.y), glm::vec3(0, 1, 0));


	modelHand = glm::translate(modelHand, translate_hand);

	glm::quat quaternion = glm::quat(rotate_hand);
	modelHand = modelHand * glm::toMat4(quaternion);


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
	modelBody = glm::translate(modelBody, glm::vec3(1.0f, -1.0f, 0.0f));

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

void loadPlane(int matrix_location)
{
	mesh_data = generateObjectBufferMesh(MESH_PLANE, shaderProgramID);

	glm::mat4 modelPlane = glm::mat4(1.0f);

	modelPlane = glm::translate(modelPlane, glm::vec3(0.0f,-5.0f,0.0f));
	modelPlane = glm::scale(modelPlane, glm::vec3(5.0f, 5.0f, 5.0f));


	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(modelPlane));
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);
}



glm::vec3 view_translate = glm::vec3(0.0f, 0.0, -8.0f);
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


	if (CCD_mode == 2) {
		view = glm::rotate(view, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		view = glm::translate(view, glm::vec3(-8.0f, -8.0f, -8.0f));


	}
	else {
		view = glm::translate(view, view_translate);

	}


	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, glm::value_ptr(persp_proj));
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, glm::value_ptr(view));
	loadPlane(matrix_location);
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


void calcAngle(glm::vec3& rotation, int rotation_axis, glm::vec3& transformStart, glm::vec3& transformEnd)
{
	int transform_axis = 0;
	if (rotation_axis == 1) {
		transform_axis = 2;
	}

	float dist = glm::distance(transformStart, transformEnd);
	rotation[rotation_axis] = glm::acos((transformStart[transform_axis] - transformEnd[transform_axis]) / dist);
	
	int position_axis = 1;
	if (rotation_axis == 1) {
		position_axis = 0;
	}

	if (transformStart[position_axis] < transformEnd[position_axis]) {
		rotation[rotation_axis] *= -1.0f;
	}

}

float handLength = 0.8f;

//based off whats seen in this video: https://www.youtube.com/shorts/MvuO9ZHGr6k
//each frame a different link angle is calculated, this way no 'for' or 'while' loop is necessary and it results in a interpolated effect

void calcCCD(int& frame_number, int number_of_links, glm::vec3* linkGlobalTransforms[],int rotation_axis, float& rotateBy, glm::vec3 target_transform) {
	glm::vec3 handTransform(modelHand[3]);
	
	glm::vec3 endOfChainTransform(modelEndOfChain[3]);

	glm::vec3 endOfChainRotation;
	

	glm::vec3 targetRotation;
	calcAngle(targetRotation, rotation_axis, (*linkGlobalTransforms[frame_number]), target_transform);
	calcAngle(endOfChainRotation, rotation_axis, (*linkGlobalTransforms[frame_number]), endOfChainTransform);
	rotateBy = targetRotation[rotation_axis] - endOfChainRotation[rotation_axis];

	
}

void incrementFrameNumber(int& frame_number, int number_of_links)
{
	frame_number++;
	if (frame_number == number_of_links) {
		frame_number = 0;
	}
}

bool turnoff = false;
bool interpolate = false;
bool animate_target = false;

int interpolation_frames = 4;
int motionSpeed = 1;

int frame_number = 0;

int direc = 1;
float radius = 5.0f;
int radius_direc = 1;
int rotation_axis = 2;

bool Z_mode = false;


void updateScene() {

	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	if (CCD_mode == 2 && transform_target.x == -4.0f) {
		view_translate.z = -9.5f;
		transform_target.x = -5.0f;
	}

	if (CCD_mode == 0 && transform_target.x == -5.0f) {
		view_translate.z = -8.0f;
		transform_target.x = -4.0f;
	}
	
	if (animate_target) {

		animateTarget();

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
			

			float rotateBy;
			glm::vec3 handTransform(modelHand[3]);
			glm::vec3 lowerArmTransform1(modelLowerArm1[3]);
			glm::vec3 lowerArmTransform2(modelLowerArm2[3]);
			glm::vec3 upperArmTransform(modelUpperArm[3]);


			if (CCD_mode == 1) {
				float targetRotations[3] = {0.0f};
				glm::vec3* linkGlobalTransforms[] = { &handTransform ,&lowerArmTransform1, &upperArmTransform };
				glm::vec3* linkLocalRotations[] = { &rotate_hand, &rotate_lower_arm1, &rotate_upper_arm };

				calcCCD(frame_number, 3, linkGlobalTransforms,2, rotateBy, transform_target);
				if (!turnoff) {
					(*linkLocalRotations[frame_number])[2] += rotateBy;
				}



			}
			if (CCD_mode == 2) {
				float targetRotations[4] = { 0.0f };

				glm::vec3* linkGlobalTransforms[] = { &handTransform , &lowerArmTransform2, &lowerArmTransform1, &upperArmTransform };
				glm::vec3* linkLocalRotations[] = { &rotate_hand, &rotate_lower_arm2, &rotate_lower_arm1, &rotate_upper_arm };
				glm::vec3 endOfChainPos(modelEndOfChain[3]);

				
				rotation_axis = 2;
				calcCCD(frame_number, 4, linkGlobalTransforms, rotation_axis, rotateBy, transform_target);

				if (!turnoff) {
					(*linkLocalRotations[frame_number])[rotation_axis] += rotateBy;

				}
				if (Z_mode) {
					calcCCD(frame_number, 4, linkGlobalTransforms, 1, rotateBy, transform_target);

					if (!turnoff) {
						(*linkLocalRotations[frame_number])[1] += rotateBy;

					}
				}
				
				

			}
			
			
			frame_number++;
			if (frame_number == (CCD_mode + 2)) {
				frame_number = 0;

			}
			
		}
		


	}
	frame_count++;


	// Draw the next frame
	glutPostRedisplay();
}

//press 'o' to activate
glm::vec3 animationPath[] = { glm::vec3(-5.0f,0,0), glm::vec3(0,4.0f,0), glm::vec3(4.0f,2.0f, 0.0f), glm::vec3(3.0f,-4.0f,0.0f),glm::vec3(-2.0f,1.0f,0.0f),glm::vec3(-5.0f,0,0) };
int spline_index = 1;
float curveX= 0.0f;
float curveY = 0.0f;
int y_direc = 1;
int x_direc = 1;
float prevY = 0.0f;
float prevX = 0.0f;
float lenX = 0.0f;
float lenY = 0.0f;

//parametric equation to join splines found here : https://math.stackexchange.com/questions/121720/ease-in-out-function/121755#121755
void animateTarget()
{
	
	curveY = pow(curveX,2) / (2.0f * (pow(curveX, 2) - curveX) + 1.0f);

	lenX = (animationPath[spline_index].x - animationPath[spline_index - 1].x);
	lenY = (animationPath[spline_index].y - animationPath[spline_index - 1].y);

	if (animationPath[spline_index].y > animationPath[spline_index - 1].y) {
		transform_target.y = curveY * lenY + animationPath[spline_index-1].y;
	}
	else {
		transform_target.y = (1-curveY) * -lenY + animationPath[spline_index].y;
	}
	
	/*
	if (splinePath[spline_index].x > splinePath[spline_index - 1].x ) {
		transform_target.x += (curveX-prevX) * lenX;
	}
	else {
		transform_target.x -= (curveX - prevX) * lenX;
	}
	*/
	
	transform_target.x += 0.0015f * lenX;
	curveX += 0.0015f;

	if (abs(glm::distance(transform_target,animationPath[spline_index])) < 0.005f ) {
		transform_target = animationPath[spline_index];
		spline_index++;
		
		std::cout << "TARGET REACHED" << sizeof(animationPath)/ sizeof(animationPath[0]) << std::endl;
		std::cout << transform_target.x << " " << transform_target.y << std::endl;
		if (animationPath[spline_index-1].x==-5.0f) {
			animate_target = false;
		}
		curveX = 0.0f;
		curveY = 0.0f;
	}

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
	if (key == 'i') {
		if (!interpolate) {
			interpolate = true;
		}
		else {
			interpolate = false;
		}
	}

	if (key == 'm') {
		motionSpeed += 1.0f;
		std::cout << motionSpeed << std::endl;
	}
	if (key == 'n') {
		motionSpeed = 1.0f;
	}
	if (key == 'z') {
		Z_mode = true;
	}

	if (key == 'o') {
		
		transform_target = animationPath[0];
		if (animate_target) {
			animate_target = false;
		}
		else {
			animate_target = true;
		}
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
