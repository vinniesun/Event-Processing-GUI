//#pragma once
#ifndef GUIPROCESS_H
#define GUIPROCESS_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <dirent.h>             // This header is used to process a directory

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"       // This header file is needed for using std::string with ImGui::InputText() instead of char*

#include "process.hpp"

static double multiplier = 1e6;

class guiProcess{
public:
    guiProcess();
    ~guiProcess();
    Events preprocess_file();
    void preprocess_initialisation();
    void preprocess_run();
    void reconstructed_initialisation();
    int run_display_reconstructed();
    void live_process_initialisation();
    void live_process_run();
    void resetFunction();
    void updateImage();

    bool gui_status;                            // Display the current status of the GUI
    bool show_setting;                          // Display the processing setting window

    int frame;                                  // Frame determines which frame of the images should be displayed
    int frame_debug;
    int time_window;                            // The time window for how long (in us) events should be accumulated for to generate a frame
    int height;                                 // The height of the Event Camera
    int width;                                  // The width of the Event Camera
    int ref_period;                             // The refractory period for Refractory Filtering
    int nn_window;                              // The time window for Nearest Neighbourhood Filtering
    int no_of_events;                           // The value determines number of events that will be processed
    int event_no;                               // The current event number to process
    int ktos;                                   // The ktos parameter for updating TOS
    int ttos;                                   // The ttos parameter for updating TOS
    int prev_time;                              // This variable keeps track of the timestamp of the previous event
    int quant;                                  // This is the quant variable for updating SAE

    bool on_off_check;                          // This flag checks if On and Off events should be stored separately or together in a Time Surface
    bool tos_check;                             // This flag checks if the events are to be stored in a TOS (when true) or a Time Surface (when false, the default)
    bool start_processing;                      // This flag indicates whether the event text file will be processed or not.
    bool display_reconstructed;                 // This flag indicates whether the reconstructed images should be displayed
    bool display_live;                          // This flag indicates whether the events will be processed in a live manner.
    bool display_processed;                     // This flag indicates whether the event is displayed post-processed.
    bool preprocessing;                         // This flag indicates if we need to preprocess the event txt file.
    bool reset;                                 // This flag indicates if the class variables are to be reset.
    bool davis240;                              // This flag indicates that the input event file is generated with a DAVIS240
    bool davis346;                              // This flag indicates that the input event file is generated iwth a DAVIS346
    bool first_recon;                           // This flag checks if this is the first reconstructed frame to be displayed. If it is, then the OpenGL texture object will be updated
    bool first_live;                            // This flag is currently unused, might delete later

    std::string filename;                       // The path to the event text file
    std::string recon_path;                     // The reconstructed images folder's path

    std::vector<std::string> recon_files;       // This vector constains the name of all the files provided in recon_path

    Events process_events;                      // The Events object used in post-processed display that holds all of the valid events
    cv::Mat surface_area;                       // This Mat object is the Time Surface that's updated per event. Can be either TOS or traditional Time Surface
    std::string mode;                           // This string indicates what kind of Time Surface Update scheme that's used.

    std::fstream event_file;                    // The fstream object for reading the files.
    int line_no;                                // This keeps track of the number of lines read by the fstream object

    int x_prev;                                 // Used by NN filtering
    int y_prev;                                 // Used by NN filtering
    int p_prev;                                 // Used by NN filtering
    std::vector<std::vector<int>> ref_mask;     // This is the 2D vector used for Refractory Filtering
    std::vector<std::vector<int>> nn_mask;      // This is the 2D vector used for NN Filtering

    GLuint texture;                             // This is the texture ID used for live processing and post-processing
    GLuint texture_recon;                       // This is the texture ID used for displaying reconstructed images.
    cv::Mat image;                              // This is the Mat object that holds the current image to be translated to OpenGL texture for display (live processing & post-processing)
    cv::Mat image_recon;                        // This is the Mat object that holds the current reconstructed frame to be translated to OpenGL texture for display
};

bool sortCriteria(std::string s1, std::string s2);  // Helper function for sorting filenames

#endif