#version 410

in vec3 lightDirection_cameraSpace;
in vec3 normal_cameraSpace;
in vec3 eyeDirection_cameraSpace;
in float distanceVertToLight;

out vec4 colour_out;

void main() {
	
	float lightPower = 0.5;
	vec3 lightColour = vec3(0.98, 0.91, 0.89);
	vec3 materialDiffuseColour = vec3(0.825, 0.625, 0.625);
	vec3 materialAmbientColour = vec3(0.1, 0.1, 0.1);
	vec3 materialSpecularColour = vec3(0.2, 0.2, 0.2);

	vec3 n = normalize(normal_cameraSpace);
	vec3 l = normalize(lightDirection_cameraSpace);

	vec3 e = normalize(eyeDirection_cameraSpace);
	vec3 r = reflect(-l, n);

	float cosAlpha = clamp(dot(e, r), 0.0, 1.0);
	float cosTheta = clamp(dot(n, l), 0.0, 1.0);
	
	vec3 colour = materialAmbientColour + (materialDiffuseColour * lightColour * lightPower * cosTheta / (distanceVertToLight * distanceVertToLight)) + (materialSpecularColour * lightColour * lightPower * pow(cosAlpha, 5) / (distanceVertToLight * distanceVertToLight)) ;

	colour_out = vec4(colour, 0.1);
}
