/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#define GLM_FORCE_RADIANS
#define NUMOFPARTS 14
#define MAXCOUNTER 140
#define MAXCURVECOUNTER 20

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"
#include "Object.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
using namespace std;
using namespace glm;

float speed_x=0; //angular speed in radians
float speed_y=0; //angular speed in radians
float extraSpeed = 0;
float cameraSpeed = 0;
quat O = quat(vec3(0, 0, 0));
vec3 pos = vec3(0, 0, 0);
float aspectRatio=1;
ShaderProgram *sp; //Pointer to the shader program
int counter = 0;
int curveCounter = 0;
float cameraDistance = 5.0f;
float cameraHeight = 3.0f;
float angle_y = 0;
float extraAngle = 0;
float sumAngle = 0;
int autoWalking = 0;


//Uncomment to draw a cube
vector <Object> parts;
Object snowObject;
Object outerSphere;
Object mountainsInner;
Object mountainsOuter;
Object damaged1;
Object damaged2;

GLuint tex0;
GLuint tex1;
GLuint snow0;
GLuint snow1;
GLuint sky0;
GLuint black;
GLuint rocks;


//Error processing callback procedure
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_y = 0.4f;
		if (key == GLFW_KEY_RIGHT) speed_y = -0.4f;
		if (key == GLFW_KEY_UP) speed_x = 0.4f;
		if (key == GLFW_KEY_A) extraSpeed = -1.0f;
		if (key == GLFW_KEY_D) extraSpeed = 1.0f;
		if (key == GLFW_KEY_W) cameraSpeed = -0.1f;
		if (key == GLFW_KEY_S) cameraSpeed = 0.1f;
		if (key == GLFW_KEY_SPACE) {
			if (autoWalking == 0) {
				autoWalking = 1;
				speed_x = 0.4f;
			}
			else {
				autoWalking = 0;
				speed_x = 0.0f;
			}
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_y = 0;
		if (key == GLFW_KEY_RIGHT) speed_y = 0;
		if (key == GLFW_KEY_UP) speed_x = 0;
		if (key == GLFW_KEY_A) extraSpeed = 0;
		if (key == GLFW_KEY_D) extraSpeed = 0;
		if (key == GLFW_KEY_W) cameraSpeed = 0.0f;
		if (key == GLFW_KEY_S) cameraSpeed = 0.0f;
	}
}

void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}

void createSingleObjectOnScene(Object object, GLuint texture1, GLuint texture2, float translateX, float translateY, float translateZ) {
	glm::mat4 M = mat4(1.0f);
	M = translate(M, vec3(translateX, translateY, translateZ));

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, object.vertices.data()); //Specify source of the data for the attribute vertex

	glEnableVertexAttribArray(sp->a("texCoord0")); //Enable sending data to the attribute texCoord0
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, object.texCoords.data()); //Specify source of the data for the attribute texCoord0

	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, object.normals.data()); //Specify source of the data for the attribute normal

	glUniform1i(sp->u("textureMap0"), 0); // Associate sampler textureMap0 with the 0-th texturing unit
	glActiveTexture(GL_TEXTURE0); //Assign texture tex0 to the 0-th texturing unit
	glBindTexture(GL_TEXTURE_2D, texture1);

	glUniform1i(sp->u("textureMap1"), 1); // Associate sampler textureMap1 with the 1-st texturing unit
	glActiveTexture(GL_TEXTURE1); //Assign texture tex1 to the 1-st texturing unit
	glBindTexture(GL_TEXTURE_2D, texture2);

	glDrawArrays(GL_TRIANGLES, 0, object.getfArray().size()); //Draw the object
}

void createSingleObjectOnScene2(Object object, GLuint texture1, float translateX, float translateY, float translateZ) {
	glm::mat4 M = mat4(1.0f);
	M = translate(M, vec3(translateX, translateY, translateZ));

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, object.vertices.data()); //Specify source of the data for the attribute vertex

	glEnableVertexAttribArray(sp->a("texCoord0")); //Enable sending data to the attribute texCoord0
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, object.texCoords.data()); //Specify source of the data for the attribute texCoord0

	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, object.normals.data()); //Specify source of the data for the attribute normal

	glUniform1i(sp->u("textureMap0"), 0); // Associate sampler textureMap0 with the 0-th texturing unit
	glActiveTexture(GL_TEXTURE0); //Assign texture tex0 to the 0-th texturing unit
	glBindTexture(GL_TEXTURE_2D, texture1);

	glDrawArrays(GL_TRIANGLES, 0, object.getfArray().size()); //Draw the object
}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Load into computer's memory
	std::vector<unsigned char> image;   //Allocate a vector for image storage
	unsigned width, height;   //Variables for image size
	//Read image
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import into graphics card's memory
	glGenTextures(1, &tex); //Initialize one handle
	glBindTexture(GL_TEXTURE_2D, tex); //Activate the handle
	//Import image into graphics card's memory associated with the handle
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

