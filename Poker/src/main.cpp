#define _CRT_SECURE_NO_WARNINGS
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <format>
#define STB_IMAGE_IMPLEMENTATION
#include "Poker/Networking.h"
#include "Poker/UI.h"
#include "Poker/Poker.h"
#include "Poker/Server.h"
#include "Poker/Client.h"

int main()
{
    char option;
    do {
        std::cout << "1. Serwer\n";
        std::cout << "2. Client\n";
        option = std::cin.get();
        std::cin.ignore(256, '\n');
        system("cls");
    } while (option != '1' && option != '2');

    if (option == '1')
    {
        std::cout << "serwer\n";
        Server s(1337);
        s.Start();
        ServerMain();
    }
    else
    {
        std::cout << "client\n";
        try
        {
            Client c("192.168.1.21", 1337);
            c.Run();
        }
        catch (std::exception& e)
        {
            std::cerr << "Client error: " << e.what() << "\n";
        }
        ClientMain();
    }
}