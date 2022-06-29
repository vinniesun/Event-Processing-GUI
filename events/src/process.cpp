// Homebrew install Eigen @ /usr/local/Cellar/eigen/3.4.0_1/include/eigen3/
// Compile Command
// g++ -std=c++11 -w -I /usr/local/Cellar/eigen/3.4.0_1/include/eigen3/ -o process process.cpp efast.cpp arcstar.cpp
// g++ -std=c++11 -w -O2 -I /usr/local/Cellar/eigen/3.4.0_1/include/eigen3/ -o process process.cpp efast.cpp arcstar.cpp
// g++ -std=c++11 -w -O2 -I /usr/local/Cellar/eigen/3.4.0_1/include/eigen3/ -I /usr/local/Cellar/opencv/4.5.5_1/include/opencv4/ -o process process.cpp efast.cpp arcstar.cpp luvharris.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d
#include "process.hpp"
#include "efast.hpp"
#include "arcstar.hpp"
#include "luvharris.hpp"

using namespace Eigen;

static double multiplier = 1e6;

Events::Events(int size, int h, int w) {
    // t.reserve(size);
    // x.reserve(size);
    // y.reserve(size);
    // p.reserve(size);
    event_count = size;

    height = h;
    width = w;
}

Events::Events() {

}

Events::~Events() {
    t.clear();
    t.shrink_to_fit();
    x.clear();
    x.shrink_to_fit();
    y.clear();
    y.shrink_to_fit();
    p.clear();
    p.shrink_to_fit();
}

Events read_file(std::string input_file, int height, int width) {
    int lines = 0;
    std::fstream event_file;

    event_file.open(input_file, std::ios::in);
    
    if (event_file.is_open()) {
        std::string temp;
        while (std::getline(event_file, temp)) lines++;
    }
    event_file.close();

    std::cout << "number of lines found: " << lines << std::endl;

    Events all_events(lines, height, width);

    event_file.open(input_file, std::ios::in);
    lines = 0;
    if (event_file.is_open()) {
        std::string temp;
        while (std::getline(event_file, temp)) {
            std::stringstream ss(temp);
            std::string s;
            int count = 0;
            while (ss.good()) {
                std::getline(ss, s, ' ');
                if (count == 0) {
                    double f = std::stod(s);
                    int time = (int)(f*multiplier);
                    // all_events.t[lines] = time;
                    all_events.t.push_back(time);
                } else if (count == 1) {
                    int x = std::stoi(s);
                    // all_events.x[lines] = x;
                    all_events.x.push_back(x);
                } else if (count == 2) {
                    int y = std::stoi(s);
                    // all_events.y[lines] = y;
                    all_events.y.push_back(y);
                } else {
                    s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
                    // if (s == "1") all_events.p[lines] = true;
                    // else all_events.p[lines] = false;
                    if (s == "1") all_events.p.push_back(true);
                    else all_events.p.push_back(false);
                }
                count++;
            }
            lines++;
        }
    }
    event_file.close();
    std::cout << "finished processing" << std::endl;

    return all_events;
}

void update_sae(cv::Mat &sae, std::string mode, int32_t x, int32_t y, int32_t t, int deltaMax, int quant, int prev_time, int ktos, int ttos, int maxX, int maxY) {
    if (mode == "timestamp") {
        sae.at<int>(y, x) = t;
    }
    else if (mode == "delta") {
        int deltaT = t - prev_time;

        for (int i = 0; i < sae.rows; i++) {
            for (int j = 0; j < sae.cols; j++) {
                sae.at<int>(i, j) = ((sae.at<int>(i, j) - deltaT) > 0) ? (sae.at<int>(i, j) - deltaT) : 0;
            }
        }
        sae.at<int>(y, x) = deltaMax;
    }
    else if (mode == "factor") {
        sae.at<int>(y, x) = (t/quant);
    }
    else if (mode == "delta_factor") {
        int deltaT = t - prev_time;

        for (int i = 0; i < sae.rows; i++) {
            for (int j = 0; j < sae.cols; j++) {
                sae.at<int>(i, j) = ((sae.at<int>(i, j) - deltaT) > 0) ? (((sae.at<int>(i, j) - deltaT)/4)*4) : 0;
            }
        }
        sae.at<int>(y, x) = deltaMax*4;
    }
    else if (mode == "TOS") {
        if (!(x < ktos || x >= maxX - ktos || y < ktos || y >= maxY - ktos)) {
            for (int i = x - ktos; i <= x + ktos; i++) { //Col
                for (int j = y - ktos; j <= y + ktos; j++) { //Row
                    sae.at<unsigned char>(j, i) = (sae.at<unsigned char>(j, i) > (255 - ttos)) ? (sae.at<unsigned char>(j, i) - 1) : 0;
                }
            }
            sae.at<unsigned char>(y, x) = 255;
        }   
    }
    else if (mode == "TOS_Quantised") {
        if (!(x < ktos || x >= maxX - ktos || y < ktos || y >= maxY - ktos)) {
            for (int i = x - ktos; i <= x + ktos; i++) { //Col
                for (int j = y - ktos; j <= y + ktos; j++) { //Row
                    sae.at<unsigned char>(j, i) = (sae.at<unsigned char>(j, i) > 0) ? (sae.at<unsigned char>(j, i) - 1) : 0;
                }
            }
            sae.at<unsigned char>(y, x) = ttos;
        }   
    }
}

