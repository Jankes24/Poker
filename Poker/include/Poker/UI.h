#pragma once
#include <iostream>

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << '\n';
}

GLFWwindow* UIInit()
{
    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();

    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(1500, 650, "POKER", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 0.0f;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 0.93f, 0.67f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.28f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.58f, 0.60f, 0.17f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(1.00f, 0.66f, 0.00f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.88f, 0.06f, 0.06f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 0.66f, 0.00f, 0.43f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.46f, 0.29f, 0.00f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.36f, 0.25f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.80f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.46f, 0.93f, 0.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.00f, 0.78f, 0.45f, 0.98f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.53f, 0.37f, 0.00f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50f, 0.33f, 0.00f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.04f, 1.00f, 0.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.63f, 0.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.83f, 0.44f, 0.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.56f, 0.37f, 0.00f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.86f, 0.65f, 0.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.98f, 0.48f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 1.00f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 1.00f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.00f, 1.00f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.80f, 0.53f, 0.00f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 0.53f, 0.00f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.83f, 0.47f, 0.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.79f, 0.39f, 0.00f, 0.99f);
    colors[ImGuiCol_TabHovered] = ImVec4(1.00f, 0.40f, 0.00f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.49f, 0.28f, 0.00f, 1.00f);
    colors[ImGuiCol_TabSelected] = ImVec4(0.75f, 0.45f, 0.00f, 1.00f);
    colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TabDimmed] = ImVec4(0.51f, 0.26f, 0.00f, 1.00f);
    colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.73f, 0.44f, 0.00f, 1.00f);

    return window;
}