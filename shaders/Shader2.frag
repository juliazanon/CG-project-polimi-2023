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

vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md, float F0, float metallic, float roughness) {
	//vec3 V  - direction of the viewer
	//vec3 N  - normal vector to the surface
	//vec3 L  - light vector (from the light model)
	//vec3 Md - main color of the surface
	//float F0 - Base color for the Fresnel term
	//float metallic - parameter that mixes the diffuse with the specular term.
	//                 in particular, parmeter K seen in the slides is: float K = 1.0f - metallic;
	//float roughness - Material roughness (parmeter rho in the slides).
	//specular color Ms is not passed, and implicitely considered white: vec3 Ms = vec3(1.0f);

	float k = 1.0f - metallic;
	vec3 Ms = vec3(1.0f);

	vec3 h = normalize(L + V);
	float D = exp(-(1 - pow(dot(h, N), 2)) / (pow(dot(h, N) * roughness, 2)))/(radians(180) * pow(roughness, 2) * pow(dot(h, N), 4));
	// float F = F0 + (1 - F0) * pow((1 - clamp(dot(V, h), 0.0, 1.0)), 5);
	float F = 1;
	//float G = min(1, min(2 * dot(h, N) * dot(V, N) / dot(V, h), 2 * dot(h, N) * dot(L, N) / dot(V, h))); // Non GGX
	float ggx1 = 2 / (1 + sqrt(1 + pow(roughness, 2) * (1 - pow(dot(N, V), 2)) / pow(dot(N, V), 2)));
	float ggx2 = 2 / (1 + sqrt(1 + pow(roughness, 2) * (1 - pow(dot(N, L), 2)) / pow(dot(N, L), 2)));
	float G = ggx1 * ggx2;

	vec3 diffuse = Md * clamp(dot(L, N), 0.0, 1.0);
	vec3 specular = Ms * (D * F * G / (4 * clamp(dot(V, N), 0.00001f, 1.0f)));

	return k * diffuse + (1 - k) * specular;
}

vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md, float sigma) {
	//vec3 V  - direction of the viewer
	//vec3 N  - normal vector to the surface
	//vec3 L  - light vector (from the light model)
	//vec3 Md - main color of the surface
	//float sigma - Roughness of the model

	vec3 vi = normalize(L - dot(L, N) * N);
	vec3 vr = normalize(V - dot(V, N) * N);
	float G = max(0, dot(vi, vr));
	vec3 l = Md * clamp(dot(L, N), 0.00001f, 1.0f);

	float A = 1 - 0.5f * (pow(sigma, 2)/(pow(sigma, 2) + 0.33f));
	float B = 0.45f * (pow(sigma, 2)/(pow(sigma, 2) + 0.09f));

	float thetai = acos(dot(L, N));
	float thetar = acos(dot(V, N));
	float alpha = max(thetai, thetar);
	float beta = min(thetai, thetar);
	
	return l * (A + G * B * sin(alpha) * tan(beta));
}

void main() {
	vec3 N = normalize(fragNorm);				// surface normal
	vec3 V = normalize(gubo.eyePos - fragPos);	// viewer direction
	vec3 L = normalize(gubo.DlightDir);			// light direction

	float amb = 2.0f; 
	vec3 albedo = texture(tex, fragUV).rgb;		// main color
	vec3 MD = albedo;
	vec3 MS = vec3(1.0f);
	vec3 MA = albedo * amb;
	vec3 LA = gubo.AmbLightColor;
    float gamma = 180.0f;
	
	// Cook Torrance shader
	//vec3 DiffSpec = BRDF(V, N, L, MD, 0.3f, 0.5f, 0.5f);
	// Oren Nayar Shader
	vec3 DiffSpec = BRDF(V, N, L, MD, 0.0f);

	vec3 Ambient = LA * MA;

	outColor = vec4(clamp(0.95 * DiffSpec * gubo.DlightColor.rgb + Ambient,0.00001f,1.0f), 1.0f);
}