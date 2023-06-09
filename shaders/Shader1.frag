#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform GlobalUniformBufferObject {
	vec3 DlightDir;		// direction of the direct light
	vec3 DlightColor;	// color of the direct light
	vec3 AmbLightColor;	// ambient light
	vec3 eyePos;		// position of the viewer
} gubo;

layout(binding = 1) uniform sampler2D tex;

void main() {
	vec3 N = normalize(fragNorm);				// surface normal
	vec3 V = normalize(gubo.eyePos - fragPos);	// viewer direction
	vec3 L = normalize(gubo.DlightDir);			// light direction


    float amb = 1.0f; 
	vec3 albedo = texture(tex, fragUV).rgb;		// main color
	vec3 MD = albedo;
	vec3 MS = vec3(1.0f);
	vec3 MA = albedo * amb;
	vec3 LA = gubo.AmbLightColor;
    float gamma = 180.0f;
	
	// Blinn Lambert shader
	
	vec3 h = normalize(L + V);
	
	vec3 specular = MS * pow(clamp(dot(N, h), 0.00001f, 1.0), gamma); // Blinn
	vec3 diffuse = L * MD * max(dot(L, N), 0); // Lambert

	vec3 DiffSpec = diffuse + specular;
	vec3 Ambient = LA * MA;

	outColor = vec4(clamp(0.95 * DiffSpec * gubo.DlightColor.rgb + Ambient, 0.00001f ,1.0f), 1.0f);	// output color
}