//Initialization code procedure
void initOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, at the program start************
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);
	sp=new ShaderProgram("v_simplest.glsl",NULL,"f_simplest.glsl");
	// AT-AT texture
	tex0 = readTexture("metal3.png");
	tex1 = readTexture("metal3.png");

	// snow texture
	snow0 = readTexture("snow_diff.png");
	snow1 = readTexture("snow_spec.png");

	// sky texture
	sky0 = readTexture("sphere_tex.png");

	// rocks texture
	rocks = readTexture("rocks.png");

	black = readTexture("black.png");


	string names[NUMOFPARTS] = { "body.txt", "hipLF.txt", "hipLB.txt", "hipRF.txt", "hipRB.txt", "thighLF.txt", "thighRF.txt", "thighLB.txt", "thighRB.txt",  "footLF.txt",  "footLB.txt",  "footRF.txt",  "footRB.txt", "head.txt"};

	for (int i = 0; i < NUMOFPARTS; i++) {
		Object part;
		part.load(names[i]);
		parts.push_back(part);
	}

	snowObject.load("snow.txt");
	outerSphere.load("sphere.txt");

	damaged1.load("damaged1.txt");
	damaged2.load("damaged2.txt");

	mountainsInner.load("mountainsInner.txt");
	mountainsOuter.load("mountainsOuter.txt");

}

//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, after the main loop ends************
	delete sp;
}

