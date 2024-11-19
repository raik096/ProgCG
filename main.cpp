// CaroselloCG.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "vendor/imgui/backends/imgui_impl_glfw.h"
#include "vendor/imgui/backends/imgui_impl_opengl3.h"
#include "vendor/imgui/imgui.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "includes/ModelLoader.h"
#include "includes/Input.h"

#include "includes/Scene.h"
#include "includes/materials/LitMaterial.h"
#include "includes/Texture.h"
#include "includes/Carosello.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGTH 720

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void printout_opengl_glsl_info() {
	const GLubyte* renderer		= glGetString(GL_RENDERER);
	const GLubyte* vendor		= glGetString(GL_VENDOR);
	const GLubyte* version		= glGetString(GL_VERSION);
	const GLubyte* glslVersion	= glGetString(GL_SHADING_LANGUAGE_VERSION);

	std::cout << "GL Vendor            :" << vendor << std::endl;
	std::cout << "GL Renderer          :" << renderer << std::endl;
	std::cout << "GL Version (string)  :" << version << std::endl;
	std::cout << "GLSL Version         :" << glslVersion << std::endl;
}

int main()
{


    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGTH, "Carosello", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glewInit();

	printout_opengl_glsl_info();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Input::setupMouseInputs(window);

    /* initialize IMGUI */
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = io.DisplaySize.y / io.DisplaySize.x;
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGTH);

    //Setup della scena
    Scene mainScene;

    Shader *basicShader = new Shader("shaders/basic");
    Texture shapesTexture("./resources/Rock.png");
    Texture shapesNormal("./resources/RockNormal.png");
    LitMaterial *basicMat = new LitMaterial(basicShader, shapesTexture, shapesNormal);
    Renderable testPlane = ModelLoader::Load("./models/testPlane.glb", basicMat);
    Mesh cubeMesh = ModelLoader::LoadMesh("./models/cube.glb")[0];

    Renderable car  = ModelLoader::Load("models/car.glb", basicMat);
    car.SetPosition(glm::vec3(50, 1, 50));
    car.Scale(0.2f);

    //Terrain terrain;
    Carosello carosello("./resources/small_test.svg");

    Renderable cube1(cubeMesh, basicMat);
    Renderable cube2(cubeMesh, basicMat);
    Renderable cube3(cubeMesh, basicMat);
    Renderable cube4(cubeMesh, basicMat);
    cube1.SetPosition(glm::vec3(5, 6, 5));
    cube2.SetPosition(glm::vec3(-5, 4, -5));
    cube3.SetPosition(glm::vec3(8, -2, -5));
    cube4.SetPosition(glm::vec3(-8, -2, 8));

    //mainScene.Add(terrain.RendereableObj());
    mainScene.Add(carosello.RendereableObj());
    mainScene.Add(cube1);
    mainScene.Add(cube2);
    mainScene.Add(cube3);
    mainScene.Add(cube4);
    mainScene.Add(car);

    mainScene.Start();
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Aggiorna la scena
        mainScene.Update();

        //Disegna la scena
        mainScene.Draw();

        Input::Reset();

        /* draw the Graphical User Interface */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::BeginMainMenuBar();
        if (ImGui::Button(mainScene.isControllingLigth?"Controlla Camera":"Controlla Luce"))
        {
            mainScene.isControllingLigth = !mainScene.isControllingLigth;
        }
        ImGui::EndMainMenuBar();

        ImGui::Begin("Test");
        ImGui::ColorEdit3("Cube Color", (float*)basicMat->GetColor());

        ImGui::SeparatorText("Shadow Debugging");
        ImGui::Checkbox("Enable shadows", &mainScene.m_SceneData.enableShadows);
        ImGui::Text("Texture ID: %x", mainScene.m_SceneData.ligth->fbo.DepthTexture());
        ImGui::Image((void*)(intptr_t)mainScene.m_SceneData.ligth->fbo.DepthTexture().GetID(), ImVec2(256, 256));
        ImGui::DragFloat3("Ligth Position", &mainScene.m_SceneData.ligth->position[0]);
        ImGui::SliderFloat("Ligth Distance", &mainScene.m_SceneData.ligth->maxDistance, 1.0f, 200.0f);
        ImGui::SliderFloat("Ligth Bias", &mainScene.m_SceneData.ligth->ligthBias, 0.0f, 1.0f);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}