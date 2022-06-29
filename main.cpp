// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"       // This header file is needed for using std::string with ImGui::InputText() instead of char*
#include <stdio.h>
// #include <string>
// #include <iostream>
// #include <opencv2/opencv.hpp>

// #include <chrono>
#include <thread>
#include "process.hpp"
#include "efast.hpp"
#include "arcstar.hpp"
#include "luvharris.hpp"
#include "customharris.hpp"
#include "progressbar.hpp"
#include "bloomfilter.hpp"

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// std::string PATH = "/Users/vincent/Desktop/CityUHK/Event_Process/Hashing/Code/Python/Shapes_Rotation_Ground_Truth/";

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// Helper to display a little (?) mark which shows a tooltip when hovered.
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

Events preprocess_file(const std::string &filename_, const int &height_, const int &width_, const int &ref_period_, const int &nn_window_) {
    if (!ref_period_ && !nn_window_) {
        Events process_ = read_file(filename_, height_, width_);
        return process_;
    } else {
        Events input_event = read_file(filename_, height_, width_);
        Events process_;
    
        if (ref_period_ && nn_window_) {
            Events ref_event;
            refractoryFiltering(input_event, ref_event, ref_period_);
            ref_event.event_count = ref_event.x.size();

            nnFiltering(ref_event, process_, nn_window_);
            process_.event_count = process_.x.size();
            ref_event.~Events();
        } else if (ref_period_) {
            refractoryFiltering(input_event, process_, ref_period_);
            process_.event_count = process_.x.size();
        } else if (nn_window_) {
            refractoryFiltering(input_event, process_, nn_window_);
            process_.event_count = process_.x.size();
        }

        return process_;
    }
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenCV Test", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    static bool gui_status = true;                  // Display the current status of the GUI
    static bool show_setting = true;                // Display the processing setting window

    static int frame = 1;                           // Frame determines which frame of the images should be displayed
    static int time_window = 0;                     // The time window for how long (in us) events should be accumulated for to generate a frame
    static int height = 0;                          // The height of the Event Camera
    static int width = 0;                           // The width of the Event Camera
    static int ref_period = 0;                      // The refractory period for Refractory Filtering
    static int nn_window = 0;                       // The time window for Nearest Neighbourhood Filtering
    static int no_of_events = -1;                   // The value determines number of events that will be processed
    volatile int event_no = 0;                        // The current event number to process
    static int ktos = 0;
    static int ttos = 0;
    static int prev_time = 0;
    static int quant = 0;

    static bool on_off_check = false;               // This flag checks if On and Off events should be stored separately or together in a Time Surface
    static bool tos_check = false;                  // This flag checks if the events are to be stored in a TOS (when true) or a Time Surface (when false, the default)
    static bool start_processing = false;           // This flag indicates whether the event text file will be processed or not.
    static bool display_reconstructed = false;      // This flag indicates whether the reconstructed images should be displayed
    static bool display_live = false;               // This flag indicates whether the events will be processed in a live manner or post-processed.
    static bool display_processed = false;
    static bool preprocessing = false;               // This flag indicates if we need to preprocess the event txt file.

    static std::string filename = "";        // The path to the event text file
    static std::string recon_path = "";      // The reconstructed images folder's path

    static Events process_events;
    static cv::Mat surface_area;
    static std::string mode = "";

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear(GL_COLOR_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Window for showing the settings of the GUI Application
        if (show_setting) {
            ImGui::SetNextWindowSize(ImVec2(0,0));      // Set the next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Once);   // This set where the next window is located

            ImGui::Begin("Settings", &show_setting);
            //ImGui::SetWindowSize(ImVec2(0,0));      // This need to be called within a Begin()/End() pair. Set to ImVec2(0,0) for auto-fit. Not preferred.

            ImGui::Text("Display Options");
            ImGui::Checkbox("Display Reconstructed Images", &display_reconstructed);
            ImGui::SameLine();
            HelpMarker("This checks whether On & Off event should be saved as together or separately.\n"
                       "If the Checkbox is checked, the reconstructed images will be displayed.\n"
                       "If the Checkbox is unchecked, the reconstructed images will not be displayed (The Default Mode).");

            ImGui::Checkbox("Live Processing", &display_live);
            ImGui::SameLine();
            HelpMarker("This checks whether the event txt file will be processed in live fashion, i.e. the text file will be streamed in live.\n"
                       "If the Checkbox is checked, the events will be processed live.\n");

            ImGui::Checkbox("Display Pre-Processing", &display_processed);
            ImGui::SameLine();
            HelpMarker("This checks whether the event txt file will be pre-processed before displaying.\n"
                       "If the Checkbox is checked, the events will be pre-processed.\n");
            ImGui::SameLine();
            ImGui::Checkbox("Pre-Processing", &preprocessing);
            ImGui::SameLine();
            HelpMarker("This checks whether the event txt file will be pre-processed before displaying.\n"
                       "If the Checkbox is checked, the events will be pre-processed.\n");

            ImGui::Text("Processing Options");
            ImGui::Checkbox("Separate On & Off Events", &on_off_check);
            ImGui::SameLine();
            HelpMarker("This checks whether On & Off event should be saved as together or separately.\n"
                       "If the Checkbox is checked, the events will be saved in two separate Time Surface, one for On events, one for Off events.\n"
                       "If the Checkbox is unchecked, the events will be saved in a single Time Surface (The Default Mode).");
            
            ImGui::Checkbox("TOS", &tos_check);
            ImGui::SameLine();
            HelpMarker("This checks whether the Time Surface or TOS is to be used.\n"
                       "If the Checkbox is checked, the TOS will be used, otherwise Time Surface is used (The Default Mode).");

            ImGui::Text("Processing Parameters");
            ImGui::LabelText("Parameter", "Value");

            ImGui::InputText("Reconstructed Folder", &recon_path);
            ImGui::SameLine();
            HelpMarker("Enter the Absolute Path to the Reconstructed Images' folder.\n"
                       "For Example: /Users/vincent/Desktop/CityUHK/Event_Process/Hashing/Code/Python/Shapes_Rotation_Ground_Truth/");

            ImGui::InputText("Event File", &filename);
            ImGui::SameLine();
            HelpMarker("Enter the Absolute Path to the event txt file.\n"
                       "For Example: /Users/vincent/Desktop/CityUHK/Event_Process/Event_Camera_Dataset/shapes_rotation/events.txt");

            ImGui::InputInt("Time Window (us)", &time_window);
            ImGui::SameLine();
            HelpMarker("Enter the Time Window period (in us) for how long events should be accumulated for to generate a frame");

            ImGui::InputInt("Number of Events to Process", &no_of_events);
            ImGui::SameLine();
            HelpMarker("Enter the Number of Events you wish to process.\n"
                       "The default value is -1, which indicates all events will be processed.");

            ImGui::InputInt("Frame Height", &height);
            ImGui::SameLine();
            HelpMarker("Enter the height of the Event Camera");
            ImGui::InputInt("Frame Width", &width);
            ImGui::SameLine();
            HelpMarker("Enter the width of the Event Camera");

            ImGui::InputInt("Refractory Period", &ref_period);
            ImGui::SameLine();
            HelpMarker("Enter the refractory period (in us) for Refractory Filtering.\n"
                       "If the value is left at zero, there will be no Refractory Filtering.");

            ImGui::InputInt("Nearest Neighbourhood Window", &nn_window);
            ImGui::SameLine();
            HelpMarker("Enter the time window (in us) for Nearest Neighbourhood Filtering.\n"
                       "If the value is left at zero, there will be no Nearest Neighbourhood Filtering.");

            ImGui::InputInt("KTOS", &ktos);
            ImGui::SameLine();
            HelpMarker("Enter the ktos value for determining TOS window size\n"
                       "If the value is left at zero, then TOS is not being used."); 

            if (ImGui::Button("Start Process")) start_processing = true;
            if (start_processing) {
                ImGui::SameLine();
                ImGui::Text("Processing Event File!");
            } else {
                ImGui::SameLine();
                ImGui::Text("\t\t\t\t\t");
            }

            if (ImGui::Button("Stop Process")) start_processing = false;
            if (!start_processing) {
                ImGui::SameLine();
                ImGui::Text("Stopped Processing Event File!");
            }

            ImVec2 windowSize = ImGui::GetWindowSize();
            ImGui::Text("Current window size is x: %.1f, y: %.1f", windowSize.x, windowSize.y);

            ImGui::End();
        }

        if (gui_status) {
            ImGui::SetNextWindowSize(ImVec2(0,0));
            ImGui::SetNextWindowPos(ImVec2(0,500), ImGuiCond_Once);

            ImGui::Begin("GUI Status", &gui_status);

            ImGui::Text("Current average framerate is %.1f FPS", ImGui::GetIO().Framerate); // Dear ImGUI calculate the average framerate over the last 120 frames.

            // ImVec2 screen_pos = ImGui::GetCursorScreenPos();
            // ImGui::Text("Current cursor position is %.1f,, %.1f", screen_pos.x, screen_pos.y);

            ImGui::End();
        }

        if (start_processing) {
            // Window for showing the live events
            if (display_live) {
                ImGui::SetNextWindowSize(ImVec2(0,0));
                ImGui::SetNextWindowPos(ImVec2(699,400), ImGuiCond_Once);

                ImGui::Begin("Live Event", &display_live);

                // Process Event

                ImGui::End();
            }

            if (display_processed) {
                // For testing, use:
                // /Users/vincent/Desktop/CityUHK/Event_Process/Event_Camera_Dataset/shapes_rotation/events.txt
                ImGui::SetNextWindowSize(ImVec2(0,0));
                ImGui::SetNextWindowPos(ImVec2(699,400), ImGuiCond_Once);

                ImGui::Begin("Pre-processed Events", &display_live);

                if (filename.empty()) {
                    ImGui::Text("Event txt Filename not provided!");
                } else {
                    if (preprocessing) {
                        ttos = 2*(2*ktos + 1);

                        if (tos_check) {
                            mode = "TOS";
                            surface_area = cv::Mat(height, width, CV_8UC1, cv::Scalar(0));
                            std::cout << "TOS" << std::endl;
                        } else {
                            mode = "timestamp";
                            surface_area = cv::Mat(height, width, CV_32SC1, cv::Scalar(0));
                            std::cout << "timestamp" << std::endl;
                        }
                        process_events = preprocess_file(filename, height, width, ref_period, nn_window);
                        std::cout << process_events.event_count << std::endl;
                        
                        preprocessing = false;
                    } else {
                        if (!ref_period && !nn_window) ImGui::Text("No Filtering!");
                        else if (ref_period) ImGui::Text("Refractory Filtering!");
                        else if (nn_window) ImGui::Text("Nearest Neighbourhood Filtering!");
                        else if (ref_period && nn_window) ImGui::Text("Refractory Filtering and Nearest Neighbourhood Filtering!");
                        ImGui::Text("Event Number: %d", event_no);
                        // Display Process
                        const int pol = process_events.p[event_no] ? 1:0;
                        int32_t x = process_events.x[event_no];
                        int32_t y = process_events.y[event_no];
                        int32_t t = process_events.t[event_no];

                        update_sae(surface_area, mode, x, y, t, time_window, quant, prev_time, ktos, ttos, width, height);

                        cv::Mat image;
                        cv::cvtColor(surface_area, image, cv::COLOR_BGR2RGBA);

                        GLuint texture;
                        glGenTextures(1, &texture);
                        glBindTexture(GL_TEXTURE_2D, texture);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
                        ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture)), ImVec2(image.cols, image.rows));

                        prev_time = t; // Update prev_time
                        event_no++;

                        if (event_no >= process_events.event_count || (event_no >= no_of_events && no_of_events != -1)) {
                            preprocessing = true;
                            display_processed = false;
                        }
                    }
                }
                ImGui::End();
            }
        } else {
            preprocessing = true;
            display_processed = false;
        }

        // Window for showing the reconstructed images
        if (display_reconstructed) {
            // For testing, use:
            // /Users/vincent/Desktop/CityUHK/Event_Process/Hashing/Code/Python/Shapes_Rotation_Ground_Truth/
            ImGui::SetNextWindowSize(ImVec2(0,0), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(699,0), ImGuiCond_Once);
            
            ImGui::Begin("Reconstructed Images", &display_reconstructed);

            if (recon_path.empty()) {
                ImGui::Text("Reconstructed Folder's Path was not provided!!!");
                ImGui::End();
            } else{
                cv::Mat image = cv::imread(recon_path + std::to_string(frame) + ".jpg", cv::IMREAD_COLOR);
                if (image.empty()) return -1;

                cv::cvtColor(image, image, cv::COLOR_BGR2RGBA);

                ImGui::SetNextWindowSize(ImVec2(0,0));
                //ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Once);

                GLuint texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
                ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture)), ImVec2(image.cols, image.rows));
                ImGui::End();

                if (frame%1530 == 0) frame = 1;
                else frame++;

                std::this_thread::sleep_for(std::chrono::milliseconds(33));
            }
        } else frame = 1;

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapInterval(0);    // This disables VSync in GLFW. 0 for off, 1 for on
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}