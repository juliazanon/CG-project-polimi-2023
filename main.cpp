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

	glm::mat4 Rotations[27];
	glm::mat4 FinRotations[27];
	int faceID = 0;
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
			//for (int j = 0; j < 3; j++) {
				Rotations[i] = glm::mat4(1);
				FinRotations[i] = glm::mat4(1);
			//}
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

		DS1.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
				});

		DS2.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
				});

		DS3.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS4.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS5.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS6.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS7.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS8.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS9.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS10.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS11.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS12.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS13.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS14.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS15.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS16.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS17.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS18.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS19.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS20.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS21.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS22.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS23.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS24.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS25.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DS26.init(this, &DSL, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &T1}
			});

		DSGubo.init(this, &DSLGubo, {
					{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
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

		DS1.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS2.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS3.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS4.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS5.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS6.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS7.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS8.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS9.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS10.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS11.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS12.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS13.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS14.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS15.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS16.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS17.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS18.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS19.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS20.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS21.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS22.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

		DS23.bind(commandBuffer, P1, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Cube.indices.size()), 1, 0, 0, 0);

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


	float rotSpeed = glm::radians(90.0f);
	const float rotRange = glm::radians(90.0f);
	float faceRot = 0.0f;
	float totFaceRot = 0.0f;

	int totShuff = 0;
	int currShuff = 0;

	int tl = 0;
	int tr = 0;
	int td = 0;
	int tu = 0;
	int tf = 0;
	int tb = 0;
	int tab = 0;
	int  *faces[6] = {&tl, &tr, &td, &tu, &tf, &tb};

	int face = 0;

	bool rotating = false;
	bool shuffling = false;
	int dir = 0;

	//create temporary variables for the rotation matrices at the beginning of the rotation
		//to be used when setting the correct deltaT independent rotation
	glm::mat4 startRot[3][3];

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		
		static bool debounce = false;
		static int curDebounce = 0;

		static bool showNormal = false;
		static bool showUV = false;
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
		/*
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
		*/

		if (currShuff == 0 && totShuff == 0) {
			tl = glfwGetKey(window, GLFW_KEY_L);
			tr = glfwGetKey(window, GLFW_KEY_R);
			tu = glfwGetKey(window, GLFW_KEY_U);
			td = glfwGetKey(window, GLFW_KEY_D);
			tf = glfwGetKey(window, GLFW_KEY_F);
			tb = glfwGetKey(window, GLFW_KEY_B);
			tab = glfwGetKey(window, GLFW_KEY_TAB);
		}

		if (tl + tr + tu + td + tf + tb) {
			if (changed == false && rotating == false) {
				changed = true;
				if (tf) faceID = 0;
				if (tb) faceID = 2;
				if (tu) faceID = 3;
				if (td) faceID = 5;
				if (tr) faceID = 6;
				if (tl) faceID = 8;
				std::cout << "reset";
			}
			else {
			}
		}
		else {
			changed = false;
		}


		// Integration with the timers and the controllers
		float deltaT;
		bool fire;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		getSixAxis(deltaT, m, r, fire);

		

		if (tl+tr+tu+td+tf+tb) {

			if (rotating == false) {

				rotating = true;
				dir = 2 * tab - 1;
				rotateFace(cube, faceID, dir);
				

				if (faceID < 3) {
					//rotation on z axis
					for (int i = 0; i < 3; i++) {
						for (int j = 0; j < 3; j++) {
							//cube rotation through rotation matrix
							startRot[i][j] = Rotations[cube[faceID][i][j]];
						}
					}
					
				}
				else if (faceID < 6) {
					//rotation on y axis
					for (int i = 0; i < 3; i++) {
						for (int j = 0; j < 3; j++) {
							startRot[i][j] = Rotations[cube[j][faceID % 3][i]];
						}
					}
				}
				else {
					//rotation on x axis
					for (int i = 0; i < 3; i++) {
						for (int j = 0; j < 3; j++) {
							startRot[i][j] = Rotations[cube[j][i][faceID % 3]];
						}
					}
				}

			}
		}
		else {
		}



		float ang = glm::radians(90.0f);
		
		//ending rotation
		if (rotating == true && totFaceRot >= ang) {
			rotating = false;

			faceRot = 0.0f;
			totFaceRot = 0.0f;

			//set face position to the correct one, independent from deltaT at the end of the rotation

			if (faceID < 3) {
				//rotation on z axis
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						//cube rotation through rotation matrix
						Rotations[cube[faceID][i][j]] =
							glm::mat4(glm::cos(dir * ang), glm::sin(dir * ang), 0, 0,
								-glm::sin(dir * ang), glm::cos(dir * ang), 0, 0,
								0, 0, 1, 0,
								0, 0, 0, 1) * 
							startRot[i][j];
					}
				}
			}

			else if (faceID < 6) {
				//rotation on y axis
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						Rotations[cube[j][faceID % 3][i]] =
							glm::mat4(glm::cos(dir * ang), 0, glm::sin(dir * ang), 0,
								0, 1, 0, 0,
								-glm::sin(dir * ang), 0, glm::cos(dir * ang), 0,
								0, 0, 0, 1) * startRot[i][j];
					}
				}
			}
			else {
				//rotation on x axis
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						Rotations[cube[j][i][faceID % 3]] =
							glm::mat4(1, 0, 0, 0,
								0, glm::cos(-(dir * ang)), glm::sin(-(dir * ang)), 0,
								0, -glm::sin(-(dir * ang)), glm::cos(-(dir * ang)), 0,
								0, 0, 0, 1) * startRot[i][j];
					}
				}
			}
		}

		if (rotating == true) {

			faceRot = rotSpeed * deltaT;
			totFaceRot += rotSpeed * deltaT;

			if (faceID < 3) {
				//rotation on z axis
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {

						//cube rotation through rotation matrix
						Rotations[cube[faceID][i][j]] =
							glm::mat4(glm::cos(dir*faceRot), glm::sin(dir * faceRot), 0, 0,
								-glm::sin(dir * faceRot), glm::cos(dir * faceRot), 0, 0,
								0, 0, 1, 0,
								0, 0, 0, 1) * Rotations[cube[faceID][i][j]];	
					}
				}
				
			}

			else if (faceID < 6) {
				//rotation on y axis
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						Rotations[cube[j][faceID % 3][i]] =
							glm::mat4(glm::cos(dir * faceRot), 0, glm::sin(dir * faceRot), 0,
								0, 1, 0, 0,
								-glm::sin(dir * faceRot), 0, glm::cos(dir * faceRot), 0,
								0, 0, 0, 1) * Rotations[cube[j][faceID % 3][i]];
					}
				}
			}
			else {
				//rotation on x axis
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						Rotations[cube[j][i][faceID % 3]] =
							glm::mat4(1, 0, 0, 0,
								0, glm::cos(-(dir * faceRot)), glm::sin(-(dir * faceRot)), 0,
								0, -glm::sin(-(dir * faceRot)), glm::cos(-(dir * faceRot)), 0,
								0, 0, 0, 1) * Rotations[cube[j][i][faceID % 3]];
					}
				}
			}
		}
		else {
		}

		if (glfwGetKey(window, GLFW_KEY_S)) {
			if (shuffling == false && rotating == false){
				shuffling = true;
			}
		}
		else{
			if (shuffling == true) {
				totShuff = rand() % 20 + 10;
				rotSpeed = glm::radians(450.0f);
			}

		}
		if (rotating == false) {
			if (currShuff < totShuff) {
				currShuff++;
				*faces[face] = 0;
				int tab = rand() % 2;
				face = rand() % 6;
				*faces[face] = 1;
				if (tf) faceID = 0;
				if (tb) faceID = 2;
				if (tu) faceID = 3;
				if (td) faceID = 5;
				if (tr) faceID = 6;
				if (tl) faceID = 8;
			} else if (currShuff == totShuff && totShuff != 0) {
				rotSpeed = glm::radians(90.0f);
				currShuff = 0;
				totShuff = 0;
				shuffling = false;
			}
		}
		else {
		}


		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}


		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		

		// update camera
		GameLogic(deltaT, m, r, fire);
										
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

		for (int i = 0; i < 27; i++) {
			FinRotations[i] = Rotations[i];
		}

		// top front
		ubo1.amb = ambIntensity; ubo1.gamma = 180.0f; ubo1.sColor = glm::vec3(1.0f);
		ubo1.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 2.1, 2.1));
		ubo1.mvpMat = ViewPrj * FinRotations[0] * ubo1.mMat;
		DS1.map(currentImage, &ubo1, sizeof(ubo1), 0);

		ubo2.amb = ambIntensity; ubo2.gamma = 180.0f; ubo2.sColor = glm::vec3(1.0f);
		ubo2.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 2.1, 2.1));
		ubo2.mvpMat = ViewPrj * FinRotations[1] * ubo2.mMat;
		DS2.map(currentImage, &ubo2, sizeof(ubo2), 0);

		ubo3.amb = ambIntensity; ubo3.gamma = 180.0f; ubo3.sColor = glm::vec3(1.0f);
		ubo3.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 2.1, 2.1));
		ubo3.mvpMat = ViewPrj * FinRotations[2]* ubo3.mMat;
		DS3.map(currentImage, &ubo3, sizeof(ubo3), 0);

		// middle front
		ubo4.amb = ambIntensity; ubo4.gamma = 180.0f; ubo4.sColor = glm::vec3(1.0f);
		ubo4.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 0, 2.1));
		ubo4.mvpMat = ViewPrj * FinRotations[3] * ubo4.mMat;
		DS4.map(currentImage, &ubo4, sizeof(ubo4), 0);

		ubo5.amb = ambIntensity; ubo5.gamma = 180.0f; ubo5.sColor = glm::vec3(1.0f);
		ubo5.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 0, 2.1));
		ubo5.mvpMat = ViewPrj * FinRotations[4] * ubo5.mMat;
		DS5.map(currentImage, &ubo5, sizeof(ubo5), 0);

		ubo6.amb = ambIntensity; ubo6.gamma = 180.0f; ubo6.sColor = glm::vec3(1.0f);
		ubo6.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 0, 2.1));
		ubo6.mvpMat = ViewPrj * FinRotations[5] * ubo6.mMat;
		DS6.map(currentImage, &ubo6, sizeof(ubo6), 0);

		// bottom front
		ubo7.amb = ambIntensity; ubo7.gamma = 180.0f; ubo7.sColor = glm::vec3(1.0f);
		ubo7.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, -2.1, 2.1));
		ubo7.mvpMat = ViewPrj * FinRotations[6] * ubo7.mMat;
		DS7.map(currentImage, &ubo7, sizeof(ubo7), 0);

		ubo8.amb = ambIntensity; ubo8.gamma = 180.0f; ubo8.sColor = glm::vec3(1.0f);
		ubo8.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, -2.1, 2.1));
		ubo8.mvpMat = ViewPrj * FinRotations[7] * ubo8.mMat;
		DS8.map(currentImage, &ubo8, sizeof(ubo8), 0);

		ubo9.amb = ambIntensity; ubo9.gamma = 180.0f; ubo9.sColor = glm::vec3(1.0f);
		ubo9.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, -2.1, 2.1));
		ubo9.mvpMat = ViewPrj * FinRotations[8] * ubo9.mMat;
		DS9.map(currentImage, &ubo9, sizeof(ubo9), 0);
		
		// top middle
		ubo10.amb = ambIntensity; ubo10.gamma = 180.0f; ubo10.sColor = glm::vec3(1.0f);
		ubo10.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 2.1, 0));
		ubo10.mvpMat = ViewPrj * FinRotations[9] * ubo10.mMat;
		DS10.map(currentImage, &ubo10, sizeof(ubo10), 0);

		ubo11.amb = ambIntensity; ubo11.gamma = 180.0f; ubo11.sColor = glm::vec3(1.0f);
		ubo11.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 2.1, 0));
		ubo11.mvpMat = ViewPrj * FinRotations[10] * ubo11.mMat;
		DS11.map(currentImage, &ubo11, sizeof(ubo11), 0);

		ubo12.amb = ambIntensity; ubo12.gamma = 180.0f; ubo12.sColor = glm::vec3(1.0f);
		ubo12.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 2.1, 0));
		ubo12.mvpMat = ViewPrj * FinRotations[11] * ubo12.mMat;
		DS12.map(currentImage, &ubo12, sizeof(ubo12), 0);

		// middle middle
		ubo13.amb = ambIntensity; ubo13.gamma = 180.0f; ubo13.sColor = glm::vec3(1.0f);
		ubo13.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 0, 0));
		ubo13.mvpMat = ViewPrj * FinRotations[12] * ubo13.mMat;
		DS13.map(currentImage, &ubo13, sizeof(ubo13), 0);

		ubo14.amb = ambIntensity; ubo14.gamma = 180.0f; ubo14.sColor = glm::vec3(1.0f);
		ubo14.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 0, 0));
		ubo14.mvpMat = ViewPrj * FinRotations[14] * ubo14.mMat;
		DS14.map(currentImage, &ubo14, sizeof(ubo14), 0);

		// bottom middle
		ubo15.amb = ambIntensity; ubo15.gamma = 180.0f; ubo15.sColor = glm::vec3(1.0f);
		ubo15.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, -2.1, 0));
		ubo15.mvpMat = ViewPrj * FinRotations[15] *  ubo15.mMat;
		DS15.map(currentImage, &ubo15, sizeof(ubo15), 0);

		ubo16.amb = ambIntensity; ubo16.gamma = 180.0f; ubo16.sColor = glm::vec3(1.0f);
		ubo16.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, -2.1, 0));
		ubo16.mvpMat = ViewPrj * FinRotations[16] * ubo16.mMat;
		DS16.map(currentImage, &ubo16, sizeof(ubo16), 0);

		ubo17.amb = ambIntensity; ubo17.gamma = 180.0f; ubo17.sColor = glm::vec3(1.0f);
		ubo17.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, -2.1, 0));
		ubo17.mvpMat = ViewPrj * FinRotations[17] * ubo17.mMat;
		DS17.map(currentImage, &ubo17, sizeof(ubo17), 0);

		// top back
		ubo18.amb = ambIntensity; ubo18.gamma = 180.0f; ubo18.sColor = glm::vec3(1.0f);
		ubo18.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 2.1, -2.1));
		ubo18.mvpMat = ViewPrj * FinRotations[18] * ubo18.mMat;
		DS18.map(currentImage, &ubo18, sizeof(ubo18), 0);

		ubo19.amb = ambIntensity; ubo19.gamma = 180.0f; ubo19.sColor = glm::vec3(1.0f);
		ubo19.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 2.1, -2.1));
		ubo19.mvpMat = ViewPrj * FinRotations[19] * ubo19.mMat;
		DS19.map(currentImage, &ubo19, sizeof(ubo19), 0);

		ubo20.amb = ambIntensity; ubo20.gamma = 180.0f; ubo20.sColor = glm::vec3(1.0f);
		ubo20.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 2.1, -2.1));
		ubo20.mvpMat = ViewPrj * FinRotations[20] * ubo20.mMat;
		DS20.map(currentImage, &ubo20, sizeof(ubo20), 0);

		// middle back
		ubo21.amb = ambIntensity; ubo21.gamma = 180.0f; ubo21.sColor = glm::vec3(1.0f);
		ubo21.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, 0, -2.1));
		ubo21.mvpMat = ViewPrj * FinRotations[21] * ubo21.mMat;
		DS21.map(currentImage, &ubo21, sizeof(ubo21), 0);

		ubo22.amb = ambIntensity; ubo22.gamma = 180.0f; ubo22.sColor = glm::vec3(1.0f);
		ubo22.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, 0, -2.1));
		ubo22.mvpMat = ViewPrj * FinRotations[22] * ubo22.mMat;
		DS22.map(currentImage, &ubo22, sizeof(ubo22), 0);

		ubo23.amb = ambIntensity; ubo23.gamma = 180.0f; ubo23.sColor = glm::vec3(1.0f);
		ubo23.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, 0, -2.1));
		ubo23.mvpMat = ViewPrj * FinRotations[23] * ubo23.mMat;
		DS23.map(currentImage, &ubo23, sizeof(ubo23), 0);

		// bottom back
		ubo24.amb = ambIntensity; ubo24.gamma = 180.0f; ubo24.sColor = glm::vec3(1.0f);
		ubo24.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(-2.1, -2.1, -2.1));
		ubo24.mvpMat = ViewPrj * FinRotations[24] * ubo24.mMat;
		DS24.map(currentImage, &ubo24, sizeof(ubo24), 0);

		ubo25.amb = ambIntensity; ubo25.gamma = 180.0f; ubo25.sColor = glm::vec3(1.0f);
		ubo25.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(0, -2.1, -2.1));
		ubo25.mvpMat = ViewPrj * FinRotations[25] * ubo25.mMat;
		DS25.map(currentImage, &ubo25, sizeof(ubo25), 0);

		ubo26.amb = ambIntensity; ubo26.gamma = 180.0f; ubo26.sColor = glm::vec3(1.0f);
		ubo26.mMat = glm::scale(glm::mat4(1), glm::vec3(0.2)) *
			glm::translate(glm::mat4(1), glm::vec3(2.1, -2.1, -2.1));
		ubo26.mvpMat = ViewPrj * FinRotations[26] * ubo26.mMat;
		DS26.map(currentImage, &ubo26, sizeof(ubo26), 0);
		

	}


	void rotateFace(int(&cube)[3][3][3], int faceID, int dir) {
		float ang = dir * glm::radians(90.0f);
		if (faceID < 3) {
			//rotation on z axis
			
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
			//logic model adjustment
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
		else {
			//rotation on x axis
			//logic model adjustment
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
		
	}

	void GameLogic(float deltaT, glm::vec3 m, glm::vec3 r, bool fire) {
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