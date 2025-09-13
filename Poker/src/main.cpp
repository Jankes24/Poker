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

    bool style_editor = false;
	int bet = 0;
	int blankCard = 0;
    char nick[32]{};
    char p_add[32]{};
    char p_remove[32]{};
    char p_deal[32]{};

    Poker game;
    Player Me;
	game.DealHandToPlayer(Me);
    game.AddPlayer("Admin");
    game.AddPlayer("Alan");
    game.AddPlayer("Jacek");
    game.AddPlayer("Cheater");
    game.NewGame();

    Image cards_textures{ "H:/Programowanie/C++/OpenGL/Poker/assets/CardAtlas.png" };
    GLFWimage icon[3];
    icon[0].pixels = stbi_load("H:/Programowanie/C++/OpenGL/Poker/assets/logo.png", &icon[0].width, &icon[0].height, 0, 4);
    icon[1].pixels = stbi_load("H:/Programowanie/C++/OpenGL/Poker/assets/logo.png", &icon[1].width, &icon[1].height, 0, 4);
    icon[2].pixels = stbi_load("H:/Programowanie/C++/OpenGL/Poker/assets/logo.png", &icon[2].width, &icon[2].height, 0, 4);
    glfwSetWindowIcon(window, 3, icon);
    stbi_image_free(icon[0].pixels);
    stbi_image_free(icon[1].pixels);
    stbi_image_free(icon[2].pixels);

    CardCrop Table_1{ game, 0 };
    CardCrop Table_2{ game, 1 };
    CardCrop Table_3{ game, 2 };
    CardCrop Table_4{ game, 3 };
    CardCrop Table_5{ game, 4 };
    CardCrop Hand_1 { Me.GetHand()[0] };
    CardCrop Hand_2{ Me.GetHand()[1] };

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) { ImGui_ImplGlfw_Sleep(10); continue; }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus;
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("Game DockSpace:", nullptr, window_flags);
            ImGui::PopStyleVar(1);

            ImGui::DockSpace(ImGui::GetID("DockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

            {
                ImGui::Begin("Twoje Karty:");
				Hand_1.Print("HandCard1", cards_textures, Me.GetHand()[0]); ImGui::SameLine();
                Hand_2.Print("HandCard2", cards_textures, Me.GetHand()[1]); ImGui::SameLine();
                ImGui::Text("Your Bet: %d", bet); ImGui::SameLine();
                ImGui::Text("Your Hand: %s", game.EvaluateBestHand(Me.GetHand()).ToString().c_str());
                ImGui::Button("Bet");  ImGui::SameLine(); ImGui::SliderInt("Amount", &bet, 0, 1000);
                ImGui::Button("Fold");
                ImGui::Button("Pass");
                ImGui::Button("Call");
				ImGui::RadioButton("Blank Card 1", &blankCard, 4); ImGui::SameLine();
				ImGui::RadioButton("Blank Card 2", &blankCard, 3); ImGui::SameLine();
				ImGui::RadioButton("Blank Card 3", &blankCard, 2); ImGui::SameLine();
				ImGui::RadioButton("Blank Card 4", &blankCard, 1);
				ImGui::Checkbox("Style Editor", &style_editor);
				if (style_editor)
                {
                    ImGui::Begin("Style");
                    ImGui::ShowStyleEditor();
                    ImGui::End();
                }
                ImGui::End();
            }
            {
                ImGui::Begin("Sesja:");
                if (ImGui::InputText("nick", nick, 32, ImGuiInputTextFlags_EnterReturnsTrue)) Me.GetNick() = nick;
                ImGui::End();
            }
            {
                ImGui::Begin("Cheat:");
                int i = 0;
                for (auto& player : game.GetResultList())
                {
                    i++;
					ImGui::SeparatorText(std::format("{}. {}'s Hand: ", i, player.first.GetNick()).c_str());
                    CardCrop::s_Print((player.first.GetNick() + "1").c_str(), cards_textures, player.first.GetHand()[0], 4); ImGui::SameLine();
                    CardCrop::s_Print((player.first.GetNick() + "2").c_str(), cards_textures, player.first.GetHand()[1], 4);
                    ImGui::Text("%s: %s", player.first.GetNick().c_str(), player.second.ToString().c_str());
                }
                ImGui::End();
            }
            {
                ImGui::Begin("Stol:");
                switch (game.GetPhase())
                {
                case Phase::PreFlop:
					CardCrop::s_Print("BlankTable_1", cards_textures, Card{ Figure(blankCard), Rank::Blank}); ImGui::SameLine();
					CardCrop::s_Print("BlankTable_2", cards_textures, Card{ Figure(blankCard), Rank::Blank }); ImGui::SameLine();
					CardCrop::s_Print("BlankTable_3", cards_textures, Card{ Figure(blankCard), Rank::Blank }); ImGui::SameLine();
					CardCrop::s_Print("BlankTable_4", cards_textures, Card{ Figure(blankCard), Rank::Blank }); ImGui::SameLine();
					CardCrop::s_Print("BlankTable_5", cards_textures, Card{ Figure(blankCard), Rank::Blank });
                    break;
                case Phase::Flop:
                    Table_1.Print("TableCard1", cards_textures, game.GetTableCard(0)); ImGui::SameLine();
                    Table_2.Print("TableCard2", cards_textures, game.GetTableCard(1)); ImGui::SameLine();
                    Table_3.Print("TableCard3", cards_textures, game.GetTableCard(2)); ImGui::SameLine();
                    CardCrop::s_Print("BlankTable_4", cards_textures, Card{ Figure(blankCard), Rank::Blank }); ImGui::SameLine();
                    CardCrop::s_Print("BlankTable_5", cards_textures, Card{ Figure(blankCard), Rank::Blank });
                    break;
                case Phase::Turn:
                    Table_1.Print("TableCard1", cards_textures, game.GetTableCard(0)); ImGui::SameLine();
                    Table_2.Print("TableCard2", cards_textures, game.GetTableCard(1)); ImGui::SameLine();
                    Table_3.Print("TableCard3", cards_textures, game.GetTableCard(2)); ImGui::SameLine();
                    Table_4.Print("TableCard4", cards_textures, game.GetTableCard(3)); ImGui::SameLine();
                    CardCrop::s_Print("BlankTable_4", cards_textures, Card{ Figure(blankCard), Rank::Blank });
                    break;
                case Phase::River:
                    Table_1.Print("TableCard1", cards_textures, game.GetTableCard(0)); ImGui::SameLine();
                    Table_2.Print("TableCard2", cards_textures, game.GetTableCard(1)); ImGui::SameLine();
                    Table_3.Print("TableCard3", cards_textures, game.GetTableCard(2)); ImGui::SameLine();
                    Table_4.Print("TableCard4", cards_textures, game.GetTableCard(3)); ImGui::SameLine();
                    Table_5.Print("TableCard5", cards_textures, game.GetTableCard(4));
                    break;
                }
                ImGui::End();
            }
            {
                ImGui::Begin("Admin Panel:");
                
                
                if (ImGui::InputText("Add player *Enter*", p_add, 32, ImGuiInputTextFlags_EnterReturnsTrue)) game.AddPlayer(p_add);
                if (ImGui::BeginCombo("Player to Remove", p_remove, ImGuiComboFlags_WidthFitPreview))
                {
					if (ImGui::Selectable(" ")) p_remove[0] = '\0';
                    for (const auto& player : game.GetPlayers()) { if (ImGui::Selectable(player.GetNick().c_str())) strncpy_s(p_remove, sizeof(p_remove), player.GetNick().c_str(), 32); }
                    ImGui::EndCombo();
				} ImGui::SameLine();
                if (ImGui::Button("Remove")) game.RemovePlayer(p_remove);
                if (ImGui::BeginCombo("Player to Deal", p_deal, ImGuiComboFlags_WidthFitPreview))
                {
                    if (ImGui::Selectable(" ")) p_deal[0] = '\0';
                    for (const auto& player : game.GetPlayers()) { if (ImGui::Selectable(player.GetNick().c_str())) strncpy_s(p_deal, sizeof(p_deal), player.GetNick().c_str(), 32); }
                    ImGui::EndCombo();
                } ImGui::SameLine();
                if (ImGui::Button("Deal")) game.DealHandToPlayer(p_deal);
                
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				ImGui::RadioButton("Phase PreFlop: ", (int*)&game.GetPhase(), 0);
				ImGui::RadioButton("Phase Flop: ", (int*)&game.GetPhase(), 1);
				ImGui::RadioButton("Phase Turn: ", (int*)&game.GetPhase(), 2);
				ImGui::RadioButton("Phase River: ", (int*)&game.GetPhase(), 3);

				ImGui::BeginTabBar("Tab");
                if (ImGui::BeginTabItem("Game:"))
                {
                    if (ImGui::Button("New Game")) game.NewGame();
                    if (ImGui::Button("Deal Hands To Everyone")) game.DealHands();
                    if (ImGui::Button("Repool")) game.Repool();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Debug Prints:"))
                {
                    if (ImGui::Button("Evaluate Hands")) game.EvaluateHands();
                    if (ImGui::Button("Judge")) std::cout << game.WhoWon();
                    if (ImGui::Button("Print Players")) game.PrintPlayers();
                    if (ImGui::Button("Print Table")) game.PrintTable();
                    if (ImGui::Button("Print Cards")) game.PrintCardPool();
                    if (ImGui::Button("Clear")) system("cls");
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Game Actions:"))
                {
                    if (ImGui::Button("Refill Table")) game.RefillTable();
                    if (ImGui::Button("Evaluate Hands")) game.EvaluateHands();
                    if (ImGui::Button("Simulate New Game")) { game.NewGame(); system("cls"); game.PrintTable(); game.PrintPlayers(); game.EvaluateHands(); std::cout << game.WhoWon(); }
                    if (ImGui::Button("Clear")) system("cls");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
                ImGui::End();
            }
            ImGui::End();
        } 
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
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