//Drawing procedure
void drawScene(GLFWwindow* window) {
	//************Place any code here that draws something inside the window******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	sumAngle += angle_y + extraAngle;
	vec3 camera = vec3(pos.x, pos.y + cameraHeight, pos.z - cameraDistance);
	camera.x -= sin(sumAngle) * cameraDistance;
	camera.z += (cameraDistance - cos(sumAngle) * cameraDistance);

	vec3 target = vec3(pos.x, pos.y + 1.0f, pos.z);

	glm::mat4 V=glm::lookAt(camera, target, glm::vec3(0.0f,1.0f,0.0f)); //compute view matrix
    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, 1920.0f/1080.0f, 1.0f, 50.0f); //compute projection matrix

    sp->use();//activate shading program
    //Send parameters to graphics card
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));


	//Body
    glm::mat4 MArray[NUMOFPARTS];
	MArray[0] = glm::mat4(1.0f);
	MArray[0] = translate(MArray[0], pos);
	MArray[0] = MArray[0] * toMat4(O);

	for (int i = 1; i < NUMOFPARTS; i++) {
		MArray[i] = MArray[0];
	}

	//Left Front Leg
	//Hip
	MArray[1] = translate(MArray[1], vec3(0.08f, 1.33f, 0.05f));
	MArray[1] = rotate(MArray[1], sin((((counter + ((0 * MAXCOUNTER) / 5)) % MAXCOUNTER) * PI * 2) / MAXCOUNTER) * (PI / 18), vec3(0, 0, 1));
	MArray[1] = translate(MArray[1], vec3(-0.3f, 0.0f, 0.0f));
	//Thigh
	MArray[5] = translate(MArray[1], vec3(-0.15f, 0.0f, 0.1f));
	MArray[5] = rotate(MArray[5], sin((((counter + ((0 * MAXCOUNTER) / 5)) % MAXCOUNTER) * PI) / MAXCOUNTER) * (PI / 18) - (PI/18), vec3(0, 0, 1));
	MArray[5] = translate(MArray[5], vec3(0.0f, -0.13f, 0.0f));
	//Foot
	MArray[9] = translate(MArray[5], vec3(0.0f, -0.4f, 0.0f));
	MArray[9] = rotate(MArray[9], sin((((counter + ((0 * MAXCOUNTER) / 5)) % MAXCOUNTER) * PI) / MAXCOUNTER) * (PI / 6), vec3(0, 0, 1));
	MArray[9] = translate(MArray[9], vec3(0.0f, -0.6f, 0.0f));

	//Right Front Leg
	//Hip
	MArray[3] = translate(MArray[3], vec3(0.08f, 1.33f, -0.05f));
	MArray[3] = rotate(MArray[3], sin((((counter + ((3 * MAXCOUNTER) / 5)) % MAXCOUNTER) * PI * 2) / MAXCOUNTER) * (PI / 18), vec3(0, 0, 1));
	MArray[3] = translate(MArray[3], vec3(-0.3f, 0.0f, 0.0f));
	//Thigh
	MArray[6] = translate(MArray[3], vec3(-0.15f, 0.0f, -0.1f));
	MArray[6] = rotate(MArray[6], sin((((counter + ((3 * MAXCOUNTER) / 5)) % MAXCOUNTER) * PI) / MAXCOUNTER) * (PI / 18) - (PI / 18), vec3(0, 0, 1));
	MArray[6] = translate(MArray[6], vec3(0.0f, -0.13f, 0.0f));
	//Foot
	MArray[11] = translate(MArray[6], vec3(0.0f, -0.4f, 0.0f));
	MArray[11] = rotate(MArray[11], sin((((counter + ((3 * MAXCOUNTER) / 5)) % MAXCOUNTER) * PI) / MAXCOUNTER) * (PI / 6), vec3(0, 0, 1));
	MArray[11] = translate(MArray[11], vec3(0.0f, -0.6f, 0.0f));

	//Right Back Leg
	//Hip
	MArray[4] = translate(MArray[4], vec3(-0.12f, 1.3f, -0.05f));
	MArray[4] = rotate(MArray[4], sin((((counter + ((2 * MAXCOUNTER) / 5)) % MAXCOUNTER) * PI * 2) / MAXCOUNTER) * (PI / 18), vec3(0, 0, -1));
	MArray[4] = translate(MArray[4], vec3(0.6f, 0.0f, 0.0f));
	//Thigh
	MArray[8] = translate(MArray[4], vec3(0.15f, 0.0f, -0.1f));
	MArray[8] = rotate(MArray[8], sin((((counter + ((2 * MAXCOUNTER) / 5)) % MAXCOUNTER) * PI) / MAXCOUNTER) * (PI / 18) - (PI / 18), vec3(0, 0, 1));
	MArray[8] = translate(MArray[8], vec3(0.0f, -0.13f, 0.0f));
	//Foot
	MArray[12] = translate(MArray[8], vec3(0.0f, -0.4f, 0.0f));
	MArray[12] = rotate(MArray[12], sin((((counter + ((2 * MAXCOUNTER) / 5)) % MAXCOUNTER) * PI) / MAXCOUNTER) * (PI / 6), vec3(0, 0, 1));
	MArray[12] = translate(MArray[12], vec3(0.0f, -0.6f, 0.0f));

	//Left Back Leg
	//Hip
	MArray[2] = translate(MArray[2], vec3(-0.12f, 1.3f, 0.05f));
	MArray[2] = rotate(MArray[2], sin((((counter + ((1 * MAXCOUNTER) / 5)) % MAXCOUNTER) * PI * 2) / MAXCOUNTER) * (PI / 18), vec3(0, 0, -1));
	MArray[2] = translate(MArray[2], vec3(0.6f, 0.0f, 0.0f));
	//Thigh
	MArray[7] = translate(MArray[2], vec3(0.15f, 0.0f, 0.1f));
	MArray[7] = rotate(MArray[7], sin((((counter + ((1 * MAXCOUNTER) / 5)) % MAXCOUNTER) * PI) / MAXCOUNTER) * (PI / 18) - (PI / 18), vec3(0, 0, 1));
	MArray[7] = translate(MArray[7], vec3(0.0f, -0.13f, 0.0f));
	//Foot
	MArray[10] = translate(MArray[7], vec3(0.0f, -0.4f, 0.0f));
	MArray[10] = rotate(MArray[10], sin((((counter + ((1 * MAXCOUNTER) / 5)) % MAXCOUNTER) * PI) / MAXCOUNTER) * (PI / 6), vec3(0, 0, 1));
	MArray[10] = translate(MArray[10], vec3(0.0f, -0.6f, 0.0f));

	//Head
	MArray[13] = translate(MArray[13], vec3(-0.9f, 1.6f, 0.0f));
	MArray[13] = rotate(MArray[13], (((float) curveCounter) / ((float) MAXCURVECOUNTER)) * (PI / 10), vec3(0, 1, 0));
	MArray[13] = rotate(MArray[13], (((float)curveCounter) / ((float)MAXCURVECOUNTER)) * (PI / 18), vec3(1, 0, 0));
	MArray[13] = translate(MArray[13], vec3(-0.25f, 0.0f, 0.0f));



	for (int i = 0; i < NUMOFPARTS; i++) {

		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(MArray[i])); //Za³aduj do programu cieniuj¹cego macierz modelu
		glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, parts[i].vertices.data()); //Specify source of the data for the attribute vertex

		glEnableVertexAttribArray(sp->a("texCoord0")); //Enable sending data to the attribute texCoord0
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, parts[i].texCoords.data()); //Specify source of the data for the attribute texCoord0

		glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute color
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, parts[i].normals.data()); //Specify source of the data for the attribute normal

		glUniform1i(sp->u("textureMap0"), 0); // Associate sampler textureMap0 with the 0-th texturing unit
		glActiveTexture(GL_TEXTURE0); //Assign texture tex0 to the 0-th texturing unit
		glBindTexture(GL_TEXTURE_2D, tex0);

		glUniform1i(sp->u("textureMap1"), 1); // Associate sampler textureMap1 with the 1-st texturing unit
		glActiveTexture(GL_TEXTURE1); //Assign texture tex1 to the 1-st texturing unit
		glBindTexture(GL_TEXTURE_2D, tex1);

		glDrawArrays(GL_TRIANGLES, 0, parts[i].getfArray().size()); //Draw the object
	}


	//// create snow
	createSingleObjectOnScene(snowObject, snow0, snow1, 0.0f, -0.2f, 0.0f);
	//// create sphere
	createSingleObjectOnScene2(outerSphere, sky0, 0.0f, 0.0f, 0.0f);

	// create damaged atat
	createSingleObjectOnScene(damaged1, tex0, tex1, 10.0f, -0.2f, 12.0f);
	createSingleObjectOnScene(damaged2, tex0, tex1, -9.0f, -0.2f, 6.0f);

	// create inner mountains
	createSingleObjectOnScene2(mountainsInner, snow0, 0.0f, 0.0f, 0.0f);
	createSingleObjectOnScene2(mountainsOuter, rocks, 0.0f, 0.0f, 0.0f);


    glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("texCoord0")); //Disable sending data to the attribute texCoord0
	glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal

    glfwSwapBuffers(window); //Copy back buffer to front buffer
}

