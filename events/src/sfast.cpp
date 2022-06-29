#include "sfast.hpp"

// For the circles, the ordering is {x, y}
const int circle1_[8][2] =  { {0, 1}, {1, 1}, {1, 0},
                              {1, -1}, {0, -1}, {-1, -1},
                              {-1, 0}, {-1, 1} };
const int circle2_[12][2] = { {0, 2}, {1, 2}, {2, 1}, {2, 0},
                              {2, -1}, {1, -2}, {0, -2}, {-1, -2},
                              {-2, -1}, {-2, 0}, {-2, 1}, {-1, 2} };

bool sFast(cv::Mat sae_, int x, int y, int t, bool p) {
    // SFAST checks for the following streak length:
    // circle1: 3~7
    // circle2: 3~11
    // In the paper they only check 
    const int max_scale = 1;

    // only check if not too close to border
    const int cs = max_scale*4;
    const int width = sae_.cols;
    const int height = sae_.rows;
    if (x < cs || x >= width-cs || y < cs || y >= height-cs)
    {
        return false;
    }

    bool found_streak = false;

    for (int i=0; i<8; i++)
    {
        for (int streak_size = 3; streak_size<=7; streak_size++) // Is it 2-7 or 3-7
        {
            // check that streak event is larger than neighbor
            if (sae_.at<uint8_t>(y+circle1_[i][1], x+circle1_[i][0]) <  sae_.at<uint8_t>(y+circle1_[(i-1+8)%8][1], x+circle1_[(i-1+8)%8][0]))
                continue;

            // check that streak event is larger than neighbor
            if (sae_.at<uint8_t>(y+circle1_[(i+streak_size-1)%8][1], x+circle1_[(i+streak_size-1)%8][0]) < sae_.at<uint8_t>(y+circle1_[(i+streak_size)%8][1], x+circle1_[(i+streak_size)%8][0]))
                continue;

            int min_t = sae_.at<uint8_t>(y+circle1_[i][1], x+circle1_[i][0]);
            for (int j=1; j<streak_size; j++)
            {
                const int tj = sae_.at<uint8_t>(y+circle1_[(i+j)%8][1], x+circle1_[(i+j)%8][0]);
                if (tj < min_t)
                    min_t = tj;
            }

            bool did_break = false;
            for (int j=streak_size; j<8; j++)
            {
                const int tj = sae_.at<uint8_t>(y+circle1_[(i+j)%8][1], x+circle1_[(i+j)%8][0]);

                if (tj >= min_t)
                {
                    did_break = true;
                    break;
                }
            }

            if (!did_break)
            {
                found_streak = true;
                break;
            }

        }
        if (found_streak)
        {
            break;
        }
    }

    if (found_streak)
    {
        found_streak = false;
        for (int i=0; i<12; i++)
        {
            for (int streak_size = 3; streak_size<=11; streak_size++)
            {
                // check that first event is larger than neighbor
                if (sae_.at<uint8_t>(y+circle2_[i][1], x+circle2_[i][0]) <  sae_.at<uint8_t>(y+circle2_[(i-1+12)%12][1], x+circle2_[(i-1+12)%12][0]))
                    continue;

                // check that streak event is larger than neighbor
                if (sae_.at<uint8_t>(y+circle2_[(i+streak_size-1)%12][1], x+circle2_[(i+streak_size-1)%12][0]) < sae_.at<uint8_t>(y+circle2_[(i+streak_size)%12][1], x+circle2_[(i+streak_size)%12][0]))
                    continue;

                int min_t = sae_.at<uint8_t>(y+circle2_[i][1], x+circle2_[i][0]);
                for (int j=1; j<streak_size; j++)
                {
                    const int tj = sae_.at<uint8_t>(y+circle2_[(i+j)%12][1], x+circle2_[(i+j)%12][0]);
                    if (tj < min_t)
                        min_t = tj;
                }

                bool did_break = false;
                for (int j=streak_size; j<12; j++)
                {
                    const int tj = sae_.at<uint8_t>(y+circle2_[(i+j)%12][1], x+circle2_[(i+j)%12][0]);
                    if (tj >= min_t)
                    {
                        did_break = true;
                        break;
                    }
                }

                if (!did_break)
                {
                    found_streak = true;
                    break;
                }
            }
            if (found_streak)
            {
                break;
            }
        }
    }

    return found_streak;
}

bool sFastOuterOnly(cv::Mat sae_, int x, int y, int t, bool p) {
    // SFAST checks for the following streak length:
    // circle1: 3~7
    // circle2: 3~11
    // In the paper they only check 
    const int max_scale = 1;

    // only check if not too close to border
    const int cs = max_scale*4;
    const int width = sae_.cols;
    const int height = sae_.rows;
    if (x < cs || x >= width-cs || y < cs || y >= height-cs)
    {
        return false;
    }

    bool found_streak = false;

    for (int i=0; i<12; i++)
    {
        for (int streak_size = 3; streak_size<=11; streak_size++)
        {
            // check that first event is larger than neighbor
            if (sae_.at<uint8_t>(y+circle2_[i][1], x+circle2_[i][0]) <  sae_.at<uint8_t>(y+circle2_[(i-1+20)%20][1], x+circle2_[(i-1+20)%20][0]))
                continue;

            // check that streak event is larger than neighbor
            if (sae_.at<uint8_t>(y+circle2_[(i+streak_size-1)%20][1], x+circle2_[(i+streak_size-1)%20][0]) < sae_.at<uint8_t>(y+circle2_[(i+streak_size)%20][1], x+circle2_[(i+streak_size)%20][0]))
                continue;

            int min_t = sae_.at<uint8_t>(y+circle2_[i][1], x+circle2_[i][0]);
            for (int j=1; j<streak_size; j++)
            {
                const int tj = sae_.at<uint8_t>(y+circle2_[(i+j)%20][1], x+circle2_[(i+j)%20][0]);
                if (tj < min_t)
                    min_t = tj;
            }

            bool did_break = false;
            for (int j=streak_size; j<12; j++)
            {
                const int tj = sae_.at<uint8_t>(y+circle2_[(i+j)%20][1], x+circle2_[(i+j)%20][0]);
                if (tj >= min_t)
                {
                    did_break = true;
                    break;
                }
            }

            if (!did_break)
            {
                found_streak = true;
                break;
            }
        }
        if (found_streak)
        {
            break;
        }
    }

    return found_streak;
}