#version 410

layout(location = 0) in vec4 position4D;
layout(location = 1) in vec4 normal4D;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 modelMat;
 
//out vec3 lightDirection_cameraSpace;
//out vec3 normal_cameraSpace;
//out vec3 eyeDirection_cameraSpace;
//out float distanceVertToLight;
out vec3 vertNormal;
out vec3 fragPos_worldSpace;

void main() {
	
	//project position stereographically to 3D
	//vec4 projSource = vec4(0.0, 0.0, 0.0, 1.0);
	//float t = 1.0 / (1.0 - position4D.w);
	float xPos3D = position4D.x / 1.0 - position4D.w;
	float yPos3D = position4D.y / 1.0 - position4D.w;
	float zPos3D = position4D.z / 1.0 - position4D.w;

	//vec3 position3D = vec3(t * position4D.x, t * position4D.y, t * position4D.z);
	vec3 position3D = vec3(xPos3D, yPos3D, zPos3D);

	fragPos_worldSpace = vec3((modelMat * vec4(position3D, 1.0)).xyz);

	//prject normal to 3D
	//float u = 1.0 / (1.0 - normal4D.w);
	float xNorm3D = normal4D.x / 1.0 - normal4D.w;
	float yNorm3D = normal4D.y / 1.0 - normal4D.w;
	float zNorm3D = normal4D.z / 1.0 - normal4D.w;
	
	//vec3 normal3D = vec3(u * normal4D.x, u * normal4D.y, u * normal4D.z);
	vec3 normal3D = vec3(xNorm3D, yNorm3D, zNorm3D);
	vertNormal = normal3D;

	gl_Position = projMat * viewMat * modelMat * vec4(position3D, 1.0);	

 	//vec3 position_cameraSpace = vec3((viewMat * modelMat * vec4(position3D, 1.0)).xyz);
	//eyeDirection_cameraSpace = vec3(0.0, 0.0, 0.0) - position_cameraSpace;	
	
	//vec3 lightPosition_worldSpace = vec3(2.0, 0.0, 0.0);	
	//vec3 lightPosition_cameraSpace = vec3((viewMat * vec4(lightPosition_worldSpace, 1.0)).xyz);
	//lightDirection_cameraSpace = lightPosition_cameraSpace + eyeDirection_cameraSpace;

	//normal_cameraSpace = vec3((viewMat * modelMat * vec4(normal3D, 1.0)).xyz);	

	//distanceVertToLight = distance(position3D, lightPosition_worldSpace);
}
