// This has been adapted from the Vulkan tutorial

#include "Starter.hpp"
#include "TextMaker.hpp"

# define M_PI           3.14159265358979323846  /* pi */

 std::vector<SingleText> demoText = {
	{1, {"Rubiks cube", "", "", ""}, 0, 0}
};

// The uniform buffer object used in this example
 struct MeshUniformBlock {
	 alignas(4) float amb;
	 alignas(4) float gamma;
	 alignas(16) glm::vec3 sColor;
	 alignas(16) glm::mat4 mvpMat;
	 alignas(16) glm::mat4 mMat;
	 alignas(16) glm::mat4 nMat;
 };

 struct GlobalUniformBlock {
	 alignas(16) glm::vec3 DlightDir;
	 alignas(16) glm::vec3 DlightColor;
	 alignas(16) glm::vec3 AmbLightColor;
	 alignas(16) glm::vec3 eyePos;
 };


struct Vertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

class Rubiks;
void GameLogic(Rubiks *A, float Ar, glm::mat4 &ViewPrj, glm::mat4 &World);

// MAIN ! 
class Rubiks : public BaseProject {
protected:
	// Here you list all the Vulkan objects you need:

	glm::mat4 Rotations[27][3];
	int selFace = 0;
	int cube[3][3][3] = { {{0, 1, 2}, { 3, 4, 5}, { 6, 7, 8,}},
			{{9, 10, 11}, { 12, 13, 14}, { 15, 16, 17,}},
			{{18, 19, 20}, { 21, 22, 23}, { 24, 25, 26,}} };


	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout DSL, DSLGubo;

	// Pipelines [Shader couples]
	VertexDescriptor VD;
	Pipeline P1;

	// Models, textures and Descriptors (values assigned to the uniforms)
	Model<Vertex> Cube;
	DescriptorSet DS1, DS2, DS3, DS4, DS5, DS6, DS7, DS8, DS9, DS10, DS11, DS12, DS13, 
		DS14, DS15, DS16, DS17, DS18, DS19, DS20, DS21, DS22, DS23, DS24, DS25, DS26,
		DSGubo;
	DescriptorSet descriptorSets[26];
	Texture T1, T2;

	MeshUniformBlock ubo1, ubo2, ubo3, ubo4, ubo5, ubo6, ubo7, ubo8, ubo9, ubo10, ubo11, ubo12,
		ubo13, ubo14, ubo15, ubo16, ubo17, ubo18, ubo19, ubo20, ubo21, ubo22, ubo23, ubo24, ubo25, ubo26;
	GlobalUniformBlock gubo;

	TextMaker txt;
	
