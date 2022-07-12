#include "guiprocess.hpp"

guiProcess::guiProcess() {
    gui_status = true;
    show_setting = true;

    frame = 1;
    frame_debug = 1;
    time_window = 0;
    height = 0;
    width = 0;
    ref_period = 0;
    nn_window = 0;
    no_of_events = -1;
    event_no = 0;
    ktos = 0;
    ttos = 0;
    prev_time = 0;
    quant = 0;

    on_off_check = false;
    tos_check = false;
    start_processing = false;
    display_reconstructed = false;
    display_live = false;
    display_processed = false;
    preprocessing = false;
    reset = false;
    davis240 = false;
    davis346 = false;
    first_recon = true;
    first_live = true;

    filename = "";
    recon_path = "";

    mode = "";

    line_no = 0;

    x_prev = 0;
    y_prev = 0;
    p_prev = 0;

    glGenTextures(1, &texture_recon);
    glBindTexture(GL_TEXTURE_2D, texture_recon);
    // glTexParameteri(GLenum target, GLenum pname, GLfloat param):
    // Sets the texture parameter.
    // "target" Specifies the target to which the texture is bound for glTexParameter function.
    // "pname" Specifies the symbolic name of a single-valued texture parameter
    // "param" Specifies the value of pname
    // GL_TEXTURE_MIN_FILTER:
    // The texture minifying function is used whenever the level-of-detail function used when sampling from teh texture determines that the texture
    // should be minified. Functions include: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR
    // GL_TEXTURE_MAG_FILTER:
    // The texture magnification function is used whenever the level-of-detail function used when sampling from the texture determines that the texture should be magnified.
    // The Functions include: GL_NEAREST, GL_LINEAR
    // glPixelStorei:
    // Sets pixel storage modes taht affect the operation of subsequent glReadPixels as well as the unpacking of texture patterns (like glTextImage2D & glTexSubImage2D)
    // These settings stick with the texture that's bound. You only need to set them once.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
}

guiProcess::~guiProcess() {

}

Events guiProcess::preprocess_file() {
    if (!ref_period && !nn_window) {
        Events process = read_file(filename, height, width);
        return process;
    } else {
        Events input_event = read_file(filename, height, width);
        Events process;
    
        if (ref_period && nn_window) {
            Events ref_event;
            refractoryFiltering(input_event, ref_event, ref_period);
            ref_event.event_count = ref_event.x.size();

            nnFiltering(ref_event, process, nn_window);
            process.event_count = process.x.size();
            ref_event.~Events();
        } else if (ref_period) {
            refractoryFiltering(input_event, process, ref_period);
            process.event_count = process.x.size();
        } else if (nn_window) {
            refractoryFiltering(input_event, process, nn_window);
            process.event_count = process.x.size();
        }

        return process;
    }
}

void guiProcess::preprocess_initialisation() {
    image = cv::Mat(height, width, CV_8UC1, cv::Scalar(0));

    // Allocate memory on the graphics card for the texture. It's fine if image is empty, the texture will just appear black until you update it.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);

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
    process_events = preprocess_file();
    // std::cout << process_events.event_count << std::endl;
}

void guiProcess::preprocess_run() {
    const int pol = process_events.p[event_no] ? 1:0;
    int32_t x = process_events.x[event_no];
    int32_t y = process_events.y[event_no];
    int32_t t = process_events.t[event_no];

    update_sae(surface_area, mode, x, y, t, time_window, quant, prev_time, ktos, ttos, width, height);

    updateImage();

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture)), ImVec2(width, height));

    prev_time = t; // Update prev_time
    event_no++;

    if (event_no >= process_events.event_count || (event_no >= no_of_events && no_of_events != -1)) {
        preprocessing = true;
        display_processed = false;
    }
}

int guiProcess::run_display_reconstructed() {
    image_recon = cv::imread(recon_path + std::to_string(frame) + ".jpg", cv::IMREAD_COLOR);
    if (image_recon.empty()) return 0;

    cv::cvtColor(image_recon, image_recon, cv::COLOR_BGR2RGBA);

    if (first_recon) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_recon.cols, image_recon.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_recon.data);
        first_recon = false;
    }

    ImGui::SetNextWindowSize(ImVec2(0,0));

    glBindTexture(GL_TEXTURE_2D, texture_recon);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_recon.cols, image_recon.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_recon.data);
    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture_recon)), ImVec2(image_recon.cols, image_recon.rows));
    

    if (frame%1530 == 0) frame = 1;
    else frame++;

    frame_debug++; // This displays the total number of frames displayed
    return 1;
}

