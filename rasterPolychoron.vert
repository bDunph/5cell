#version 410

layout(location = 0) in vec4 position4D;
layout(location = 1) in vec4 normal4D;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 modelMat;
uniform vec3 camPos;
uniform vec3 lightPos;
uniform mat4 rotZW;
uniform mat4 scaleMat;
 
//out vec3 lightDirection_cameraSpace;
//out vec3 normal_cameraSpace;
//out vec3 eyeDirection_cameraSpace;
//out float distanceVertToLight;
out vec3 vertNormal_worldSpace;
out vec3 fragPos_worldSpace;
out vec3 lightPos_viewMat;

void main() {

	
	lightPos_viewMat = vec3((viewMat * vec4(lightPos, 1.0)).xyz);
	vec4 scaledPos = scaleMat * position4D;
	vec4 rotatedPos = rotZW * scaledPos;

	float dist = 2.0;
	float xPos3D = rotatedPos.x * dist / (dist - rotatedPos.w);
	float yPos3D = rotatedPos.y * dist / (dist - rotatedPos.w);
	float zPos3D = rotatedPos.z * dist / (dist - rotatedPos.w);

	vec3 position3D = vec3(xPos3D, yPos3D, zPos3D);
	
	fragPos_worldSpace = vec3((modelMat * vec4(position3D, 1.0)).xyz);

	//prject normal to 3D
	float xNorm3D = normal4D.x * dist / (dist - normal4D.w);
	float yNorm3D = normal4D.y * dist / (dist - normal4D.w);
	float zNorm3D = normal4D.z * dist / (dist - normal4D.w);
	
	vec3 normal3D = vec3(xNorm3D, yNorm3D, zNorm3D);
	vertNormal_worldSpace = mat3(transpose(inverse(modelMat))) *  normal3D;

	gl_Position = projMat * viewMat * modelMat *  vec4(position3D, 1.0);	

 	//vec3 position_cameraSpace = vec3((viewMat * modelMat * vec4(position3D, 1.0)).xyz);
	//eyeDirection_cameraSpace = vec3(0.0, 0.0, 0.0) - position_cameraSpace;	
	
	//vec3 lightPosition_worldSpace = vec3(2.0, 0.0, 0.0);	
	//vec3 lightPosition_cameraSpace = vec3((viewMat * vec4(lightPosition_worldSpace, 1.0)).xyz);
	//lightDirection_cameraSpace = lightPosition_cameraSpace + eyeDirection_cameraSpace;

	//normal_cameraSpace = vec3((viewMat * modelMat * vec4(normal3D, 1.0)).xyz);	

	//distanceVertToLight = distance(position3D, lightPosition_worldSpace);
}
