// Dear ImGui: standalone example application for SDL2 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "../include/imgui.h"
#include "../include/imgui_impl_sdl.h"
#include "../include/imgui_impl_opengl3.h"
#include "../include/ContactManager.h"
#include "../include/Contact.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

// Main code
int main(int, char**)
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("My ImGui Application", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    static ContactManager* contacts = new ContactManager();
    static Contact* foundContact = new Contact();

    // Initialize contacts
    if(contacts->m_contacts.empty())
    {
      contacts->init("contacts.txt");
    }

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        static char first[128] = "";
        static char last[128] = "";
        static char num[128] = "";
        static char email[128] = "";

        static char find[128] = "";
        static bool saved = false; 


        {
          ImGui::Begin("Add / Search");
          ImGui::Text("Add a new contact here...");
          ImGui::InputText("First Name", first, IM_ARRAYSIZE(first));
          ImGui::InputText("Last Name", last, IM_ARRAYSIZE(last));
          ImGui::InputText("Phone Number", num, IM_ARRAYSIZE(num));
          ImGui::InputText("E-mail", email, IM_ARRAYSIZE(email));
          if (ImGui::Button("Submit"))
          {
           
            Contact* contact = new Contact(first, last, num, email);
            contacts->addContact(contact);

            strncpy(first, "", 128);
            strncpy(last, "", 128);
            strncpy(num, "", 128);
            strncpy(email, "", 128);
          }

          ImGui::Text("Find Contact");
          ImGui::InputText("Search first name", find, IM_ARRAYSIZE(find));
          if (ImGui::Button("Find"))
          {
            foundContact = contacts->findContact(find);

            strncpy(find, "", 128);
          }

          ImGui::Text("Save Contacts");
          if (ImGui::Button("Save"))
          {

            if(contacts->save("contacts.txt"))
            {
              saved = true;
            }
            else 
            {
              saved = false;
            }
          }

          if(saved)
          {
            ImGui::SameLine();
            ImGui::Text("Saved!");
          }

          ImGui::End();
        }
        // Contacts Window
        {
          ImGui::Begin("Contacts"); // Begin Window
          if (ImGui::BeginTable("table1", 5)) // Begin Table
          {
            if(!contacts->m_contacts.empty())
            {
              for(int i = 0; i < contacts->m_contacts.size(); i++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 5; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        if(column == 0)
                        {
                          ImGui::Text("%s", &contacts->m_contacts[i]->m_first_name);
                        }
                        if(column == 1)
                        {
                          ImGui::Text("%s", &contacts->m_contacts[i]->m_last_name);
                        }
                        if(column == 2)
                        {
                          ImGui::Text("%s", &contacts->m_contacts[i]->m_phone);
                        }
                        if(column == 3)
                        {
                          ImGui::Text("%s", &contacts->m_contacts[i]->m_email);
                        }
                        if(column == 4)
                        {
                          ImGui::PushID(i);
                          ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
                          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
                          ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
                          if(ImGui::Button("Delete"))
                          {
                            std::cout << "clicked" << std::endl;
                            contacts->removeContact(contacts->m_contacts[i]);
                          }
                          ImGui::PopStyleColor(3);
                          ImGui::PopID();
                        }
                    }
                }
            }
              ImGui::EndTable();
          } // End Table


          ImGui::End(); // End Window
        }
        // Search Window
        static char searchFirst[128] = "";
        if(strlen(searchFirst)  == 0 && foundContact->m_first_name != "")
        {
          strncpy(searchFirst, foundContact->m_first_name.c_str(), 128);
        }
        static char searchLast[128] = "";
        if(strlen(searchLast) == 0 && foundContact->m_last_name != "")
        {
          strncpy(searchLast, foundContact->m_last_name.c_str(), 128);
        }
        static char searchPhone[128] = "";
        if(strlen(searchPhone) == 0 && foundContact->m_phone != "")
        {
          strncpy(searchPhone, foundContact->m_phone.c_str(), 128);
        }
        static char searchEmail[128] = "";
        if(strlen(searchEmail) == 0 && foundContact->m_email != "")
        {
          strncpy(searchEmail, foundContact->m_email.c_str(), 128);
        }

        {
            ImGui::Begin("Search");
            if (ImGui::BeginTable("table1", 5)) // Begin Table
            {
              ImGui::TableNextRow();
                for (int column = 0; column < 5; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    if(column == 0)
                    {
                      ImGui::PushID(column);
                      ImGui::InputText("##", searchFirst, IM_ARRAYSIZE(searchFirst));
                      ImGui::PopID();
                    }
                    if(column == 1)
                    {
                      ImGui::PushID(column);
                      ImGui::InputText("##", searchLast, IM_ARRAYSIZE(searchFirst));
                      ImGui::PopID();
                    }
                    if(column == 2)
                    {
                      ImGui::PushID(column);
                      ImGui::InputText("##", searchPhone, IM_ARRAYSIZE(searchFirst));
                      ImGui::PopID();
                    }
                    if(column == 3)
                    {
                      ImGui::PushID(column);
                      ImGui::InputText("##", searchEmail, IM_ARRAYSIZE(searchFirst));
                      ImGui::PopID();
                    }
                    if(column == 4)
                    {
                      ImGui::PushID(column);
                      ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1/7.0f, 0.6f, 0.6f));
                      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1/7.0f, 0.7f, 0.7f));
                      ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1/7.0f, 0.8f, 0.8f));
                      if(ImGui::Button("Update"))
                      {
                        std::cout << "clicked" << std::endl;
                        foundContact->updateContact(searchFirst, searchLast, searchPhone, searchEmail);
                      }
                      ImGui::PopStyleColor(3);
                      ImGui::PopID();
                    }
                }
                ImGui::EndTable();
            }
            ImGui::End();
        }


        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    } // window loop

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
};
