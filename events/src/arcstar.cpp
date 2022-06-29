#include "arcstar.hpp"

const int kSmallCircle_[16][2] = {{0, 3}, {1, 3}, {2, 2}, {3, 1},
                            {3, 0}, {3, -1}, {2, -2}, {1, -3},
                            {0, -3}, {-1, -3}, {-2, -2}, {-3, -1},
                            {-3, 0}, {-3, 1}, {-2, 2}, {-1, 3}};
const int kLargeCircle_[20][2] = {{0, 4}, {1, 4}, {2, 3}, {3, 2},
                            {4, 1}, {4, 0}, {4, -1}, {3, -2},
                            {2, -3}, {1, -4}, {0, -4}, {-1, -4},
                            {-2, -3}, {-3, -2}, {-4, -1}, {-4, 0},
                            {-4, 1}, {-3, 2}, {-2, 3}, {-1, 4}};

bool arcStar(std::vector<std::vector<int>> sae_, int x, int y, int t, bool p) {
    // only check if not too close to border
    const int max_scale = 1;
    const int cs = max_scale*4;
    const int width = sae_[0].size();
    const int height = sae_.size();
    if (x < cs || x >= width-cs || y < cs || y >= height-cs)
    {
        return false;
    }

    const int kSmallCircleSize = 16;
    const int kLargeCircleSize = 20;
    const int kSmallMinThresh = 3;
    const int kSmallMaxThresh = 6;
    const int kLargeMinThresh = 4;
    const int kLargeMaxThresh = 8;

    bool is_arc_valid = false;
    // Small Circle exploration
    //std::cout << "SMALL CIRCLE EXPLORATION" << std::endl;
    // Initialize arc from newest element
    int segment_new_min_t = sae_[y+kSmallCircle_[0][1]][x+kSmallCircle_[0][0]];

    // Left and Right are equivalent to CW and CCW as in the paper
    int arc_right_idx = 0;
    int arc_left_idx;
    // Find newest
    for (int i=1; i<kSmallCircleSize; i++) {
      const int t_temp =sae_[y+kSmallCircle_[i][1]][x+kSmallCircle_[i][0]];
      if (t_temp > segment_new_min_t) {
        segment_new_min_t = t_temp;
        arc_right_idx = i; // % End up in the maximum value
        //std::cout << "max right arc index is: " << arc_right_idx << std::endl;
      }
    }
    // Shift to the sides of the newest element;
    arc_left_idx = (arc_right_idx-1+kSmallCircleSize)%kSmallCircleSize;
    arc_right_idx= (arc_right_idx+1)%kSmallCircleSize;
    int arc_left_value = sae_[y+kSmallCircle_[arc_left_idx][1]][x+kSmallCircle_[arc_left_idx][0]];
    int arc_right_value = sae_[y+kSmallCircle_[arc_right_idx][1]][x+kSmallCircle_[arc_right_idx][0]];
    int arc_left_min_t = arc_left_value;
    int arc_right_min_t = arc_right_value;

    // Expand
    // Initial expand does not require checking
    int iteration = 1; // The arc already contain the maximum
    for (; iteration<kSmallMinThresh; iteration++) {
      // Decide the most promising arc
      if (arc_right_value > arc_left_value) { // Right arc
        if (arc_right_min_t < segment_new_min_t) {
            segment_new_min_t = arc_right_min_t;
            //std::cout << "smallest timestamp of right segment: " << segment_new_min_t << std::endl;
        }
        // Expand arc
        arc_right_idx= (arc_right_idx+1)%kSmallCircleSize;
        arc_right_value = sae_[y+kSmallCircle_[arc_right_idx][1]][x+kSmallCircle_[arc_right_idx][0]];
        if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
          arc_right_min_t = arc_right_value;
          //std::cout << "Updated smallest timestamp of right segment: " << arc_right_min_t << std::endl;
        }
      } else { // Left arc
        // Include arc in new segment
        if (arc_left_min_t < segment_new_min_t) {
          segment_new_min_t = arc_left_min_t;
          //std::cout << "smallest timestamp of left segment: " << segment_new_min_t << std::endl;
        }

        // Expand arc
        arc_left_idx= (arc_left_idx-1+kSmallCircleSize)%kSmallCircleSize;
        arc_left_value = sae_[y+kSmallCircle_[arc_left_idx][1]][x+kSmallCircle_[arc_left_idx][0]];
        if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
          arc_left_min_t = arc_left_value;
          //std::cout << "Updated smallest timestamp of right segment: " << arc_left_min_t << std::endl;
        }
      }
    }
    int newest_segment_size = kSmallMinThresh;

    // Further expand until completion of the circle
    for (; iteration<kSmallCircleSize; iteration++) {
      // Decide the most promising arc
      if (arc_right_value > arc_left_value) { // Right arc
        // Include arc in new segment
        if ((arc_right_value >=  segment_new_min_t)) {
          newest_segment_size = iteration+1; // Check
          if (arc_right_min_t < segment_new_min_t) {
            segment_new_min_t = arc_right_min_t;
          }
        }

        // Expand arc
        arc_right_idx= (arc_right_idx+1)%kSmallCircleSize;
        arc_right_value = sae_[y+kSmallCircle_[arc_right_idx][1]][x+kSmallCircle_[arc_right_idx][0]];
        if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
          arc_right_min_t = arc_right_value;
        }
      } else { // Left arc
        // Include arc in new segment
        if ((arc_left_value >=  segment_new_min_t)) {
          newest_segment_size = iteration+1;
          if (arc_left_min_t < segment_new_min_t) {
            segment_new_min_t = arc_left_min_t;
          }
        }

        // Expand arc
        arc_left_idx= (arc_left_idx-1+kSmallCircleSize)%kSmallCircleSize;
        arc_left_value = sae_[y+kSmallCircle_[arc_left_idx][1]][x+kSmallCircle_[arc_left_idx][0]];
        if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
          arc_left_min_t = arc_left_value;
        }
      }
    }

    if (// Corners with newest segment of a minority of elements in the circle
        // These corners are equivalent to those in Mueggler et al. BMVC17
            (newest_segment_size <= kSmallMaxThresh) ||
        // Corners with newest segment of a majority of elements in the circle
        // This can be commented out to decrease noise at expenses of less repeatibility. If you do, DO NOT forget to comment the equilvent line in the large circle
        ((newest_segment_size >= (kSmallCircleSize - kSmallMaxThresh)) && (newest_segment_size <= (kSmallCircleSize - kSmallMinThresh)))) {
      is_arc_valid = true;
    }

    // Large Circle exploration
    if (is_arc_valid) {
    is_arc_valid = false;

      segment_new_min_t = sae_[y+kLargeCircle_[0][1]][x+kLargeCircle_[0][0]];
      arc_right_idx = 0;

      // Initialize in the newest element
      for (int i=1; i<kLargeCircleSize; i++) {
        const int t_temp =sae_[y+kLargeCircle_[i][1]][x+kLargeCircle_[i][0]];
        if (t_temp > segment_new_min_t) {
          segment_new_min_t = t_temp;
          arc_right_idx = i; // % End up in the maximum value
        }
      }
      // Shift to the sides of the newest elements;
      arc_left_idx = (arc_right_idx-1+kLargeCircleSize)%kLargeCircleSize;
      arc_right_idx= (arc_right_idx+1)%kLargeCircleSize;
      arc_left_value = sae_[y+kLargeCircle_[arc_left_idx][1]][x+kLargeCircle_[arc_left_idx][0]];
      arc_right_value = sae_[y+kLargeCircle_[arc_right_idx][1]][x+kLargeCircle_[arc_right_idx][0]];
      arc_left_min_t = arc_left_value;
      arc_right_min_t = arc_right_value;

      // Expand
      // Initial expand does not require checking
      iteration = 1;
      for (; iteration<kLargeMinThresh; iteration++) {
        // Decide the most promising arc
        if (arc_right_value > arc_left_value) { // Right arc
          if (arc_right_min_t < segment_new_min_t) {
              segment_new_min_t = arc_right_min_t;
          }
          // Expand arc
          arc_right_idx= (arc_right_idx+1)%kLargeCircleSize;
          arc_right_value = sae_[y+kLargeCircle_[arc_right_idx][1]][x+kLargeCircle_[arc_right_idx][0]];
          if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
            arc_right_min_t = arc_right_value;
          }
        } else { // Left arc
          // Include arc in new segment
          if (arc_left_min_t < segment_new_min_t) {
            segment_new_min_t = arc_left_min_t;
          }

          // Expand arc
          arc_left_idx= (arc_left_idx-1+kLargeCircleSize)%kLargeCircleSize;
          arc_left_value = sae_[y+kLargeCircle_[arc_left_idx][1]][x+kLargeCircle_[arc_left_idx][0]];
          if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
            arc_left_min_t = arc_left_value;
          }
        }
      }
      newest_segment_size = kLargeMinThresh;

      // Further expand until completion of the circle
      for (; iteration<kLargeCircleSize; iteration++) {
        // Decide the most promising arc
        if (arc_right_value > arc_left_value) { // Right arc
          // Include arc in new segment
          if ((arc_right_value >=  segment_new_min_t)) {
            newest_segment_size = iteration+1;
            if (arc_right_min_t < segment_new_min_t) {
              segment_new_min_t = arc_right_min_t;
            }
          }

          // Expand arc
          arc_right_idx= (arc_right_idx+1)%kLargeCircleSize;
          arc_right_value = sae_[y+kLargeCircle_[arc_right_idx][1]][x+kLargeCircle_[arc_right_idx][0]];
          if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
            arc_right_min_t = arc_right_value;
          }
        } else { // Left arc
          // Include arc in new segment
          if ((arc_left_value >=  segment_new_min_t)) {
            newest_segment_size = iteration+1;
            if (arc_left_min_t < segment_new_min_t) {
              segment_new_min_t = arc_left_min_t;
            }
          }

          // Expand arc
          arc_left_idx= (arc_left_idx-1+kLargeCircleSize)%kLargeCircleSize;
          arc_left_value = sae_[y+kLargeCircle_[arc_left_idx][1]][x+kLargeCircle_[arc_left_idx][0]];
          if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
            arc_left_min_t = arc_left_value;
          }
        }
      }

      if (// Corners with newest segment of a minority of elements in the circle
          // These corners are equivalent to those in Mueggler et al. BMVC17
              (newest_segment_size <= kLargeMaxThresh) ||
          // Corners with newest segment of a majority of elements in the circle
          // This can be commented out to decrease noise at expenses of less repeatibility. If you do, DO NOT forget to comment the equilvent line in the small circle
          (newest_segment_size >= (kLargeCircleSize - kLargeMaxThresh) && (newest_segment_size <= (kLargeCircleSize - kLargeMinThresh))) ) {
        is_arc_valid = true;
      }
    }

    return is_arc_valid;
}