void guiProcess::live_process_initialisation() {
    if (!filename.empty()) event_file.open(filename, std::ios::in);

    // end_event = (end_event == 0) ? lines : end_event; // Check the terminating condition. If end_event == 0, it means loop till the end of the file.

    ref_mask = std::vector<std::vector<int>>(height, std::vector<int>(width, -ref_period));
    nn_mask = std::vector<std::vector<int>>(height, std::vector<int>(width, -nn_window));

    ttos = 2*(2*ktos + 1);

    if (tos_check) {
        mode = "TOS";
        surface_area = cv::Mat(height, width, CV_8UC1, cv::Scalar(0));
    } else {
        mode = "timestamp";
        surface_area = cv::Mat(height, width, CV_32SC1, cv::Scalar(0));
    }
}

void guiProcess::live_process_run() {
    std::string temp;
    int32_t x = 0;
    int32_t y = 0;
    int32_t t = 0;
    int p = 0;

    if (!std::getline(event_file, temp)) {
        display_live = false;
        return;
    }
    std::stringstream ss(temp);
    std::string s;
    int count = 0;
    while (ss.good()) {
        std::getline(ss, s, ' ');
        if (count == 0) {
            double f = std::stod(s);
            t = (int)(f*multiplier);
        } else if (count == 1) {
            x = std::stoi(s);
        } else if (count == 2) {
            y = std::stoi(s);
        } else {
            s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
            if (s == "1") p = 1;
            else p = 0;
        }
        count++;
    }

    // Check if the event passes Refractory Filtering
    if (ref_period) {
        if (!refractoryFiltering_live(ref_mask, x, y, t, p, ref_period)) {
            line_no++;

            updateImage();

            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
            ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture)), ImVec2(width, height));

            return;
        }
    }
    
    // Check if the event passes NN Filtering
    if (nn_window) {
        if (!nnFiltering_live(nn_mask, x, y, t, p, x_prev, y_prev, p_prev, width, height, nn_window)) {
            line_no++;

            updateImage();

            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
            ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture)), ImVec2(width, height));

            return;
        }
    }
    
    update_sae(surface_area, mode, x, y, t, time_window, quant, prev_time, ktos, ttos, width, height);

    updateImage();

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture)), ImVec2(width, height));

    prev_time = t; // Update prev_time
    line_no++;
}

void guiProcess::resetFunction() {
    on_off_check = false;
    tos_check = false;
    start_processing = false;
    display_reconstructed = false;
    display_live = false;
    display_processed = false;
    preprocessing = false;
    reset = false;
    davis240 = false;
    davis346 = false;
    first_recon = true;
    first_live = true;

    recon_path = "";
    filename = "";

    frame = 1;
    frame_debug = 1;
    time_window = 0;
    height = 0;
    width = 0;
    ref_period = 0;
    nn_window = 0;
    no_of_events = -1;
    event_no = 0;
    ktos = 0;
    ttos = 0;
    prev_time = 0;
    quant = 0;
    line_no = 0;
    //glDeleteTextures(1, &texture);

    if (event_file.is_open()) event_file.close();
}

void guiProcess::updateImage() {
    if (mode == "TOS") {
        cv::cvtColor(surface_area, image, cv::COLOR_BGR2RGBA);
    } else { // For normal timestamp version Time Surface
        cv::Mat temp, heatmap;
        // Normalise Timesurface value to uint8 range
        cv::normalize(surface_area, temp, 0, 255, cv::NORM_MINMAX, CV_8U);
        // Apply Colourmap to the normalised image to generate heatmap
        cv::applyColorMap(temp, heatmap, cv::COLORMAP_JET);
        // Store the heatmap in our image after converting image to RGBA format
        cv::cvtColor(heatmap, image, cv::COLOR_BGR2RGBA);
    }
}