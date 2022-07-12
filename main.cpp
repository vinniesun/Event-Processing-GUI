#include <thread>
#include "process.hpp"
#include "efast.hpp"
#include "arcstar.hpp"
#include "luvharris.hpp"
#include "customharris.hpp"
#include "progressbar.hpp"
#include "bloomfilter.hpp"
#include "guiprocess.hpp"

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
// #include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
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
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Event Processing", NULL, NULL);
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

    guiProcess mainProcess;

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
        if (mainProcess.show_setting) {
            ImGui::SetNextWindowSize(ImVec2(0,0));      // Set the next window size. Set axis to 0.0f to force an auto-fit on this axis.
            ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Once);   // This set where the next window is located

            ImGui::Begin("Settings", &mainProcess.show_setting);
            //ImGui::SetWindowSize(ImVec2(0,0));      // This need to be called within a Begin()/End() pair. Set to ImVec2(0,0) for auto-fit. Not preferred.

            ImGui::Text("Display Options");
            ImGui::Checkbox("Display Reconstructed Images", &mainProcess.display_reconstructed);
            ImGui::SameLine();
            HelpMarker("This checks whether On & Off event should be saved as together or separately.\n"
                       "If the Checkbox is checked, the reconstructed images will be displayed.\n"
                       "If the Checkbox is unchecked, the reconstructed images will not be displayed (The Default Mode).");

            ImGui::Checkbox("Live Processing", &mainProcess.display_live);
            ImGui::SameLine();
            HelpMarker("This checks whether the event txt file will be processed in live fashion, i.e. the text file will be streamed in live.\n"
                       "If the Checkbox is checked, the events will be processed live.\n");
            if (mainProcess.display_live) mainProcess.display_processed = false;
            ImGui::SameLine();
            ImGui::Checkbox("Display Pre-Processing", &mainProcess.display_processed);
            ImGui::SameLine();
            HelpMarker("This checks whether the event txt file will be pre-processed before displaying.\n"
                       "If the Checkbox is checked, the events will be pre-processed.\n");
            if (mainProcess.display_processed) mainProcess.display_live = false;

            ImGui::Checkbox("Pre-Processing", &mainProcess.preprocessing);
            ImGui::SameLine();
            HelpMarker("This checks whether the event txt file will be pre-processed before displaying.\n"
                       "If the Checkbox is checked, the events will be pre-processed.\n");
            if (mainProcess.display_live) mainProcess.preprocessing = false;

            ImGui::Text("Processing Options");
            ImGui::Checkbox("Separate On & Off Events", &mainProcess.on_off_check);
            ImGui::SameLine();
            HelpMarker("This checks whether On & Off event should be saved as together or separately.\n"
                       "If the Checkbox is checked, the events will be saved in two separate Time Surface, one for On events, one for Off events.\n"
                       "If the Checkbox is unchecked, the events will be saved in a single Time Surface (The Default Mode).");
            
            ImGui::Checkbox("TOS", &mainProcess.tos_check);
            ImGui::SameLine();
            HelpMarker("This checks whether the Time Surface or TOS is to be used.\n"
                       "If the Checkbox is checked, the TOS will be used, otherwise Time Surface is used (The Default Mode).");

            ImGui::Text("Processing Parameters");
            ImGui::LabelText("Parameter", "Value");

            ImGui::InputText("Reconstructed Folder", &mainProcess.recon_path);
            ImGui::SameLine();
            HelpMarker("Enter the Absolute Path to the Reconstructed Images' folder.\n"
                       "For Example: /Users/vincent/Desktop/CityUHK/Event_Process/Hashing/Code/Python/Shapes_Rotation_Ground_Truth/");

            ImGui::InputText("Event File", &mainProcess.filename);
            ImGui::SameLine();
            HelpMarker("Enter the Absolute Path to the event txt file.\n"
                       "For Example: /Users/vincent/Desktop/CityUHK/Event_Process/Event_Camera_Dataset/shapes_rotation/events.txt");

            ImGui::InputInt("Time Window (us)", &mainProcess.time_window);
            ImGui::SameLine();
            HelpMarker("Enter the Time Window period (in us) for how long events should be accumulated for to generate a frame");

            ImGui::InputInt("Number of Events to Process", &mainProcess.no_of_events);
            ImGui::SameLine();
            HelpMarker("Enter the Number of Events you wish to process.\n"
                       "The default value is -1, which indicates all events will be processed.");

            ImGui::Checkbox("DAVIS240 Camera", &mainProcess.davis240);
            ImGui::SameLine();
            HelpMarker("Check this box if your event file is generated from a DAVIS240 Camera.\n");
            if (mainProcess.davis240) {
                mainProcess.davis346 = false;
                mainProcess.height = 180;
                mainProcess.width = 240;
            }
            ImGui::SameLine();
            ImGui::Checkbox("DAVIS346 Camera", &mainProcess.davis346);
            ImGui::SameLine();
            HelpMarker("Check this box if your event file is generated from a DAVIS346 Camera.\n");
            if (mainProcess.davis346) {
                mainProcess.davis240 = false;
                mainProcess.height = 260;
                mainProcess.width = 346;
            }

            ImGui::InputInt("Frame Height", &mainProcess.height);
            ImGui::SameLine();
            HelpMarker("Enter the height of the Event Camera");
            ImGui::InputInt("Frame Width", &mainProcess.width);
            ImGui::SameLine();
            HelpMarker("Enter the width of the Event Camera");

            ImGui::InputInt("Refractory Period (us)", &mainProcess.ref_period);
            ImGui::SameLine();
            HelpMarker("Enter the refractory period (in us) for Refractory Filtering.\n"
                       "If the value is left at zero, there will be no Refractory Filtering.");

            ImGui::InputInt("Nearest Neighbourhood Window (us)", &mainProcess.nn_window);
            ImGui::SameLine();
            HelpMarker("Enter the time window (in us) for Nearest Neighbourhood Filtering.\n"
                       "If the value is left at zero, there will be no Nearest Neighbourhood Filtering.");

            ImGui::InputInt("KTOS", &mainProcess.ktos);
            ImGui::SameLine();
            HelpMarker("Enter the ktos value for determining TOS window size\n"
                       "If the value is left at zero, then TOS is not being used."); 

            if (ImGui::Button("Start Process")) {
                if (!(mainProcess.davis240 || mainProcess.davis346)) {
                    
                    mainProcess.start_processing = false;

                    ImGui::OpenPopup("Camera Not Chosen");
                } else mainProcess.start_processing = true;
            }

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Camera Not Chosen", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("The Camera Model was not chosen. Please select Camera Model before using processing!\n");
                ImGui::Separator();
                ImGui::SetItemDefaultFocus();
                if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                
                ImGui::EndPopup();
            }

            if (mainProcess.start_processing) {
                ImGui::SameLine();
                ImGui::Text("Processing Event File!");
            } else {
                ImGui::SameLine();
                ImGui::Text("\t\t\t\t\t");
            }

            if (ImGui::Button("Stop Process")) {
                mainProcess.start_processing = false;
                mainProcess.display_reconstructed = false;
                mainProcess.preprocessing = false;
            }
            // if (!start_processing) {
            //     ImGui::SameLine();
            //     ImGui::Text("Stopped Processing Event File!");
            // }

            if (ImGui::Button("Reset Parameters")) mainProcess.reset = true;

            ImVec2 windowSize = ImGui::GetWindowSize();
            ImGui::Text("Current window size is x: %.1f, y: %.1f", windowSize.x, windowSize.y);

            ImGui::End();
        }

        if (mainProcess.gui_status) {
            ImGui::SetNextWindowSize(ImVec2(0,0));
            ImGui::SetNextWindowPos(ImVec2(0,537), ImGuiCond_Once);

            ImGui::Begin("GUI Status", &mainProcess.gui_status);

            ImGui::Text("Current average framerate is %.1f FPS", ImGui::GetIO().Framerate); // Dear ImGUI calculate the average framerate over the last 120 frames.
            
            // GetMousePos() gets the current mouse's coordinate relative to the window's top left corner, which is 0, 0
            ImVec2 screen_pos = ImGui::GetMousePos();
            ImGui::Text("Current mouse position is %0.0f, %0.0f", screen_pos.x, screen_pos.y);

            ImGui::End();
        }

        // For testing, use:
        // /Users/vincent/Desktop/CityUHK/Event_Process/Event_Camera_Dataset/shapes_rotation/events.txt
        if (mainProcess.start_processing) {
            ImGui::SetNextWindowSize(ImVec2(0,0));
            ImGui::SetNextWindowPos(ImVec2(799,400), ImGuiCond_Once);

            // Window for showing the live events
            if (mainProcess.display_live) {
                ImGui::Begin("Live Event", &mainProcess.display_live);

                if (!mainProcess.ref_period && !mainProcess.nn_window) ImGui::Text("No Filtering!");
                else if (mainProcess.ref_period) ImGui::Text("Refractory Filtering!");
                else if (mainProcess.nn_window) ImGui::Text("Nearest Neighbourhood Filtering!");
                else if (mainProcess.ref_period && mainProcess.nn_window) ImGui::Text("Refractory Filtering and Nearest Neighbourhood Filtering!");
                ImGui::Text("Event Number: %d", mainProcess.line_no);

                // Process Event
                if (mainProcess.event_file.is_open()) mainProcess.live_process_run();
                else {
                    mainProcess.live_process_initialisation();

                    mainProcess.live_process_run();
                }

                ImGui::End();
            }

            else if (mainProcess.display_processed) {
                ImGui::Begin("Pre-processed Events", &mainProcess.display_live);

                if (mainProcess.filename.empty()) {
                    ImGui::Text("Event txt Filename not provided!");
                } else {
                    if (mainProcess.preprocessing) {
                        mainProcess.preprocess_initialisation();
                        
                        mainProcess.preprocessing = false;
                    } else {
                        if (!mainProcess.ref_period && !mainProcess.nn_window) ImGui::Text("No Filtering!");
                        else if (mainProcess.ref_period) ImGui::Text("Refractory Filtering!");
                        else if (mainProcess.nn_window) ImGui::Text("Nearest Neighbourhood Filtering!");
                        else if (mainProcess.ref_period && mainProcess.nn_window) ImGui::Text("Refractory Filtering and Nearest Neighbourhood Filtering!");
                        ImGui::Text("Event Number: %d", mainProcess.event_no);

                        if (mainProcess.event_no >= mainProcess.no_of_events && mainProcess.no_of_events != -1) {
                            ImGui::Text("Finished");
                        } else {
                            // Display Process
                            mainProcess.preprocess_run();
                        }
                    }
                }
                ImGui::End();
            }
        }

        // Window for showing the reconstructed images
        if (mainProcess.display_reconstructed) {
            // For testing, use:
            // /Users/vincent/Desktop/CityUHK/Event_Process/Hashing/Code/Python/Shapes_Rotation_Ground_Truth/
            ImGui::SetNextWindowSize(ImVec2(0,0), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(799,0), ImGuiCond_Once);
            
            ImGui::Begin("Reconstructed Images", &mainProcess.display_reconstructed);

            if (mainProcess.recon_path.empty()) {
                ImGui::Text("Reconstructed Folder's Path was not provided!!!");
                ImGui::End();
            } else{
                ImGui::Text("Current Frame is: %d", mainProcess.frame_debug);

                int status = mainProcess.run_display_reconstructed();

                if (!status) std::cout << "Invalid Data Path" << std::endl;

                ImGui::End();

                // This forces the display framerate to around 25~30FPS when displaying reconstructed images
                std::this_thread::sleep_for(std::chrono::milliseconds(33));
            }
        } else mainProcess.frame = 1;

        if (mainProcess.reset) {
            mainProcess.resetFunction();
        }

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