void update_sae(std::vector<std::vector<int>> &sae, std::string mode, int32_t x, int32_t y, int32_t t, int deltaMax, int quant, int prev_time, int ktos, int ttos, int maxX, int maxY) {
    if (mode == "timestamp") {
        sae[y][x] = t;
    }
    else if (mode == "delta") {
        int deltaT = t - prev_time;

        for (int i = 0; i < maxY; i++) {
            for (int j = 0; j < maxX; j++) {
                sae[i][j] = ((sae[i][j] - deltaT) > 0) ? (sae[i][j] - deltaT) : 0;
            }
        }
        sae[y][x] = deltaMax;
    }
    else if (mode == "factor") {
        sae[y][x] = (t/quant);
    }
    else if (mode == "delta_factor") {
        int deltaT = t - prev_time;

        for (int i = 0; i < maxY; i++) {
            for (int j = 0; j < maxX; j++) {
                sae[i][j] = ((sae[i][j] - deltaT) > 0) ? (((sae[i][j] - deltaT)/4)*4) : 0;
            }
        }
        sae[y][x] = deltaMax*4;
    }
    else if (mode == "TOS") {
        if (!(x < ktos || x >= maxX - ktos || y < ktos || y >= maxY - ktos)) {
            for (int i = x - ktos; i <= x + ktos; i++) { //Col
                for (int j = y - ktos; j <= y + ktos; j++) { //Row
                    sae[j][i] = (sae[j][i] > (255 - ttos)) ? (sae[j][i] - 1) : 0;
                }
            }
            sae[y][x] = 255;
        }   
    }
    else if (mode == "TOS_Quantised") {
        if (!(x < ktos || x >= maxX - ktos || y < ktos || y >= maxY - ktos)) {
            for (int i = x - ktos; i <= x + ktos; i++) { //Col
                for (int j = y - ktos; j <= y + ktos; j++) { //Row
                    sae[j][i] = (sae[j][i] > 0) ? (sae[j][i] - 1) : 0;
                }
            }
            sae[y][x] = ttos;
        }   
    }
}

void refractoryFiltering(Events &currentEvent, Events &outputEvent, int ref_period) {
    std::vector<std::vector<int>> ref_mask (currentEvent.height, std::vector<int>(currentEvent.width, -ref_period));
    outputEvent.width = currentEvent.width;
    outputEvent.height = currentEvent.height;

    // Refractory Filtering
    for (int i = 0; i < currentEvent.event_count; i++) {
        if (currentEvent.t[i] - ref_mask[currentEvent.y[i]][currentEvent.x[i]] > ref_period) {
            outputEvent.x.push_back(currentEvent.x[i]);
            outputEvent.y.push_back(currentEvent.y[i]);
            outputEvent.t.push_back(currentEvent.t[i]);
            outputEvent.p.push_back(currentEvent.p[i]);
            ref_mask[currentEvent.y[i]][currentEvent.x[i]] = currentEvent.t[i];
        }
        // if ((i+1)%15119 == 0) {
        //     std::cout << outputEvent.x.size() << std::endl;
        // }
    }
}

void nnFiltering(Events &currentEvent, Events &outputEvent, int nn_window) {
    outputEvent.width = currentEvent.width;
    outputEvent.height = currentEvent.height;

    int max_x = currentEvent.width-1;
    int max_y = currentEvent.height-1;
    int x_prev = 0;
    int y_prev = 0;
    int p_prev = 0;

    std::vector<std::vector<int>> nn_mask(currentEvent.height, std::vector<int>(currentEvent.width, -nn_window));

    for (int i = 0; i < currentEvent.event_count; i++) {
        int x = currentEvent.x[i];
        int y = currentEvent.y[i];
        int t = currentEvent.t[i];
        int p = currentEvent.p[i];

        if (x_prev != x || y_prev != y || p_prev != p) {
            nn_mask[y][x] = -nn_window;
            
            auto min_x_sub = std::max(0, x-1);
            auto max_x_sub = std::min(max_x, x+1);
            auto min_y_sub = std::max(0, y-1);
            auto max_y_sub = std::min(max_y, y+1);

            std::vector<int> temp;
            for (int j = min_y_sub; j < max_y_sub+1; j++) {
                for (int k = min_x_sub; k < max_x_sub+1; k++) {
                    temp.push_back(nn_mask[j][k]);
                }
            }

            for (auto& v:temp) {
                v = t - v;
            }

            int t_min = *std::min_element(temp.begin(), temp.end());
            if (t_min <= nn_window) {
                outputEvent.x.push_back(x);
                outputEvent.y.push_back(y);
                outputEvent.t.push_back(t);
                outputEvent.p.push_back(p);
            }
        }
        nn_mask[y][x] = t;
        x_prev = x;
        y_prev = y;
        p_prev = p;

        // if ((i+1) == 15118 || (i+1) == 30237 || (i+1) == 45356 || (i+1) == 60475 || (i+1) == 75594) {
        //     std::cout << outputEvent.x.size() << std::endl;
        // }
    }
}