int main(void)
{
	GLFWwindow* window; //Pointer to object that represents the application window

	glfwSetErrorCallback(error_callback);//Register error processing callback procedure

	if (!glfwInit()) { //Initialize GLFW library
		fprintf(stderr, "Can't initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1920, 1080, "AT-AT Simulator", NULL, NULL);  //Create a window 500pxx500px titled "OpenGL" and an OpenGL context associated with it.

	if (!window) //If no window is opened then close the program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Since this moment OpenGL context corresponding to the window is active and all OpenGL calls will refer to this context.
	glfwSwapInterval(1); //During vsync wait for the first refresh

	GLenum err;
	if ((err=glewInit()) != GLEW_OK) { //Initialize GLEW library
		fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Call initialization procedure


	//Main application loop
	glfwSetTime(0); //Wyzeruj licznik czasu
	while (!glfwWindowShouldClose(window)) //As long as the window shouldnt be closed yet...
	{
		// Out of map
		if ((pos.x * pos.x) + (pos.z * pos.z) > 16 * 16) {
			O = quat(vec3(0, 0, 0));
			pos = vec3(0, 0, 0);
			counter = 0;
			curveCounter = 0;
		}

		//Head tilt
		if (speed_y > 0) {
			curveCounter += 1;
		} else if(speed_y < 0) {
			curveCounter -= 1;
		} else if ((speed_y == 0) && (curveCounter > 0)) {
			curveCounter -= 1;
		} else if ((speed_y == 0) && (curveCounter < 0)) {
			curveCounter += 1;
		}

		if (curveCounter > MAXCURVECOUNTER) curveCounter = MAXCURVECOUNTER;
		if (curveCounter < -1*MAXCURVECOUNTER) curveCounter = -1*MAXCURVECOUNTER;

		//Walk cycle
		angle_y =speed_y*glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
		if (speed_x == 0)
			angle_y = 0;
		else {
			counter += 1;
			if (counter == MAXCOUNTER) counter = 0;
		}
		extraAngle = extraSpeed * glfwGetTime();

		cameraDistance += cameraSpeed;
		if (cameraDistance < 2.3f) cameraDistance = 2.3f;
		else if (cameraDistance > 7.0f) cameraDistance = 7.0f;

		O = rotate(O, angle_y, vec3(0, 1, 0));
		

		O = normalize(O);

		vec3 dir = toMat3(O) * vec3(-1, 0, 0);

		pos += speed_x * (float)glfwGetTime() * dir;

		glfwSetTime(0); //Zero the timer
		drawScene(window); //Execute drawing procedure
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}

