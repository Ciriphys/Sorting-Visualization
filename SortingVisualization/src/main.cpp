#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include <sstream>
#include <cirigl.h>

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include "SortingEngine.h"
#include "Random.h"
#include "Macro.h"

std::shared_ptr<SortingEngine> SortingEngine::s_Instance = nullptr;
std::shared_ptr<Random>	Random::s_Instance = nullptr;

DataAnalysis SortingEngine::s_DataAnalyzer;

GLFWwindow* InitAPI();
void InitializeImGui(GLFWwindow*);
void DeInit();

int main(int argc, char const** argv)
{
	GLFWwindow* mainWindow = InitAPI();
	if (!mainWindow) { return -1; }

	StartOpenGLDebugging();

	/* Initializing Variables */
	uint32_t segmentNumber = 200;
	uint32_t previousSegmentNumber = segmentNumber;

	float vBuffer[] =
	{
		 0.0f,   0.0f,
		 0.0f,   100.0f,
		 100.0f, 100.0f,
		 100.0f, 0.0f
	};

	uint32_t iBuffer[] = { 0, 1, 2, 2, 3, 0 };

	VertexArray* vao = new VertexArray;
	VertexBuffer* vbo = new VertexBuffer(sizeof(vBuffer), vBuffer);
	IndexBuffer* ibo = new IndexBuffer(sizeof(iBuffer), iBuffer);

	BufferLayout* bufferLayout = new BufferLayout;
	bufferLayout->Push<float>(2);
	vao->AddLayout(*vbo, *bufferLayout);

	vao->AttachIndexBuffer(ibo);

	Shader* shader = new Shader("assets/shader/segment.glsl");

	glm::mat4 projectionMatrix = glm::ortho(-HALF_WIDTH_F, HALF_WIDTH_F, -HALF_HEIGHT_F, HALF_HEIGHT_F, 0.1f, 100.0f);
	glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	
	std::vector<uint32_t> segmentVector(segmentNumber, -1);

	for (uint32_t i = 0; i < segmentNumber; i++)
		segmentVector[i] = i;

	/*  Initializing Array visualizer boundaries variables */
	float const xOffset = 50.0f;
	float const yOffset = (1 / (SCREEN_RATIO)) * xOffset;
	float gridWidth = WINDOW_WIDTH_F - 2.0f * xOffset;
	float gridHeight = (1 / (SCREEN_RATIO)) * gridWidth;

	/* Initializing control panel widget variables*/
	glm::vec4 startColor = glm::vec4(RGBA(255, 75, 26, 255));
	glm::vec4 endColor = glm::vec4(RGBA(47, 71, 210, 255));
	
	bool useColor = true;
	bool useDots = false;

	/* Initializing rendering utility variables */
	uint32_t lastAccessCount = 0, lastSwapCount = 0, lastObjectCopies = 0;
	bool shouldContinue = true;
	float lastFrameTime = 0.0f;
	auto sortingEngine = SortingEngine::Get();
	auto sortingEngine2 = SortingEngine::Get();

	/* Initialize ImGui */
	InitializeImGui(mainWindow);

	while (!glfwWindowShouldClose(mainWindow))
	{
		glClearColor(RGBA(10, 10, 10, 255));
		glClear(GL_COLOR_BUFFER_BIT);

		vao->Enable();
		shader->Enable();

		float currentFrameTime = (float)glfwGetTime();
		float deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		ImGui_ImplGlfwGL3_NewFrame();

		/* Check if sorting has ended */
		if (sortingEngine->GetRenderQueue().size() == 0)
		{
			if (glfwGetKey(mainWindow, GLFW_KEY_ENTER) == GLFW_PRESS || glfwGetMouseButton(mainWindow, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
			{
				shouldContinue = true;
				lastAccessCount = 0;
				lastSwapCount = 0;
			}
		}

		/* Display control panel widget if not sorting */
		if(sortingEngine->GetRenderQueue().size() == 0 && shouldContinue)
		{
			ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowPos({ 10, 10 });
			ImGui::DragInt("Segments number", (int*)&segmentNumber, 1, 200, 1200);
			ImGui::ColorEdit4("Start Color", &startColor[0]);
			ImGui::ColorEdit4("End Color", &endColor[0]);
			ImGui::Checkbox("Use interpolated color", &useColor);
			if (ImGui::Checkbox("Use Dots", &useDots));

			static int sortingIndex = -1;
			if (ImGui::Combo("Sorting Algorithm", &sortingIndex, "Bubble Sort\0Radix Sort\0Selection Sort\0Insertion Sort\0Heap Sort\0Quick Sort\0Merge Sort\0Shell Sort\0Cocktail Shaker Sort\0"))
			{
				shouldContinue = false;
				sortingEngine->operator[]({ "Shuffle", segmentVector });

				switch (sortingIndex)
				{
				case 0: 
					sortingEngine->operator[]({ "Bubble Sort" , segmentVector });
					break;
				case 1: 
					sortingEngine->operator[]({ "Radix Sort" , segmentVector });
					break;
				case 2:
					sortingEngine->operator[]({ "Selection Sort" , segmentVector });
					break;
				case 3:
					sortingEngine->operator[]({ "Insertion Sort" , segmentVector });
					break;
				case 4:
					sortingEngine->operator[]({ "Heap Sort" , segmentVector });
					break;
				case 5:
					sortingEngine->operator[]({ "Quick Sort" , segmentVector });
					break;
				case 6:
					sortingEngine->operator[]({ "Merge Sort" , segmentVector });
					break;
				case 7:
					sortingEngine->operator[]({ "Shell Sort" , segmentVector });
					break;
				case 8:
					sortingEngine->operator[]({ "Cocktail Shaker Sort" , segmentVector });
					break;
				}
			}
			ImGui::End();
		}
		else
		{
			/* Display Information Panel if sorting */
			ImGui::Begin("Information Panel", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowPos({ 10, 10 });
			if (lastAccessCount == 0 && lastSwapCount == 0 && lastObjectCopies == 0)
				ImGui::Text("Shuffling...");
			else
				ImGui::Text("Access Count: %d, Swap Count: %d, Values Copied: %d", lastAccessCount, lastSwapCount, lastObjectCopies);
			if(sortingEngine->GetRenderQueue().size() == 0)
				ImGui::Text("Press Enter or Click to continue...");
			ImGui::End();
		}

		/* Clamp segmentNumber between 200 and 1200 and resize the array according to segmentNumber */
		if (previousSegmentNumber != segmentNumber)
		{
			if (segmentNumber <= 1200 && segmentNumber >= 200)
			{
				segmentVector.resize(segmentNumber);
				if(previousSegmentNumber < segmentNumber)
					for (uint32_t i = previousSegmentNumber; i < segmentNumber; i++)
						segmentVector[i] = i;

				previousSegmentNumber = segmentNumber;
			}
			else
				segmentNumber = previousSegmentNumber;
		}

		/* 
			Compute segments size related to internal grid
			100.0f is the size of a quad in pixels 
		*/
		float widthInPixels = gridWidth / segmentNumber;
		float heightInPixels = gridHeight / segmentNumber;
		float xScalingFactor = widthInPixels / 100.0f;	
		float yScalingFactor = heightInPixels / 100.0f;
		float width = -HALF_WIDTH_F + xOffset;
		float height = -HALF_HEIGHT_F + yOffset;

		/* Get current array information if sorting phase has began */
		if (sortingEngine->GetRenderQueue().size() > 0)
		{
			segmentVector = sortingEngine->GetRenderQueue().front().vectorStatus;
			lastAccessCount = sortingEngine->GetRenderQueue().front().dataAnalyzerStatus.accessCount;
			lastSwapCount = sortingEngine->GetRenderQueue().front().dataAnalyzerStatus.swapCount;
			lastObjectCopies = sortingEngine->GetRenderQueue().front().dataAnalyzerStatus.objectCopies;
			sortingEngine->Pop();
		}
		
		/* Render every segment (or dot) according to size and window constraints */
		uint32_t counter = 0;
		for (auto& value : segmentVector)
		{
			glm::mat4 modelMatrix = glm::mat4(1.0f);

			glm::vec3 modelPosition;
			glm::vec3 modelScale;

			if (useDots)
			{
				modelPosition = glm::vec3(
					width + counter * widthInPixels,
					height + value * heightInPixels,
					0.0f
				);

				modelScale = glm::vec3(
					xScalingFactor,
					yScalingFactor,
					1.0f
				);
			}
			else
			{
				modelPosition = glm::vec3(
					width + counter * widthInPixels,
					height,
					0.0f
				);

				modelScale = glm::vec3(
					xScalingFactor,
					yScalingFactor * (value + 1),
					1.0f
				);
			}

			modelMatrix = glm::translate(modelMatrix, modelPosition);
			modelMatrix = glm::scale(modelMatrix, modelScale);

			shader->SetUniformMat4x4f("u_mvp", projectionMatrix * viewMatrix * modelMatrix);

			/*  
				Set uniform color in the fragment shader,
				if useColor is true, the color will be linearly iterpolated between startColor and endColor
				otherwise, the uniform color will be set to white.
			*/
			shader->SetUniformVec4(
				"u_color",
				useColor ? glm::lerp(
					startColor,
					endColor,
					(float)value / (float)segmentNumber
				) : glm::vec4(1.0f)
			);

			glDrawElements(GL_TRIANGLES, ibo->GetCount(), GL_UNSIGNED_INT, nullptr);

			counter++;
		}

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();
		glfwSwapBuffers(mainWindow);
	}
	
	/* Delete pointers */
	delete vao, vbo, ibo, shader, bufferLayout;

	DeInit();
	return 0;
}

void InitializeImGui(GLFWwindow* window)
{
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);

	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("assets/fonts/ruda-bold.ttf", 14);

	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(10, 10);
	style->WindowRounding = 0.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(RGBA(25, 25, 25, 255)); // 0.06f, 0.05f, 0.07f, 1.00f
	style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(RGBA(50, 50, 50, 255)); // 0.10f, 0.09f, 0.12f, 1.00f
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(RGBA(75, 75, 75, 255)); // 0.24f, 0.23f, 0.29f, 1.00f
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Separator] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);

	style->WindowBorderSize = 0.0f;
}

GLFWwindow* InitAPI()
{
	GLFWwindow* mainWindow;

	if (!glfwInit()) { return nullptr; }

	/* Set Window Hints */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glEnable(GL_MULTISAMPLE);

	mainWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Sorting Visualization", nullptr, nullptr);
	glfwMakeContextCurrent(mainWindow);

	if (glewInit()) { return nullptr; }

	return mainWindow;
}

void DeInit()
{
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}