/*int main() {
    double threshold = 0.0;
    //Eigen::MatrixXd masks = Eigen::MatrixXd::Constant(180, 240, -100);
    //masks = masks.unaryExpr([threshold](double x) -> double {return (x < threshold) ? threshold : x;});
    //std::cout << masks << std::endl;

    int ref_period = 1000;
    int nn_window = 5000;

    Events current_event = read_file();
    
    //std::cout << current_event.t[0] << ", " << current_event.x[0] << ", " << current_event.y[0] << ", "  << current_event.p[0] << std::endl;
    //std::cout << current_event.t[1] << ", " << current_event.x[1] << ", " << current_event.y[1] << ", "  << current_event.p[1] << std::endl;

    Events ref_event;
    refractoryFiltering(current_event, ref_event, ref_period);
    //Eigen::MatrixXd ref_mask = Eigen::MatrixXd::Constant(current_event.height, current_event.width, -ref_period);
    //std::cout << mask.rows() << ", " << mask.cols() << std::endl;

    // Refractory Filtering
    //for (int i = 0; i < current_event.event_count; i++) {
    //    if (current_event.t[i] - ref_mask(current_event.y[i], current_event.x[i]) > ref_period) {
    //        ref_event.x.push_back(current_event.x[i]);
    //        ref_event.y.push_back(current_event.y[i]);
    //        ref_event.t.push_back(current_event.t[i]);
    //        ref_event.p.push_back(current_event.p[i]);
    //        ref_mask(current_event.y[i], current_event.x[i]) = current_event.t[i];
    //    }
    //}
    //ref_mask.resize(0,0);
    ref_event.event_count = ref_event.x.size();
    std::cout << "Number of events after ref filtering: " << ref_event.event_count << std::endl;

    int max_x = current_event.width-1;
    int max_y = current_event.height-1;
    //int x_prev = 0;
    //int y_prev = 0;
    //int p_prev = 0;
    Events nn_event;
    //mask = Eigen::MatrixXd::Constant(current_event.height, current_event.width, -nn_window);
    //std::vector<std::vector<int>> nn_mask(current_event.height, std::vector<int>(current_event.width, -nn_window));

    // Nearest Neighbourhood Filtering
    for (int i = 0; i < ref_event.event_count; i++) {
        int x = ref_event.x[i];
        int y = ref_event.y[i];
        int t = ref_event.t[i];
        int p = ref_event.p[i];

        if (x_prev != x || y_prev != y || p_prev != p) {
            nn_mask[y][x] = -nn_window;
            
            auto min_x_sub = std::max(0, x-1);
            auto max_x_sub = std::min(max_x, x+1);
            auto min_y_sub = std::max(0, y-1);
            auto max_y_sub = std::min(max_y, y+1);

            std::vector<int> temp;
            for (int j = min_y_sub; j < max_y_sub+1; j++) {
                for (int k = min_x_sub; k < max_x_sub+1; k++) {
                    temp.push_back(nn_mask[j][k]);
                }
            }

            for (auto& v:temp) {
                v = t - v;
            }

            int t_min = *std::min_element(temp.begin(), temp.end());
            if (t_min <= nn_window) {
                nn_event.x.push_back(x);
                nn_event.y.push_back(y);
                nn_event.t.push_back(t);
                nn_event.p.push_back(p);
            }
        }
        nn_mask[y][x] = t;
        x_prev = x;
        y_prev = y;
        p_prev = p;
    }
    nnFiltering(ref_event, nn_event, nn_window);
    nn_event.event_count = nn_event.x.size();
    std::cout << "Number of Events after NN filtering: " << nn_event.event_count << std::endl;

    //Eigen::MatrixXd sae_[2];

    //sae_[0] = Eigen::MatrixXd::Zero(current_event.height, current_event.width);
    //sae_[1] = Eigen::MatrixXd::Zero(current_event.height, current_event.width);

    std::vector<std::vector<int>> sae_1 (current_event.height, std::vector<int>(current_event.width, 0));
    std::vector<std::vector<int>> sae_0 (current_event.height, std::vector<int>(current_event.width, 0));

    //std::cout << sae_1.size() << ", " << sae_1[0].size() << std::endl;

    int prev_time = 0;

    int efast_corner = 0;
    int arcstar_corner = 0;
    int luvharris_corner = 0;

    int cycle = 0;

    // Corner Evaluation
    std::ofstream outfile;
    int quant = (int)std::pow(2.0, 16.0); // Template is double std::pow(double base, double exponent), hence we need to cast it to an int
    int ktos = 3;
    int ttos = 2*(2*ktos + 1);
    int harris_threshold = 240; // Default is 200
    int blockSize=2; // Neighbourhood Size. Default is 2
    int apertureSize=5; // This is the Sobel Operator's Size. Default is 3.
    double k=0.04;
    cv::Mat lookup_1 = cv::Mat::zeros(sae_1.size(), sae_1[0].size(), CV_8UC1);
    cv::Mat lookup_0 = cv::Mat::zeros(sae_1.size(), sae_1[0].size(), CV_8UC1);
    for (int i = 400000; i < 500000; i++) {
        const int pol = nn_event.p[i] ? 1:0;
        int32_t x = nn_event.x[i];
        int32_t y = nn_event.y[i];
        int32_t t = nn_event.t[i];

        if (pol) {
            //Possible Mode Choices: "timestamp", "delta", "factor", "delta_factor", "TOS"
            update_sae(sae_1, "TOS", x, y, t, 66000, quant, prev_time, ktos, ttos, max_x, max_y);
            if (eFast(sae_1, x, y, t, pol)) {
                efast_corner++;
                outfile.open("eFast_Corners.txt", std::ios_base::app);
                if (!outfile) {
                    std::cout << "Failed to open eFast_Corners.txt" << std::endl;
                    return -1;
                }
                outfile << t << "," << x << "," << y << "," << pol << "\n";
                outfile.close();
            }
            if (arcStar(sae_1, x, y, t, pol)) {
                arcstar_corner++;
                outfile.open("arcStar_Corners.txt", std::ios_base::app);
                if (!outfile) {
                    std::cout << "Failed to open arcStar_Corners.txt" << std::endl;
                    return -1;
                }
                outfile << t << "," << x << "," << y << "," << pol << "\n";
                outfile.close();
            }
            if (cycle > 0) {
                //perform luvharris
                if (luvHarris(lookup_1, x, y, t, pol, harris_threshold)) luvharris_corner++;
            } else {
                //update harris lookup
                cv::Mat temp_sae = convertVectorToMat(sae_1);
                lookup_1 = generateLookup(temp_sae, blockSize, apertureSize, k);
            }
        } else {
            update_sae(sae_0, "TOS", x, y, t, 66000, quant, prev_time, 0, 0, max_x, max_y);
            if (eFast(sae_0, x, y, t, pol)) {
                efast_corner++;
                outfile.open("eFast_Corners.txt", std::ios_base::app);
                if (!outfile) {
                    std::cout << "Failed to open eFast_Corners.txt" << std::endl;
                    return -1;
                }
                outfile << t << "," << x << "," << y << "," << pol << "\n";
                outfile.close();
            }
            if (arcStar(sae_0, x, y, t, pol)) {
                arcstar_corner++;
                outfile.open("arcStar_Corners.txt", std::ios_base::app);
                if (!outfile) {
                    std::cout << "Failed to open arcStar_Corners.txt" << std::endl;
                    return -1;
                }
                outfile << t << "," << x << "," << y << "," << pol << "\n";
                outfile.close();
            }
            if (cycle > 0) {
                //perform luvharris
                if (luvHarris(lookup_0, x, y, t, pol, harris_threshold)) luvharris_corner++;
            } else {
                //update harris lookup
                cv::Mat temp_sae = convertVectorToMat(sae_0);
                lookup_0 = generateLookup(temp_sae, blockSize, apertureSize, k);
            }
        }
        prev_time = t;

        if (cycle == 5000) {
            cycle = 0;
        } else {
            cycle++;
        }

        outfile.open("All_events.txt", std::ios_base::app);
        if (!outfile) {
            std::cout << "Failed to open All_events.txt" << std::endl;
            return -1;
        }
        outfile << t << "," << x << "," << y << "," << pol << "\n";
        outfile.close();
    }

    std::cout << "Number of eFast corner: " << efast_corner << std::endl;
    std::cout << "Number of arcStar corner: " << arcstar_corner << std::endl;
    std::cout << "Number of luvHarris corner: " << luvharris_corner << std::endl;

    return 0;
}*/