// This has been adapted from the Vulkan tutorial

#include "Starter.hpp"
#include "TextMaker.hpp"


 std::vector<SingleText> demoText = {
	{1, {"Rubiks cube", "", "", ""}, 0, 0}
};

// The uniform buffer object used in this example
struct UniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 selector;
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
};

struct Vertex {
	glm::vec3 pos;
	glm::vec2 UV;
};

class Rubiks;
void GameLogic(Rubiks *A, float Ar, glm::mat4 &ViewPrj, glm::mat4 &World);

// MAIN ! 
class Rubiks : public BaseProject {
protected:
	// Here you list all the Vulkan objects you need:
	

	glm::mat4 Rotations[27];
	int selFace = 0;
	int cube[3][3][3] = { {{0, 1, 2}, { 3, 4, 5}, { 6, 7, 8,}},
			{{9, 10, 11}, { 12, 13, 14}, { 15, 16, 17,}},
			{{18, 19, 20}, { 21, 22, 23}, { 24, 25, 26,}} };


	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout DSL1;

	// Pipelines [Shader couples]
	VertexDescriptor VD;
	Pipeline P1;

	// Models, textures and Descriptors (values assigned to the uniforms)
	Model<Vertex> M1;
	DescriptorSet DS1, DS2, DS3, DS4, DS5, DS6, DS7, DS8, DS9, DS10, DS11, DS12, DS13, 
		DS14, DS15, DS16, DS17, DS18, DS19, DS20, DS21, DS22, DS23, DS24, DS25, DS26;
	Texture T1, T2;

	TextMaker txt;
	
