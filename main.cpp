#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <assert.h>
#include <math.h>
#include <iostream>

#include "log.h"
#include "shader_manager.h"
#include "utils.h"

#define GOLDRATIO 1.6180339887

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof((x)[0]))
#endif

//keep track of window size for things like the viewport and the mouse cursor
int g_gl_width;
int g_gl_height;

//a call-back function
void glfw_window_size_callback(GLFWwindow* window, int width, int height){
	g_gl_width = width;
	g_gl_height = height;

	/* update any perspective matrices here */
}

int main(){

	bool logStarted = restart_gl_log();
	assert(logStarted);

	//start gl context and O/S window using the glfw helper library
	if(!glfwInit()){
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	//uncomment these lines if on osx
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//anti-aliasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	// GLFWmonitor* mon = glfwGetPrimaryMonitor();
	// const GLFWvidmode* vmode = glfwGetVideoMode(mon);

	g_gl_width = 640;//vmode->width;
	g_gl_height = 480;//vmode->height;

	GLFWwindow* window = glfwCreateWindow(g_gl_width, g_gl_height, "Extended GL Init", NULL, NULL);
	if(!window){
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	//uncomment if you want to print the OpenGL parameters to the log file
	// log_gl_params();

	//start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();


	//get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); //get renderer string
	const GLubyte* version = glGetString(GL_VERSION); //version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//tell GL only to draw onto a pixel if a shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); //enable depth testing
	glDepthFunc(GL_LESS); //depth testing interprets a smaller value as "closer"
	glDisable(GL_CULL_FACE);

	/* specify 4D coordinates of 5-cell from https://en.wikipedia.org/wiki/5-cell */
	/*float vertices [20] = {
		2.0, 0.0, 0.0, 0.0,
		0.0, 2.0, 0.0, 0.0,
		0.0, 0.0, 2.0, 0.0,
		0.0, 0.0, 0.0, 2.0,
		GOLDRATIO, GOLDRATIO, GOLDRATIO, GOLDRATIO
	};*/
	float vertices [20] = {
		1.0f/sqrt(10.0f), 1.0f/sqrt(6.0f), 1.0f/sqrt(3.0f), 1.0f,
		1.0f/sqrt(10.0f), 1.0f/sqrt(6.0f), 1.0f/sqrt(3.0f), -1.0f,
		1.0f/sqrt(10.0f), 1.0f/sqrt(6.0f), -2.0f/sqrt(3.0f), 0.0f,
		1.0f/sqrt(10.0f), -sqrt(3.0f/2.0f), 0.0f, 0.0f,
		-2.0f * sqrt(2.0f/5.0f), 0.0f, 0.0f, 0.0f
	};

	/* indices specifying 10 faces */
	unsigned int indices [30] = {
		0, 1, 2,
		2, 3, 0,
		0, 3, 1,
		1, 3, 2,
		2, 3, 4,
		4, 3, 1,
		1, 2, 4,
		4, 2, 0,
		0, 1, 4,
		4, 3, 0
		
	};

	//array of verts
	glm::vec4 vertArray [5] = {
		glm::vec4(vertices[0], vertices[1], vertices[2], vertices[3]),
		glm::vec4(vertices[4], vertices[5], vertices[6], vertices[7]),
		glm::vec4(vertices[8], vertices[9], vertices[10], vertices[11]),
		glm::vec4(vertices[12], vertices[13], vertices[14], vertices[15]),
		glm::vec4(vertices[16], vertices[17], vertices[18], vertices[19])
	};

	//array of faces
	glm::vec3 faceArray [10] = {
		glm::vec3(indices[0], indices[1], indices[2]),
		glm::vec3(indices[3], indices[4], indices[5]),
		glm::vec3(indices[6], indices[7], indices[8]),
		glm::vec3(indices[9], indices[10], indices[11]),
		glm::vec3(indices[12], indices[13], indices[14]),
		glm::vec3(indices[15], indices[16], indices[17]),
		glm::vec3(indices[18], indices[19], indices[20]),
		glm::vec3(indices[21], indices[22], indices[23]),
		glm::vec3(indices[24], indices[25], indices[26]),
		glm::vec3(indices[27], indices[28], indices[29]),
	};

	glm::vec4 faceNormalArray [10];
	
	//calculate vertex normals in 4D to send to shaders for lighting
	for(int i = 0; i < _countof(faceArray); i++){
		//calculate three linearly independent vectors for each face
		unsigned int indexA = faceArray[i].x;
		unsigned int indexB = faceArray[i].y;
		unsigned int indexC = faceArray[i].z;

		glm::vec4 vertA = vertArray[indexA];
		glm::vec4 vertB = vertArray[indexB];
		glm::vec4 vertC = vertArray[indexC];

		glm::vec4 vectorA = glm::vec4(vertB.x - vertA.x, vertB.y - vertA.y, vertB.z - vertA.z, vertB.w - vertA.w);
		glm::vec4 vectorB = glm::vec4(vertC.x - vertB.x, vertC.y - vertB.y, vertC.z - vertB.z, vertC.w - vertB.w);
		glm::vec4 vectorC = glm::vec4(vertA.x - vertC.x, vertA.y - vertC.y, vertA.z - vertC.z, vertA.w - vertC.w);

		//calculate the  normal for each face
	 	//using matrices and  Laplace expansion we can find the normal 
		//vector in 4D given three input vectors	
		//this procedure is following the article at https://ef.gy/linear-algebra:normal-vectors-in-higher-dimensional-spaces 
		/* a x b x c = 	| a0 b0 c0 right|
				| a1 b1 c1 up	|
				| a2 b2 c2 back	|	
				| a3 b3 c3 charm|*/
		glm::vec4 right = glm::vec4(1.0, 0.0, 0.0, 0.0);	
		glm::vec4 up = glm::vec4(0.0, 1.0, 0.0, 0.0);	
		glm::vec4 back = glm::vec4(0.0, 0.0, 1.0, 0.0);	
		glm::vec4 charm = glm::vec4(0.0, 0.0, 0.0, 1.0);	

		glm::mat3 matA = glm::mat3(	vectorA.y, vectorB.y, vectorC.y,
						vectorA.z, vectorB.z, vectorC.z,
						vectorA.w, vectorB.w, vectorC.w);

		glm::mat3 matB = glm::mat3(	vectorA.x, vectorB.x, vectorC.x,
						vectorA.z, vectorB.z, vectorC.z,
						vectorA.w, vectorB.w, vectorC.w);

		glm::mat3 matC = glm::mat3(	vectorA.x, vectorB.x, vectorC.x,
						vectorA.y, vectorB.y, vectorC.y,
						vectorA.w, vectorB.w, vectorC.w);
	
		glm::mat3 matD = glm::mat3(	vectorA.x, vectorB.x, vectorC.x,
						vectorA.y, vectorB.y, vectorC.y,
						vectorA.z, vectorB.z, vectorC.z);

		float determinantA = glm::determinant(matA);	
		float determinantB = glm::determinant(matB);	
		float determinantC = glm::determinant(matC);	
		float determinantD = glm::determinant(matD);	

		glm::vec4 termA = (determinantA * right) * -1.0f;
		glm::vec4 termB = determinantB * up;
		glm::vec4 termC = (determinantC * back) * -1.0f;
		glm::vec4 termD = determinantD * charm;

		glm::vec4 faceNormal = termA + termB + termC + termD;
		faceNormalArray[i] += faceNormal;
	}
	
	float vertexNormalArray [20];

	//calculate the normal for each vertex by taking the average of the normals of each adjacent face
	for(int i = 0; i < _countof(vertArray); i++){
		glm::vec4 cumulativeNormals = glm::vec4(0.0);
		for(int j = 0; j < _countof(faceArray); j++){
				std::cout << "HERE!" << std::endl;
			//does this face [j] contain vert [i]?
			unsigned int vertA = faceArray[j].x;				 
			unsigned int vertB = faceArray[j].y;
			unsigned int vertC = faceArray[j].z;
			if(vertA == i || vertB == i || vertC == i){
				cumulativeNormals += faceNormalArray[j];
			}	
		}
		glm::vec4 vertexNormal = glm::normalize(cumulativeNormals);
		vertexNormalArray[i*4] += vertexNormal.x;
		vertexNormalArray[i*4+1] += vertexNormal.y;
		vertexNormalArray[i*4+2] += vertexNormal.z;
		vertexNormalArray[i*4+3] += vertexNormal.w;
	}
		
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	GLuint vertNormals;
	glGenBuffers(1, &vertNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vertNormals);
	glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), vertexNormalArray, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	GLuint index;
	glGenBuffers(1, &index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 30 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	//load shaders
	const char* vertex_shader;
	bool isVertLoaded = load_shader("rasterPolychoron.vert", vertex_shader);
	if(!isVertLoaded) return 1;

	const char* fragment_shader;
	bool isFragLoaded = load_shader("5cell.frag", fragment_shader);
	if(!isFragLoaded) return 1;

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	delete[] vertex_shader;
	//check for compile errors
	bool isVertCompiled = shader_compile_check(vs);
	if(!isVertCompiled) return 1;

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);
	delete[] fragment_shader;
	//check for compile errors
	bool isFragCompiled = shader_compile_check(fs);
	if(!isFragCompiled) return 1;
	
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, fs);
	glAttachShader(shader_program, vs);
	glLinkProgram(shader_program);
	bool didShadersLink = shader_link_check(shader_program);
	if(!didShadersLink) return 1;

	//matrices
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 modelMatrix;

	// projection matrix setup	
	projectionMatrix = glm::perspective(60.0f, (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);

	GLint projMatLoc = glGetUniformLocation(shader_program, "projMat");
	GLint viewMatLoc = glGetUniformLocation(shader_program, "viewMat");
	GLint modelMatLoc = glGetUniformLocation(shader_program, "modelMat");
	
	//view matrix setup
	//glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.5f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	//glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	//glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	
	//glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	//viewMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);	

	//model matrix
	modelMatrix = glm::mat4(1.0);

	//uniforms
	GLint lightPosLoc = glGetUniformLocation(shader_program, "lightPos");
	glm::vec3 lightPos = glm::vec3(0.0, 1.0, 0.0); 

	//GLint colour = glGetUniformLocation(shader_program, "inputColour");
	//glUseProgram(shader_program);
	//glUniform4f(colour, 0.5f, 0.0f, 0.5f, 1.0f);

	//replacement for iResolution in mengerTest.frag
	//GLint resolution = glGetUniformLocation(shader_program, "resolution");
	//glUseProgram(shader_program);
	//glUniform2f(resolution, (float)g_gl_width, (float)g_gl_height);

	print_all(shader_program);

	//only use during development as computationally expensive
	bool validProgram = is_valid(shader_program);
	if(!validProgram){
		fprintf(stderr, "ERROR: shader program not valid\n");
		return 1;
	}

	//workaround for macOS Mojave bug
	bool needDraw = true;

	float radius = 0.5f;

	while(!glfwWindowShouldClose(window)){

		_update_fps_counter(window);

		//wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//resize openGL elements
		glViewport(0, 0, g_gl_width, g_gl_height);

		/* draw stuff here */
		
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		viewMatrix = glm::lookAt(glm::vec3(camX, 0.0f, camZ), cameraTarget, up);

		glUseProgram(shader_program);
		// glUniform1f(glGetUniformLocation(shader_program, "iGlobalTime"), global_time);

		glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
		glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

		glBindVertexArray(vao);
		// draw 5-cell using index buffer
		glDrawElements(GL_TRIANGLES, 30 * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);

		//update other events like input handling
		glfwPollEvents();

		// workaround for macOS Mojave bug
		if(needDraw){
			glfwShowWindow(window);
			glfwHideWindow(window);
			glfwShowWindow(window);
			needDraw = false;
		}

		//put the stuff we've been drawing onto the display
		glfwSwapBuffers(window);

		if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)){
			glfwSetWindowShouldClose(window, 1);
		}

		glfwSetWindowSizeCallback(window, glfw_window_size_callback);
		glfwSetErrorCallback(glfw_error_callback);
	}

	//close GL context and any other GL resources
	glfwTerminate();
	return 0;
}