bool arcStar(cv::Mat sae_, int x, int y, int t, bool p) {
    // only check if not too close to border
    const int max_scale = 1;
    const int cs = max_scale*4;
    const int width = sae_.cols;
    const int height = sae_.rows;
    if (x < cs || x >= width-cs || y < cs || y >= height-cs)
    {
        return false;
    }

    const int kSmallCircleSize = 16;
    const int kLargeCircleSize = 20;
    const int kSmallMinThresh = 3;
    const int kSmallMaxThresh = 6;
    const int kLargeMinThresh = 4;
    const int kLargeMaxThresh = 8;

    bool is_arc_valid = false;
    // Small Circle exploration
    //std::cout << "SMALL CIRCLE EXPLORATION" << std::endl;
    // Initialize arc from newest element
    int segment_new_min_t = sae_.at<uint8_t>(y+kSmallCircle_[0][1], x+kSmallCircle_[0][0]);

    // Left and Right are equivalent to CW and CCW as in the paper
    int arc_right_idx = 0;
    int arc_left_idx;
    // Find newest
    for (int i=1; i<kSmallCircleSize; i++) {
      const int t_temp =sae_.at<uint8_t>(y+kSmallCircle_[i][1], x+kSmallCircle_[i][0]);
      if (t_temp > segment_new_min_t) {
        segment_new_min_t = t_temp;
        arc_right_idx = i; // % End up in the maximum value
        //std::cout << "max right arc index is: " << arc_right_idx << std::endl;
      }
    }
    // Shift to the sides of the newest element;
    arc_left_idx = (arc_right_idx-1+kSmallCircleSize)%kSmallCircleSize;
    arc_right_idx= (arc_right_idx+1)%kSmallCircleSize;
    int arc_left_value = sae_.at<uint8_t>(y+kSmallCircle_[arc_left_idx][1], x+kSmallCircle_[arc_left_idx][0]);
    int arc_right_value = sae_.at<uint8_t>(y+kSmallCircle_[arc_right_idx][1], x+kSmallCircle_[arc_right_idx][0]);
    int arc_left_min_t = arc_left_value;
    int arc_right_min_t = arc_right_value;

    // Expand
    // Initial expand does not require checking
    int iteration = 1; // The arc already contain the maximum
    for (; iteration<kSmallMinThresh; iteration++) {
      // Decide the most promising arc
      if (arc_right_value > arc_left_value) { // Right arc
        if (arc_right_min_t < segment_new_min_t) {
            segment_new_min_t = arc_right_min_t;
            //std::cout << "smallest timestamp of right segment: " << segment_new_min_t << std::endl;
        }
        // Expand arc
        arc_right_idx= (arc_right_idx+1)%kSmallCircleSize;
        arc_right_value = sae_.at<uint8_t>(y+kSmallCircle_[arc_right_idx][1], x+kSmallCircle_[arc_right_idx][0]);
        if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
          arc_right_min_t = arc_right_value;
          //std::cout << "Updated smallest timestamp of right segment: " << arc_right_min_t << std::endl;
        }
      } else { // Left arc
        // Include arc in new segment
        if (arc_left_min_t < segment_new_min_t) {
          segment_new_min_t = arc_left_min_t;
          //std::cout << "smallest timestamp of left segment: " << segment_new_min_t << std::endl;
        }

        // Expand arc
        arc_left_idx= (arc_left_idx-1+kSmallCircleSize)%kSmallCircleSize;
        arc_left_value = sae_.at<uint8_t>(y+kSmallCircle_[arc_left_idx][1], x+kSmallCircle_[arc_left_idx][0]);
        if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
          arc_left_min_t = arc_left_value;
          //std::cout << "Updated smallest timestamp of right segment: " << arc_left_min_t << std::endl;
        }
      }
    }
    int newest_segment_size = kSmallMinThresh;

    // Further expand until completion of the circle
    for (; iteration<kSmallCircleSize; iteration++) {
      // Decide the most promising arc
      if (arc_right_value > arc_left_value) { // Right arc
        // Include arc in new segment
        if ((arc_right_value >=  segment_new_min_t)) {
          newest_segment_size = iteration+1; // Check
          if (arc_right_min_t < segment_new_min_t) {
            segment_new_min_t = arc_right_min_t;
          }
        }

        // Expand arc
        arc_right_idx= (arc_right_idx+1)%kSmallCircleSize;
        arc_right_value = sae_.at<uint8_t>(y+kSmallCircle_[arc_right_idx][1], x+kSmallCircle_[arc_right_idx][0]);
        if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
          arc_right_min_t = arc_right_value;
        }
      } else { // Left arc
        // Include arc in new segment
        if ((arc_left_value >=  segment_new_min_t)) {
          newest_segment_size = iteration+1;
          if (arc_left_min_t < segment_new_min_t) {
            segment_new_min_t = arc_left_min_t;
          }
        }

        // Expand arc
        arc_left_idx= (arc_left_idx-1+kSmallCircleSize)%kSmallCircleSize;
        arc_left_value = sae_.at<uint8_t>(y+kSmallCircle_[arc_left_idx][1], x+kSmallCircle_[arc_left_idx][0]);
        if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
          arc_left_min_t = arc_left_value;
        }
      }
    }

    if (// Corners with newest segment of a minority of elements in the circle
        // These corners are equivalent to those in Mueggler et al. BMVC17
            (newest_segment_size <= kSmallMaxThresh) ||
        // Corners with newest segment of a majority of elements in the circle
        // This can be commented out to decrease noise at expenses of less repeatibility. If you do, DO NOT forget to comment the equilvent line in the large circle
        ((newest_segment_size >= (kSmallCircleSize - kSmallMaxThresh)) && (newest_segment_size <= (kSmallCircleSize - kSmallMinThresh)))) {
      is_arc_valid = true;
    }

    // Large Circle exploration
    if (is_arc_valid) {
    is_arc_valid = false;

      segment_new_min_t = sae_.at<uint8_t>(y+kLargeCircle_[0][1], x+kLargeCircle_[0][0]);
      arc_right_idx = 0;

      // Initialize in the newest element
      for (int i=1; i<kLargeCircleSize; i++) {
        const int t_temp =sae_.at<uint8_t>(y+kLargeCircle_[i][1], x+kLargeCircle_[i][0]);
        if (t_temp > segment_new_min_t) {
          segment_new_min_t = t_temp;
          arc_right_idx = i; // % End up in the maximum value
        }
      }
      // Shift to the sides of the newest elements;
      arc_left_idx = (arc_right_idx-1+kLargeCircleSize)%kLargeCircleSize;
      arc_right_idx= (arc_right_idx+1)%kLargeCircleSize;
      arc_left_value = sae_.at<uint8_t>(y+kLargeCircle_[arc_left_idx][1], x+kLargeCircle_[arc_left_idx][0]);
      arc_right_value = sae_.at<uint8_t>(y+kLargeCircle_[arc_right_idx][1], x+kLargeCircle_[arc_right_idx][0]);
      arc_left_min_t = arc_left_value;
      arc_right_min_t = arc_right_value;

      // Expand
      // Initial expand does not require checking
      iteration = 1;
      for (; iteration<kLargeMinThresh; iteration++) {
        // Decide the most promising arc
        if (arc_right_value > arc_left_value) { // Right arc
          if (arc_right_min_t < segment_new_min_t) {
              segment_new_min_t = arc_right_min_t;
          }
          // Expand arc
          arc_right_idx= (arc_right_idx+1)%kLargeCircleSize;
          arc_right_value = sae_.at<uint8_t>(y+kLargeCircle_[arc_right_idx][1], x+kLargeCircle_[arc_right_idx][0]);
          if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
            arc_right_min_t = arc_right_value;
          }
        } else { // Left arc
          // Include arc in new segment
          if (arc_left_min_t < segment_new_min_t) {
            segment_new_min_t = arc_left_min_t;
          }

          // Expand arc
          arc_left_idx= (arc_left_idx-1+kLargeCircleSize)%kLargeCircleSize;
          arc_left_value = sae_.at<uint8_t>(y+kLargeCircle_[arc_left_idx][1], x+kLargeCircle_[arc_left_idx][0]);
          if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
            arc_left_min_t = arc_left_value;
          }
        }
      }
      newest_segment_size = kLargeMinThresh;

      // Further expand until completion of the circle
      for (; iteration<kLargeCircleSize; iteration++) {
        // Decide the most promising arc
        if (arc_right_value > arc_left_value) { // Right arc
          // Include arc in new segment
          if ((arc_right_value >=  segment_new_min_t)) {
            newest_segment_size = iteration+1;
            if (arc_right_min_t < segment_new_min_t) {
              segment_new_min_t = arc_right_min_t;
            }
          }

          // Expand arc
          arc_right_idx= (arc_right_idx+1)%kLargeCircleSize;
          arc_right_value = sae_.at<uint8_t>(y+kLargeCircle_[arc_right_idx][1], x+kLargeCircle_[arc_right_idx][0]);
          if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
            arc_right_min_t = arc_right_value;
          }
        } else { // Left arc
          // Include arc in new segment
          if ((arc_left_value >=  segment_new_min_t)) {
            newest_segment_size = iteration+1;
            if (arc_left_min_t < segment_new_min_t) {
              segment_new_min_t = arc_left_min_t;
            }
          }

          // Expand arc
          arc_left_idx= (arc_left_idx-1+kLargeCircleSize)%kLargeCircleSize;
          arc_left_value = sae_.at<uint8_t>(y+kLargeCircle_[arc_left_idx][1], x+kLargeCircle_[arc_left_idx][0]);
          if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
            arc_left_min_t = arc_left_value;
          }
        }
      }

      if (// Corners with newest segment of a minority of elements in the circle
          // These corners are equivalent to those in Mueggler et al. BMVC17
              (newest_segment_size <= kLargeMaxThresh) ||
          // Corners with newest segment of a majority of elements in the circle
          // This can be commented out to decrease noise at expenses of less repeatibility. If you do, DO NOT forget to comment the equilvent line in the small circle
          (newest_segment_size >= (kLargeCircleSize - kLargeMaxThresh) && (newest_segment_size <= (kLargeCircleSize - kLargeMinThresh))) ) {
        is_arc_valid = true;
      }
    }

    return is_arc_valid;
}