	// Other application parameters
	float Ar;
	glm::mat4 ViewPrj;
	glm::vec3 Pos = glm::vec3(0.0f,0.0f,0.0f);
	glm::vec3 cameraPos;
	float Yaw = glm::radians(30.0f);
	float Pitch = glm::radians(22.5f);

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, title and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "Rubiks project";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.015f, 0.03f, 1.0f};
		
		// Descriptor pool sizes
		uniformBlocksInPool = 27;
		texturesInPool = 30;
		setsInPool = 30;
		
		Ar = 4.0f / 3.0f;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		std::cout << "Window resized to: " << w << " x " << h << "\n";
		Ar = (float)w / (float)h;
	}
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]

		for (int i = 0; i < 27; i++) {
			for (int j = 0; j < 3; j++) {
				Rotations[i][j] = glm::mat4(1);
			}
		};

		DSL.init(this, {
					// this array contains the binding:
					// first  element : the binding number
					// second element : the type of element (buffer or texture)
					// third  element : the pipeline stage where it will be used
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				});

		DSLGubo.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
			});

		// Vertex descriptors
		VD.init(this, {
				  {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, norm),
						 sizeof(glm::vec3), NORMAL},
				  {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, UV),
				         sizeof(glm::vec2), UV}
				});

		// Pipelines [Shader couples]
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		P1.init(this, &VD, "shaders/ShaderVert.spv", "shaders/ShaderFrag.spv", {&DSLGubo, &DSL});
		/*P1.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, false);*/

		// Models, textures and Descriptors (values assigned to the uniforms)
		Cube.init(this, &VD, "Models/Cube.obj", OBJ);

		T1.init(this, "textures/Checker.png");
		
		txt.init(this, &demoText);
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		P1.create();

		// the ds array must be populated after being initialized
		DS1.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
				});
		descriptorSets[0] = DS1;

		DS2.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
				});
		descriptorSets[1] = DS2;

		DS3.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[2] = DS3;

		DS4.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[3] = DS4;

		DS5.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[4] = DS5;

		DS6.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[5] = DS6;

		DS7.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[6] = DS7;

		DS8.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[7] = DS8;

		DS9.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[8] = DS9;

		DS10.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[9] = DS10;

		DS11.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[10] = DS11;

		DS12.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[11] = DS12;

		DS13.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[12] = DS13;

		DS14.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[13] = DS14;

		DS15.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[14] = DS15;

		DS16.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[15] = DS16;

		DS17.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[16] = DS17;

		DS18.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[17] = DS18;

		DS19.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[18] = DS19;

		DS20.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[19] = DS20;

		DS21.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[20] = DS21;

		DS22.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[21] = DS22;

		DS23.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[22] = DS23;

		DS24.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[23] = DS24;

		DS25.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[24] = DS25;

		DS26.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});
		descriptorSets[25] = DS26;

		DSGubo.init(this, &DSLGubo, {
					{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
			});

		txt.pipelinesAndDescriptorSetsInit();
	}

	// Here you destroy your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsCleanup() {
		P1.cleanup();
		
		for (DescriptorSet ds : descriptorSets) {
			ds.cleanup();
		}

		DSGubo.cleanup();

		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// You also have to destroy the pipelines
	void localCleanup() {
		T1.cleanup();

		Cube.cleanup();

		DSL.cleanup();
		DSLGubo.cleanup();
		
		P1.destroy();		
		
		txt.localCleanup();
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		DSGubo.bind(commandBuffer, P1, 0, currentImage);

		P1.bind(commandBuffer);
		Cube.bind(commandBuffer);

		for (DescriptorSet ds : descriptorSets) {
			ds.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);
		}

		DS24.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS25.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS26.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		txt.populateCommandBuffer(commandBuffer, currentImage);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		static auto startTime = std::chrono::high_resolution_clock::now();
		static float lastTime = 0.0f;
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>
			(currentTime - startTime).count();
		float deltaT = time - lastTime;
		lastTime = time;
		const float rotSpeed = glm::radians(45.0f);

		static bool debounce = false;
		static int curDebounce = 0;

		static bool showNormal = false;
		static bool showUV = false;
		static bool rotateds = false;
		static bool rotatedw = false;
		static bool changed = false;

		
		if(glfwGetKey(window, GLFW_KEY_N)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_N;
				showNormal = !showNormal;
				showUV = false;
			}
		} else {
			if((curDebounce == GLFW_KEY_N) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		if(glfwGetKey(window, GLFW_KEY_U)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_U;
				showNormal = false;
				showUV = !showUV;
			}
		} else {
			if((curDebounce == GLFW_KEY_U) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}
		

		if (glfwGetKey(window, GLFW_KEY_S)) {
			if (rotateds == false) {
				rotateds = true;
				rotateFace(cube, selFace, -1);
			}
			else {
			}
		}
		else {
			rotateds = false;
		}
		if (glfwGetKey(window, GLFW_KEY_W)) {
			if (rotatedw == false) {
				rotatedw = true;
				rotateFace(cube, selFace, 1);
			}
			else {
			}
		}
		else {
			rotatedw = false;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			if (changed == false) {
				changed = true;
				selFace++;
				selFace = selFace % 9;

				std::cout << selFace;
			}
			else {
			}
		}
		else {
			changed = false;
		}

		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}


		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		// update camera
		GameLogic();
										
		// Here is where you actually update your uniforms

		// updates global uniforms
		gubo.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = cameraPos;

		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);

		static int a = 2;
		static int b = 0;
		static int c = 1;

		float ambIntensity = 5.0f;

		// top front
		ubo1.amb = ambIntensity; ubo1.gamma = 180.0f; ubo1.sColor = glm::vec3(1.0f);
		ubo1.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 2.1, 2.1));
		ubo1.mvpMat = ViewPrj * Rotations[0][a] * Rotations[0][b] * Rotations[0][c] * ubo1.mMat;
		descriptorSets[0].map(currentImage, &ubo1, sizeof(ubo1), 0);

		ubo2.amb = ambIntensity; ubo2.gamma = 180.0f; ubo2.sColor = glm::vec3(1.0f);
		ubo2.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 2.1, 2.1));
		ubo2.mvpMat = ViewPrj * Rotations[1][a] * Rotations[1][b] * Rotations[1][c] * ubo2.mMat;
		descriptorSets[1].map(currentImage, &ubo2, sizeof(ubo2), 0);

		ubo3.amb = ambIntensity; ubo3.gamma = 180.0f; ubo3.sColor = glm::vec3(1.0f);
		ubo3.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 2.1, 2.1));
		ubo3.mvpMat = ViewPrj * Rotations[2][a] * Rotations[2][b] * Rotations[2][c] * ubo3.mMat;
		descriptorSets[2].map(currentImage, &ubo3, sizeof(ubo3), 0);

		// middle front
		ubo4.amb = ambIntensity; ubo4.gamma = 180.0f; ubo4.sColor = glm::vec3(1.0f);
		ubo4.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 0, 2.1));
		ubo4.mvpMat = ViewPrj * Rotations[3][a] * Rotations[3][b] * Rotations[3][c] * ubo4.mMat;
		descriptorSets[3].map(currentImage, &ubo4, sizeof(ubo4), 0);

		ubo5.amb = ambIntensity; ubo5.gamma = 180.0f; ubo5.sColor = glm::vec3(1.0f);
		ubo5.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 0, 2.1));
		ubo5.mvpMat = ViewPrj * Rotations[4][a] * Rotations[4][b] * Rotations[4][c] * ubo5.mMat;
		descriptorSets[4].map(currentImage, &ubo5, sizeof(ubo5), 0);

		ubo6.amb = ambIntensity; ubo6.gamma = 180.0f; ubo6.sColor = glm::vec3(1.0f);
		ubo6.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 0, 2.1));
		ubo6.mvpMat = ViewPrj * Rotations[5][a] * Rotations[5][b] * Rotations[5][c] * ubo6.mMat;
		descriptorSets[5].map(currentImage, &ubo6, sizeof(ubo6), 0);

		// bottom front
		ubo7.amb = ambIntensity; ubo7.gamma = 180.0f; ubo7.sColor = glm::vec3(1.0f);
		ubo7.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, -2.1, 2.1));
		ubo7.mvpMat = ViewPrj * Rotations[6][a] * Rotations[6][b] * Rotations[6][c] * ubo7.mMat;
		descriptorSets[6].map(currentImage, &ubo7, sizeof(ubo7), 0);

		ubo8.amb = ambIntensity; ubo8.gamma = 180.0f; ubo8.sColor = glm::vec3(1.0f);
		ubo8.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, -2.1, 2.1));
		ubo8.mvpMat = ViewPrj * Rotations[7][a] * Rotations[7][b] * Rotations[7][c] * ubo8.mMat;
		descriptorSets[7].map(currentImage, &ubo8, sizeof(ubo8), 0);

		ubo9.amb = ambIntensity; ubo9.gamma = 180.0f; ubo9.sColor = glm::vec3(1.0f);
		ubo9.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, -2.1, 2.1));
		ubo9.mvpMat = ViewPrj * Rotations[8][a] * Rotations[8][b] * Rotations[8][c] * ubo9.mMat;
		descriptorSets[8].map(currentImage, &ubo9, sizeof(ubo9), 0);
		
		// top middle
		ubo10.amb = ambIntensity; ubo10.gamma = 180.0f; ubo10.sColor = glm::vec3(1.0f);
		ubo10.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 2.1, 0));
		ubo10.mvpMat = ViewPrj * Rotations[9][a] * Rotations[9][b] * Rotations[9][c] * ubo10.mMat;
		descriptorSets[9].map(currentImage, &ubo10, sizeof(ubo10), 0);

		ubo11.amb = ambIntensity; ubo11.gamma = 180.0f; ubo11.sColor = glm::vec3(1.0f);
		ubo11.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 2.1, 0));
		ubo11.mvpMat = ViewPrj * Rotations[10][a] * Rotations[10][b] * Rotations[10][c] * ubo11.mMat;
		descriptorSets[10].map(currentImage, &ubo11, sizeof(ubo11), 0);

		ubo12.amb = ambIntensity; ubo12.gamma = 180.0f; ubo12.sColor = glm::vec3(1.0f);
		ubo12.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 2.1, 0));
		ubo12.mvpMat = ViewPrj * Rotations[11][a] * Rotations[11][b] * Rotations[11][c] * ubo12.mMat;
		descriptorSets[11].map(currentImage, &ubo12, sizeof(ubo12), 0);

		// middle middle
		ubo13.amb = ambIntensity; ubo13.gamma = 180.0f; ubo13.sColor = glm::vec3(1.0f);
		ubo13.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 0, 0));
		ubo13.mvpMat = ViewPrj * Rotations[12][a] * Rotations[12][b] * Rotations[12][c] * ubo13.mMat;
		descriptorSets[12].map(currentImage, &ubo13, sizeof(ubo13), 0);

		ubo14.amb = ambIntensity; ubo14.gamma = 180.0f; ubo14.sColor = glm::vec3(1.0f);
		ubo14.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 0, 0));
		ubo14.mvpMat = ViewPrj * Rotations[14][a] * Rotations[14][b] * Rotations[14][c] * ubo14.mMat;
		descriptorSets[13].map(currentImage, &ubo14, sizeof(ubo14), 0);

		// bottom middle
		ubo15.amb = ambIntensity; ubo15.gamma = 180.0f; ubo15.sColor = glm::vec3(1.0f);
		ubo15.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, -2.1, 0));
		ubo15.mvpMat = ViewPrj * Rotations[15][a] * Rotations[15][b] * Rotations[15][c] * ubo15.mMat;
		descriptorSets[14].map(currentImage, &ubo15, sizeof(ubo15), 0);

		ubo16.amb = ambIntensity; ubo16.gamma = 180.0f; ubo16.sColor = glm::vec3(1.0f);
		ubo16.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, -2.1, 0));
		ubo16.mvpMat = ViewPrj * Rotations[16][a] * Rotations[16][b] * Rotations[16][c] * ubo16.mMat;
		descriptorSets[15].map(currentImage, &ubo16, sizeof(ubo16), 0);

		ubo17.amb = ambIntensity; ubo17.gamma = 180.0f; ubo17.sColor = glm::vec3(1.0f);
		ubo17.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, -2.1, 0));
		ubo17.mvpMat = ViewPrj * Rotations[17][a] * Rotations[17][b] * Rotations[17][c] * ubo17.mMat;
		descriptorSets[16].map(currentImage, &ubo17, sizeof(ubo17), 0);

		// top back
		ubo18.amb = ambIntensity; ubo18.gamma = 180.0f; ubo18.sColor = glm::vec3(1.0f);
		ubo18.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 2.1, -2.1));
		ubo18.mvpMat = ViewPrj * Rotations[18][a] * Rotations[18][b] * Rotations[18][c] * ubo18.mMat;
		descriptorSets[17].map(currentImage, &ubo18, sizeof(ubo18), 0);

		ubo19.amb = ambIntensity; ubo19.gamma = 180.0f; ubo19.sColor = glm::vec3(1.0f);
		ubo19.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 2.1, -2.1));
		ubo19.mvpMat = ViewPrj * Rotations[19][a] * Rotations[19][b] * Rotations[19][c] * ubo19.mMat;
		descriptorSets[18].map(currentImage, &ubo19, sizeof(ubo19), 0);

		ubo20.amb = ambIntensity; ubo20.gamma = 180.0f; ubo20.sColor = glm::vec3(1.0f);
		ubo20.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 2.1, -2.1));
		ubo20.mvpMat = ViewPrj * Rotations[20][a] * Rotations[20][b] * Rotations[20][c] * ubo20.mMat;
		descriptorSets[19].map(currentImage, &ubo20, sizeof(ubo20), 0);

		// middle back
		ubo21.amb = ambIntensity; ubo21.gamma = 180.0f; ubo21.sColor = glm::vec3(1.0f);
		ubo21.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 0, -2.1));
		ubo21.mvpMat = ViewPrj * Rotations[21][a] * Rotations[21][b] * Rotations[21][c] * ubo21.mMat;
		descriptorSets[20].map(currentImage, &ubo21, sizeof(ubo21), 0);

		ubo22.amb = ambIntensity; ubo22.gamma = 180.0f; ubo22.sColor = glm::vec3(1.0f);
		ubo22.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 0, -2.1));
		ubo22.mvpMat = ViewPrj * Rotations[22][a] * Rotations[22][b] * Rotations[22][c] * ubo22.mMat;
		descriptorSets[21].map(currentImage, &ubo22, sizeof(ubo22), 0);

		ubo23.amb = ambIntensity; ubo23.gamma = 180.0f; ubo23.sColor = glm::vec3(1.0f);
		ubo23.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 0, -2.1));
		ubo23.mvpMat = ViewPrj * Rotations[23][a] * Rotations[23][b] * Rotations[23][c] * ubo23.mMat;
		descriptorSets[22].map(currentImage, &ubo23, sizeof(ubo23), 0);

		// bottom back
		ubo24.amb = ambIntensity; ubo24.gamma = 180.0f; ubo24.sColor = glm::vec3(1.0f);
		ubo24.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, -2.1, -2.1));
		ubo24.mvpMat = ViewPrj * Rotations[24][a] * Rotations[24][b] * Rotations[24][c] * ubo24.mMat;
		descriptorSets[23].map(currentImage, &ubo24, sizeof(ubo24), 0);

		ubo25.amb = ambIntensity; ubo25.gamma = 180.0f; ubo25.sColor = glm::vec3(1.0f);
		ubo25.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, -2.1, -2.1));
		ubo25.mvpMat = ViewPrj * Rotations[25][a] * Rotations[25][b] * Rotations[25][c] * ubo25.mMat;
		descriptorSets[24].map(currentImage, &ubo25, sizeof(ubo25), 0);

		ubo26.amb = ambIntensity; ubo26.gamma = 180.0f; ubo26.sColor = glm::vec3(1.0f);
		ubo26.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, -2.1, -2.1));
		ubo26.mvpMat = ViewPrj * Rotations[26][a] * Rotations[26][b] * Rotations[26][c] * ubo26.mMat;
		descriptorSets[25].map(currentImage, &ubo26, sizeof(ubo26), 0);
	}
	void rotateFace(int(&cube)[3][3][3], int faceID, int dir) {
		std::cout << faceID;
		float ang = dir * glm::radians(90.0f);
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					std::cout << "/" << cube[i][j][k] << "/";

				}
				std::cout << " ";
			}
			std::cout << '\n';
		}
		if (faceID < 3) {
			//rotation on z axis
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					
					//cube rotation through rotation matrix
					Rotations[cube[faceID][i][j]][2] =
						 glm::mat4(glm::cos(ang), glm::sin(ang), 0, 0,
							-glm::sin(ang), glm::cos(ang), 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1) * Rotations[cube[faceID][i][j]][2];

				}
			}
			//logic model adjustment
			if (dir == 1) {
				int t = cube[faceID % 3][0][0];
				cube[faceID % 3][0][0] = cube[faceID % 3][0][2];
				cube[faceID % 3][0][2] = cube[faceID % 3][2][2];
				cube[faceID % 3][2][2] = cube[faceID % 3][2][0];
				cube[faceID % 3][2][0] = t;
				t = cube[faceID % 3][0][1];
				cube[faceID % 3][0][1] = cube[faceID % 3][1][2];
				cube[faceID % 3][1][2] = cube[faceID % 3][2][1];
				cube[faceID % 3][2][1] = cube[faceID % 3][1][0];
				cube[faceID % 3][1][0] = t;
			}
			if (dir == -1) {
				int t = cube[faceID % 3][0][0];
				cube[faceID % 3][0][0] = cube[faceID % 3][2][0];
				cube[faceID % 3][2][0] = cube[faceID % 3][2][2];
				cube[faceID % 3][2][2] = cube[faceID % 3][0][2];
				cube[faceID % 3][0][2] = t;
				t = cube[faceID % 3][0][1];
				cube[faceID % 3][0][1] = cube[faceID % 3][1][0];
				cube[faceID % 3][1][0] = cube[faceID % 3][2][1];
				cube[faceID % 3][2][1] = cube[faceID % 3][1][2];
				cube[faceID % 3][1][2] = t;
			}
		}

		else if (faceID < 6) {
			//rotation on y axis
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					Rotations[cube[j][faceID % 3][i]][1] =
						glm::mat4(glm::cos(ang), 0, glm::sin(ang), 0,
							0, 1, 0, 0,
							-glm::sin(ang), 0, glm::cos(ang), 0,
							0, 0, 0, 1) * Rotations[cube[j][faceID % 3][i]][1];
				}
			}//logic model adjustment
			if (dir == 1) {
				int t = cube[0][faceID % 3][0];
				cube[0][faceID % 3][0] = cube[0][faceID % 3][2];
				cube[0][faceID % 3][2] = cube[2][faceID % 3][2];
				cube[2][faceID % 3][2] = cube[2][faceID % 3][0];
				cube[2][faceID % 3][0] = t;
				t = cube[0][faceID % 3][1];
				cube[0][faceID % 3][1] = cube[1][faceID % 3][2];
				cube[1][faceID % 3][2] = cube[2][faceID % 3][1];
				cube[2][faceID % 3][1] = cube[1][faceID % 3][0];
				cube[1][faceID % 3][0] = t;
			}
			if (dir == -1) {
				int t = cube[0][faceID % 3][0];
				cube[0][faceID % 3][0] = cube[2][faceID % 3][0];
				cube[2][faceID % 3][0] = cube[2][faceID % 3][2];
				cube[2][faceID % 3][2] = cube[0][faceID % 3][2];
				cube[0][faceID % 3][2] = t;
				t = cube[0][faceID % 3][1];
				cube[0][faceID % 3][1] = cube[1][faceID % 3][0];
				cube[1][faceID % 3][0] = cube[2][faceID % 3][1];
				cube[2][faceID % 3][1] = cube[1][faceID % 3][2];
				cube[1][faceID % 3][2] = t;
			}
		}
		else if (faceID < 9) {
			//rotation on x axis
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					Rotations[cube[j][i][faceID % 3]][0] =
						glm::mat4(1, 0, 0, 0,
							0, glm::cos(ang), glm::sin(ang), 0,
							0, -glm::sin(ang), glm::cos(ang), 0,
							0, 0, 0, 1) * Rotations[cube[j][i][faceID % 3]][0];
				}
			}//logic model adjustment
			if (dir == 1) {
				int t = cube[0][0][faceID % 3];
				cube[0][0][faceID % 3] = cube[0][2][faceID % 3];
				cube[0][2][faceID % 3] = cube[2][2][faceID % 3];
				cube[2][2][faceID % 3] = cube[2][0][faceID % 3];
				cube[2][0][faceID % 3] = t;
				t = cube[0][1][faceID % 3];
				cube[0][1][faceID % 3] = cube[1][2][faceID % 3];
				cube[1][2][faceID % 3] = cube[2][1][faceID % 3];
				cube[2][1][faceID % 3] = cube[1][0][faceID % 3];
				cube[1][0][faceID % 3] = t;
			}
			if (dir == -1) {
				int t = cube[0][0][faceID % 3];
				cube[0][0][faceID % 3] = cube[2][0][faceID % 3];
				cube[2][0][faceID % 3] = cube[2][2][faceID % 3];
				cube[2][2][faceID % 3] = cube[0][2][faceID % 3];
				cube[0][2][faceID % 3] = t;
				t = cube[0][1][faceID % 3];
				cube[0][1][faceID % 3] = cube[1][0][faceID % 3];
				cube[1][0][faceID % 3] = cube[2][1][faceID % 3];
				cube[2][1][faceID % 3] = cube[1][2][faceID % 3];
				cube[1][2][faceID % 3] = t;
			}
		}
		else {

		}
		
	}

	void GameLogic() {
		// Parameters
		// Camera FOV-y, Near Plane and Far Plane
		const float FOVy = glm::radians(45.0f);
		const float nearPlane = 0.1f;
		const float farPlane = 100.f;
		// Camera target height and distance
		const float camHeight = 0;
		const float camDist = 2; // radius
		// Rotation and motion speed
		const float ROT_SPEED = glm::radians(120.0f);

		// Integration with the timers and the controllers
		float deltaT;
		bool fire;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		getSixAxis(deltaT, m, r, fire);

		// Game Logic implementation
		ViewPrj = glm::mat4(1);

		// World
		// Rotation
		
		Pitch -= ROT_SPEED * deltaT * r.x;
		glm::mat4 Mv = glm::mat4(1);
		if (cos(Pitch) >= 0.001) {
			Yaw += ROT_SPEED * deltaT * r.y;
			glm::quat qe = glm::quat(glm::vec3(0, Yaw, 0));
			glm::mat4 World = MakeWorldMatrix(Pos, qe, glm::vec3(1, 1, 1));

			cameraPos = World * glm::vec4(0, camHeight + (camDist * sin(Pitch)), camDist * cos(Pitch), 1);
			Mv = glm::lookAt(cameraPos, Pos, glm::vec3(0, 1, 0));
		}
		else {
			Yaw -= ROT_SPEED * deltaT * r.y;
			glm::quat qe = glm::quat(glm::vec3(0, Yaw, 0));
			glm::mat4 World = MakeWorldMatrix(Pos, qe, glm::vec3(1, 1, 1));
			cameraPos = World * glm::vec4(0, camHeight + (camDist * sin(Pitch)), camDist * cos(Pitch), 1);
			Mv = glm::lookAt(cameraPos, Pos, glm::vec3(0, -1, 0));
		}
		
		glm::mat4 Mp = glm::perspective(glm::radians(90.0f), Ar, 0.1f, 50.0f);
		Mp[1][1] *= -1;

		ViewPrj = Mp * Mv;
	}

	glm::mat4 MakeWorldMatrix(glm::vec3 pos, glm::quat rQ, glm::vec3 size) {
		// creates and returns a World Matrix that positions the object at <pos>,
		// orients it according to <rQ>, and scales it according to the sizes
		// given in vector <size>
		glm::mat4 M =
			glm::translate(glm::mat4(1.0), pos) *
			glm::mat4(rQ);
		glm::scale(glm::mat4(1.0), size);
		return M;
	}
};

// This is the main: probably you do not need to touch this!
int main() {
    Rubiks app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}