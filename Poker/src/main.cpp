#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>

#include "Poker/Networking.h"
#include "Poker/UI.h"
#include "Poker/Poker.h"

int main(int, char**)
{
    std::srand(time(0));

    /*
    asio::io_context io_context;
    Server server(io_context, "127.0.0.1", "1337");
    server.start();
    std::jthread t([&io_context]() { io_context.run(); });
    */

    GLFWwindow* window = UIInit();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImVec4 clear_color = ImVec4(0.1f, 0.2f, 0.3f, 1.0f);
    int bet = 0;
    char p_add[32]{};
    char p_remove[32]{};
    char p_deal[32]{};
    char nick[32]{};

    Poker game;
    game.AddPlayer("Admin");
    game.AddPlayer("Alan");
    game.AddPlayer("Jacek");
    game.AddPlayer("Cheater");
    game.NewGame();
    game.DealHandToPlayer("Cheater", { Figure::Hearts, Rank::Ace, Figure::Hearts, Rank::King });
    game.GetTableCard(0) = {Figure::Hearts, Rank::Queen};
    game.GetTableCard(1) = {Figure::Hearts, Rank::Jack};
    game.GetTableCard(2) = {Figure::Hearts, Rank::Ten};
    game.GetTableCard(3) = {Figure::Hearts, Rank::Nine};

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui
        {
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("Game DockSpace:", nullptr, window_flags);
            ImGui::PopStyleVar(1);

            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            {
                ImGui::Begin("Twoje Karty:");
                ImGui::InputInt("Bet Amount", &bet);
                ImGui::Button("Bet");
                ImGui::Button("Fold");
                ImGui::Button("Pass");
                ImGui::Button("Call");
                ImGui::End();
            }
            {
                ImGui::Begin("Sesja:");
                ImGui::InputText("nick", nick, 32);
                ImGui::Button("apply");
                ImGui::End();
            }
            {
                ImGui::Begin("Stol:");
                ImGui::End();
            }
            {
                ImGui::Begin("Admin Panel:");
                ImGui::InputText("Player: ", p_add, 32); if (ImGui::Button("Add Player")) game.AddPlayer(p_add);
                ImGui::InputText("Player to remove: ", p_remove, 32); if (ImGui::Button("Remove Player")) game.RemovePlayer(p_remove);
                ImGui::InputText("Player to deal: ", p_deal, 32); if (ImGui::Button("Deal Cards To:")) game.DealHandToPlayer(p_deal);
                if (ImGui::Button("New Game")) game.NewGame();
                if (ImGui::Button("Judge")) std::cout << game.WhoWon();
                if (ImGui::Button("Deal Cards To Everyone")) game.DealHands();
                if (ImGui::Button("Repool Deck")) game.Repool();
                if (ImGui::Button("Print Players")) game.PrintPlayers();
                if (ImGui::Button("Print Cards")) game.PrintCardPool();
                if (ImGui::Button("Print Table")) game.PrintTable();
                if (ImGui::Button("Refill Table")) game.RefillTable();
                if (ImGui::Button("Deal Hand")) game.DealHand();
                if (ImGui::Button("Pop Random Card")) game.PopRandomCard();
                if (ImGui::Button("Evaluate Hands")) game.EvaluateHands();
                if (ImGui::Button("Evaluate And Print New Hands")) { game.NewGame(); system("cls"); game.PrintTable(); game.PrintPlayers(); game.EvaluateHands(); std::cout << game.WhoWon(); }
                if (ImGui::Button("Clear")) system("cls");
                ImGui::End();
            }
            ImGui::End();
        } 

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
