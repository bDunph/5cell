#version 410

uniform vec3 lightPos;

//in vec3 lightDirection_cameraSpace;
//in vec3 normal_cameraSpace;
//in vec3 eyeDirection_cameraSpace;
//in float distanceVertToLight;
in vec3 vertNormal;
in vec3 fragPos_worldSpace;

out vec4 colour_out;

void main() {
	
	//float lightPower = 0.5;
	vec3 lightColour = vec3(1.0, 1.0, 1.0);
	//vec3 materialDiffuseColour = vec3(0.5, 0.125, 0.0);
	//vec3 materialAmbientColour = vec3(0.1, 0.1, 0.1);
	//vec3 materialSpecularColour = vec3(0.2, 0.2, 0.2);
	//*** Ambient ***//
	vec3 objectColour = vec3(0.5, 0.125, 0.05);
	float ambientStrength = 0.3;

	vec3 ambient = ambientStrength * lightColour;

	//*** Diffuse ***//
	vec3 norm = normalize(vertNormal);
	vec3 lightDir_worldSpace = normalize(lightPos - fragPos_worldSpace);
	float diffuseAngle = max(dot(norm, lightDir_worldSpace), 0.0);
	vec3 diffuse = diffuseAngle * lightColour;

	vec3 result = (ambient + diffuse) * objectColour;
	colour_out = vec4(result, 1.0);

	//vec3 n = normalize(normal_cameraSpace);
	//vec3 l = normalize(lightDirection_cameraSpace);

	//vec3 e = normalize(eyeDirection_cameraSpace);
	//vec3 r = reflect(-l, n);

	//float cosAlpha = clamp(dot(e, r), 0.0, 1.0);
	//float cosTheta = clamp(dot(n, l), 0.0, 1.0);
	
	//vec3 colour = materialAmbientColour + (materialDiffuseColour * lightColour * lightPower * cosTheta / (distanceVertToLight * distanceVertToLight)) + (materialSpecularColour * lightColour * lightPower * pow(cosAlpha, 5) / (distanceVertToLight * distanceVertToLight)) ;

	//colour_out = vec4(colour, 1.0);
}