	// Other application parameters
	float Ar;
	glm::mat4 ViewPrj;
	glm::vec3 Pos = glm::vec3(0.0f,0.0f,0.0f);
	glm::vec3 cameraPos;
	float Yaw = glm::radians(30.0f);
	float Pitch = glm::radians(22.5f);
	float Roll = glm::radians(0.0f);

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "Rubiks project";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.015f, 0.03f, 1.0f};
		
		// Descriptor pool sizes
		uniformBlocksInPool = 26;
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
			Rotations[i] = glm::mat4(1);
		};

		DSL1.init(this, {
					// this array contains the binding:
					// first  element : the binding number
					// second element : the type of element (buffer or texture)
					// third  element : the pipeline stage where it will be used
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				});

		// Vertex descriptors
		VD.init(this, {
				  {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, UV),
				         sizeof(glm::vec2), UV}
				});

		// Pipelines [Shader couples]
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		P1.init(this, &VD, "shaders/ShaderVert.spv", "shaders/ShaderFrag.spv", {&DSL1});
		P1.setAdvancedFeatures(VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, false);

		// Models, textures and Descriptors (values assigned to the uniforms)
		M1.init(this, &VD, "Models/Cube.obj", OBJ);

		T1.init(this, "textures/Checker.png");
		
		txt.init(this, &demoText);
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		P1.create();

		DS1.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
				});

		DS2.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
				});

		DS3.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS4.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS5.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS6.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS7.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS8.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS9.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS10.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS11.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS12.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS13.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS14.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS15.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS16.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS17.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS18.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS19.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS20.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS21.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS22.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS23.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS24.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS25.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS26.init(this, &DSL1, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		txt.pipelinesAndDescriptorSetsInit();
	}

	// Here you destroy your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsCleanup() {
		P1.cleanup();
		
		DS1.cleanup();
		DS2.cleanup();
		DS3.cleanup();
		DS4.cleanup();
		DS5.cleanup();
		DS6.cleanup();
		DS7.cleanup();
		DS8.cleanup();
		DS9.cleanup();
		DS10.cleanup();
		DS11.cleanup();
		DS12.cleanup();
		DS13.cleanup();
		DS14.cleanup();
		DS15.cleanup();
		DS16.cleanup();
		DS17.cleanup();
		DS18.cleanup();
		DS19.cleanup();
		DS20.cleanup();
		DS21.cleanup();
		DS22.cleanup();
		DS23.cleanup();
		DS24.cleanup();
		DS25.cleanup();
		DS26.cleanup();

		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// You also have to destroy the pipelines
	void localCleanup() {
		T1.cleanup();

		M1.cleanup();

		DSL1.cleanup();
		
		P1.destroy();		
		
		txt.localCleanup();
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		P1.bind(commandBuffer);
		M1.bind(commandBuffer);

		DS1.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS2.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS3.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS4.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS5.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS6.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS7.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS8.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS9.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS10.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS11.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS12.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS13.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS14.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS15.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS16.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS17.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS18.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS19.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS20.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS21.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS22.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS23.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS24.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS25.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

		DS26.bind(commandBuffer, P1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

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

		
		GameLogic();
		
		UniformBufferObject ubo{};								
		// Here is where you actually update your uniforms

		// updates global uniforms
		ubo.mMat = glm::mat4(1);
		ubo.mvpMat = ViewPrj;
		ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
		
		float dang = Pitch + glm::radians(15.0f);

		GlobalUniformBufferObject gubo{};

		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(3));
		ubo.mvpMat = ViewPrj * ubo.mMat;
		ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));

		gubo.selector = glm::vec3(showNormal || showUV ? 0 : 1, showNormal ? 1 : 0, showUV ? 1 : 0);
		gubo.lightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.eyePos = cameraPos;

		// top front
		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 2.1, 2.1));
		ubo.mvpMat = ViewPrj * Rotations[0] * ubo.mMat;
		DS1.map(currentImage, &ubo, sizeof(ubo), 0);

		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 2.1, 2.1));
		ubo.mvpMat = ViewPrj * Rotations[1] * ubo.mMat;
		DS2.map(currentImage, &ubo, sizeof(ubo), 0);

		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 2.1, 2.1));
		ubo.mvpMat = ViewPrj * Rotations[2] * ubo.mMat;
		DS3.map(currentImage, &ubo, sizeof(ubo), 0);

		// middle front
		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 0, 2.1));
		ubo.mvpMat = ViewPrj * Rotations[3] * ubo.mMat;
		DS4.map(currentImage, &ubo, sizeof(ubo), 0);

		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 0, 2.1));
		ubo.mvpMat = ViewPrj * Rotations[4] * ubo.mMat;
		DS5.map(currentImage, &ubo, sizeof(ubo), 0);

		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 0, 2.1));
		ubo.mvpMat = ViewPrj * Rotations[5] * ubo.mMat;
		DS6.map(currentImage, &ubo, sizeof(ubo), 0);

		// bottom front
		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, -2.1, 2.1));
		ubo.mvpMat = ViewPrj * Rotations[6] * ubo.mMat;
		DS7.map(currentImage, &ubo, sizeof(ubo), 0);

		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, -2.1, 2.1));
		ubo.mvpMat = ViewPrj * Rotations[7] * ubo.mMat;
		DS8.map(currentImage, &ubo, sizeof(ubo), 0);

		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, -2.1, 2.1));
		ubo.mvpMat = ViewPrj * Rotations[8] * ubo.mMat;
		DS9.map(currentImage, &ubo, sizeof(ubo), 0);

		// top middle
		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 2.1, 0));
		ubo.mvpMat = ViewPrj * Rotations[9] * ubo.mMat;
		DS10.map(currentImage, &ubo, sizeof(ubo), 0);


		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 2.1, 0));
		ubo.mvpMat = ViewPrj * Rotations[10] * ubo.mMat;
		DS11.map(currentImage, &ubo, sizeof(ubo), 0);


		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 2.1, 0));
		ubo.mvpMat = ViewPrj * Rotations[11] * ubo.mMat;
		DS12.map(currentImage, &ubo, sizeof(ubo), 0);


		// middle middle
		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 0, 0));
		ubo.mvpMat = ViewPrj * Rotations[12] * ubo.mMat;
		DS13.map(currentImage, &ubo, sizeof(ubo), 0);


		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 0, 0));
		ubo.mvpMat = ViewPrj * Rotations[14] * ubo.mMat;
		DS14.map(currentImage, &ubo, sizeof(ubo), 0);


		// bottom middle
		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, -2.1, 0));
		ubo.mvpMat = ViewPrj * Rotations[15] * ubo.mMat;
		DS15.map(currentImage, &ubo, sizeof(ubo), 0);


		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, -2.1, 0));
		ubo.mvpMat = ViewPrj * Rotations[16] * ubo.mMat;
		DS16.map(currentImage, &ubo, sizeof(ubo), 0);


		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, -2.1, 0));
		ubo.mvpMat = ViewPrj * Rotations[17] * ubo.mMat;
		DS17.map(currentImage, &ubo, sizeof(ubo), 0);


		// top back
		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 2.1, -2.1));
		ubo.mvpMat = ViewPrj * Rotations[18] * ubo.mMat;
		DS18.map(currentImage, &ubo, sizeof(ubo), 0);


		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 2.1, -2.1));
		ubo.mvpMat = ViewPrj * Rotations[19] * ubo.mMat;
		DS19.map(currentImage, &ubo, sizeof(ubo), 0);


		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 2.1, -2.1));
		ubo.mvpMat = ViewPrj * Rotations[20] * ubo.mMat;
		DS20.map(currentImage, &ubo, sizeof(ubo), 0);


		// middle back
		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 0, -2.1));
		ubo.mvpMat = ViewPrj * Rotations[21] * ubo.mMat;
		DS21.map(currentImage, &ubo, sizeof(ubo), 0);


		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 0, -2.1));
		ubo.mvpMat = ViewPrj * Rotations[22] * ubo.mMat;
		DS22.map(currentImage, &ubo, sizeof(ubo), 0);


		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 0, -2.1));
		ubo.mvpMat = ViewPrj * Rotations[23] * ubo.mMat;
		DS23.map(currentImage, &ubo, sizeof(ubo), 0);


		// bottom back
		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, -2.1, -2.1));
		ubo.mvpMat = ViewPrj * Rotations[24] * ubo.mMat;
		DS24.map(currentImage, &ubo, sizeof(ubo), 0);


		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, -2.1, -2.1));
		ubo.mvpMat = ViewPrj * Rotations[25] * ubo.mMat;
		DS25.map(currentImage, &ubo, sizeof(ubo), 0);


		ubo.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, -2.1, -2.1));
		ubo.mvpMat = ViewPrj * Rotations[26] * ubo.mMat;
		DS26.map(currentImage, &ubo, sizeof(ubo), 0);


	}
	void rotateFace(int(&cube)[3][3][3], int faceID, int dir) {
		float ang = dir * glm::radians(90.0f);
		if (faceID < 3) {
			//rotation on z axis
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					
					//cube rotation through rotation matrix
					Rotations[cube[faceID][j][i]] =
						 glm::mat4(glm::cos(ang), glm::sin(ang), 0, 0,
							-glm::sin(ang), glm::cos(ang), 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1) * Rotations[cube[faceID][j][i]];

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
					Rotations[cube[j][faceID % 3][i]] =
						glm::mat4(glm::cos(ang), 0, glm::sin(ang), 0,
							0, 1, 0, 0,
							-glm::sin(ang), 0, glm::cos(ang), 0,
							0, 0, 0, 1) * Rotations[cube[j][faceID % 3][i]];
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
					Rotations[cube[j][i][faceID % 3]] = 
						glm::mat4(1, 0, 0, 0,
							0, glm::cos(ang), glm::sin(ang), 0,
							0, -glm::sin(ang), glm::cos(ang), 0,
							0, 0, 0, 1) * Rotations[cube[j][i][faceID % 3]];
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
		const float camDist = 2;
		// Camera Pitch limits
		const float minPitch = glm::radians(-60.0f);
		const float maxPitch = glm::radians(60.0f);
		// Rotation and motion speed
		const float ROT_SPEED = glm::radians(120.0f);
		const float MOVE_SPEED = 2.0f;

		// Integration with the timers and the controllers
		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);

		// Game Logic implementation
		// Current Player Position - statc variable make sure its value remain unchanged in subsequent calls to the procedure

		// To be done in the assignment
		ViewPrj = glm::mat4(1);
		glm::mat4 World = glm::mat4(1);
		
		// World
		// Position
		glm::vec3 ux = glm::rotate(glm::mat4(1.0f), Yaw, glm::vec3(0,1,0)) * glm::vec4(1,0,0,1);
		glm::vec3 uz = glm::rotate(glm::mat4(1.0f), Yaw, glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1);
		Pos = Pos + MOVE_SPEED * m.x * ux * deltaT;
		Pos = Pos + MOVE_SPEED * m.y * glm::vec3(0,1,0) * deltaT;
		Pos.y = Pos.y < 0.0f ? 0.0f : Pos.y;
		Pos = Pos + MOVE_SPEED * m.z * uz * deltaT;
		// Rotation
		Yaw = Yaw - ROT_SPEED * deltaT * r.y;
		Pitch = Pitch + ROT_SPEED * deltaT * r.x;

		World = MakeWorldMatrix(Pos, glm::quat(glm::vec3(0, 0, 0)), glm::vec3(1, 1, 1));

		// camera rotation
		glm::quat qe = glm::quat(glm::vec3(0, Yaw, 0));
		glm::mat4 World2 = MakeWorldMatrix(Pos, qe, glm::vec3(1, 1, 1));

		glm::vec3 camPos = World2 * glm::vec4(0, camHeight + (camDist * sin(Pitch)), camDist * cos(Pitch), 1);
		glm::vec3 LookAt = Pos + glm::vec3(0, camHeight, 0);

		ViewPrj = MakeViewProjectionMatrix(Ar, camPos, LookAt);
	}

	glm::mat4 MakeViewProjectionMatrix(float Ar, glm::vec3 camPos, glm::vec3 Pos) {
		glm::mat4 Mp = glm::perspective(glm::radians(90.0f), Ar, 0.1f, 50.0f);
		Mp[1][1] *= -1;

		glm::mat4 Mv = glm::lookAt(camPos, Pos, glm::vec3(0, 1, 0));

		return Mp * Mv;
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