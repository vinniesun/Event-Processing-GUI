#include "efast.hpp"

// For the circles, the ordering is {x, y}
const int circle1_[8][2] =  { {0, 1}, {1, 1}, {1, 0},
                              {1, -1}, {0, -1}, {-1, -1},
                              {-1, 0}, {-1, 1} };
const int circle2_[12][2] = { {0, 2}, {1, 2}, {2, 1}, {2, 0},
                              {2, -1}, {1, -2}, {0, -2}, {-1, -2},
                              {-2, -1}, {-2, 0}, {-2, 1}, {-1, 2} };
const int circle3_[16][2] = { {0, 3}, {1, 3}, {2, 2}, {3, 1},
                              {3, 0}, {3, -1}, {2, -2}, {1, -3},
                              {0, -3}, {-1, -3}, {-2, -2}, {-3, -1},
                              {-3, 0}, {-3, 1}, {-2, 2}, {-1, 3} };
const int circle4_[20][2] = { {0, 4}, {1, 4}, {2, 3}, {3, 2},
                              {4, 1}, {4, 0}, {4, -1}, {3, -2},
                              {2, -3}, {1, -4}, {0, -4}, {-1, -4},
                              {-2, -3}, {-3, -2}, {-4, -1}, {-4, 0},
                              {-4, 1}, {-3, 2}, {-2, 3}, {-1, 4} };

bool eFast(std::vector<std::vector<int>> sae_, int x, int y, int t, bool p) {
    const int max_scale = 1;

    // only check if not too close to border
    const int cs = max_scale*4;
    const int width = sae_[0].size();
    const int height = sae_.size();
    if (x < cs || x >= width-cs || y < cs || y >= height-cs)
    {
        return false;
    }

    // bool found_streak = false;

    // for (int i=0; i<16; i++)
    // {
    //     for (int streak_size = 3; streak_size<=6; streak_size++)
    //     {
    //         // check that streak event is larger than neighbor
    //         if (sae_[y+circle3_[i][1]][x+circle3_[i][0]] <  sae_[y+circle3_[(i-1+16)%16][1]][x+circle3_[(i-1+16)%16][0]])
    //             continue;

    //         // check that streak event is larger than neighbor
    //         if (sae_[y+circle3_[(i+streak_size-1)%16][1]][x+circle3_[(i+streak_size-1)%16][0]] < sae_[y+circle3_[(i+streak_size)%16][1]][x+circle3_[(i+streak_size)%16][0]])
    //             continue;

    //         int min_t = sae_[y+circle3_[i][1]][x+circle3_[i][0]];
    //         for (int j=1; j<streak_size; j++)
    //         {
    //             const int tj = sae_[y+circle3_[(i+j)%16][1]][x+circle3_[(i+j)%16][0]];
    //             if (tj < min_t)
    //                 min_t = tj;
    //         }

    //         bool did_break = false;
    //         for (int j=streak_size; j<16; j++)
    //         {
    //             const int tj = sae_[y+circle3_[(i+j)%16][1]][x+circle3_[(i+j)%16][0]];

    //             if (tj >= min_t)
    //             {
    //                 did_break = true;
    //                 break;
    //             }
    //         }

    //         if (!did_break)
    //         {
    //             found_streak = true;
    //             break;
    //         }

    //     }
    //     if (found_streak)
    //     {
    //         break;
    //     }
    // }

    // if (found_streak)
    // {
    //     found_streak = false;
    //     for (int i=0; i<20; i++)
    //     {
    //         for (int streak_size = 4; streak_size<=8; streak_size++)
    //         {
    //             // check that first event is larger than neighbor
    //             if (sae_[y+circle4_[i][1]][x+circle4_[i][0]] <  sae_[y+circle4_[(i-1+20)%20][1]][x+circle4_[(i-1+20)%20][0]])
    //                 continue;

    //             // check that streak event is larger than neighbor
    //             if (sae_[y+circle4_[(i+streak_size-1)%20][1]][x+circle4_[(i+streak_size-1)%20][0]] < sae_[y+circle4_[(i+streak_size)%20][1]][x+circle4_[(i+streak_size)%20][0]])
    //                 continue;

    //             int min_t = sae_[y+circle4_[i][1]][x+circle4_[i][0]];
    //             for (int j=1; j<streak_size; j++)
    //             {
    //                 const int tj = sae_[y+circle4_[(i+j)%20][1]][x+circle4_[(i+j)%20][0]];
    //                 if (tj < min_t)
    //                     min_t = tj;
    //             }

    //             bool did_break = false;
    //             for (int j=streak_size; j<20; j++)
    //             {
    //                 const int tj = sae_[y+circle4_[(i+j)%20][1]][x+circle4_[(i+j)%20][0]];
    //                 if (tj >= min_t)
    //                 {
    //                     did_break = true;
    //                     break;
    //                 }
    //             }

    //             if (!did_break)
    //             {
    //                 found_streak = true;
    //                 break;
    //             }
    //         }
    //         if (found_streak)
    //         {
    //             break;
    //         }
    //     }
    // }

    bool found_streak = false;

    for (int i=0; i<12; i++)
    {
        for (int streak_size = 2; streak_size<=4; streak_size++)
        {
            // check that streak event is larger than neighbor
            if (sae_[y+circle2_[i][1]][x+circle2_[i][0]] <  sae_[y+circle2_[(i-1+12)%12][1]][x+circle2_[(i-1+12)%12][0]])
                continue;

            // check that streak event is larger than neighbor
            if (sae_[y+circle2_[(i+streak_size-1)%12][1]][x+circle2_[(i+streak_size-1)%12][0]] < sae_[y+circle2_[(i+streak_size)%12][1]][x+circle2_[(i+streak_size)%12][0]])
                continue;

            int min_t = sae_[y+circle2_[i][1]][x+circle2_[i][0]];
            for (int j=1; j<streak_size; j++)
            {
                const int tj = sae_[y+circle2_[(i+j)%12][1]][x+circle2_[(i+j)%12][0]];
                if (tj < min_t)
                    min_t = tj;
            }

            bool did_break = false;
            for (int j=streak_size; j<12; j++)
            {
                const int tj = sae_[y+circle2_[(i+j)%12][1]][x+circle2_[(i+j)%12][0]];

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
        for (int i=0; i<16; i++)
        {
            for (int streak_size = 3; streak_size<=6; streak_size++)
            {
                // check that first event is larger than neighbor
                if (sae_[y+circle3_[i][1]][x+circle3_[i][0]] <  sae_[y+circle3_[(i-1+16)%16][1]][x+circle3_[(i-1+16)%16][0]])
                    continue;

                // check that streak event is larger than neighbor
                if (sae_[y+circle3_[(i+streak_size-1)%16][1]][x+circle3_[(i+streak_size-1)%16][0]] < sae_[y+circle3_[(i+streak_size)%16][1]][x+circle3_[(i+streak_size)%16][0]])
                    continue;

                int min_t = sae_[y+circle3_[i][1]][x+circle3_[i][0]];
                for (int j=1; j<streak_size; j++)
                {
                    const int tj = sae_[y+circle3_[(i+j)%16][1]][x+circle3_[(i+j)%16][0]];
                    if (tj < min_t)
                        min_t = tj;
                }

                bool did_break = false;
                for (int j=streak_size; j<16; j++)
                {
                    const int tj = sae_[y+circle3_[(i+j)%16][1]][x+circle3_[(i+j)%16][0]];
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

bool eFast(cv::Mat sae_, int x, int y, int t, bool p)  {
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

    for (int i=0; i<16; i++)
    {
        for (int streak_size = 3; streak_size<=6; streak_size++)
        {
            // check that streak event is larger than neighbor
            if (sae_.at<uint8_t>(y+circle3_[i][1], x+circle3_[i][0]) <  sae_.at<uint8_t>(y+circle3_[(i-1+16)%16][1], x+circle3_[(i-1+16)%16][0]))
                continue;

            // check that streak event is larger than neighbor
            if (sae_.at<uint8_t>(y+circle3_[(i+streak_size-1)%16][1], x+circle3_[(i+streak_size-1)%16][0]) < sae_.at<uint8_t>(y+circle3_[(i+streak_size)%16][1], x+circle3_[(i+streak_size)%16][0]))
                continue;

            int min_t = sae_.at<uint8_t>(y+circle3_[i][1], x+circle3_[i][0]);
            for (int j=1; j<streak_size; j++)
            {
                const int tj = sae_.at<uint8_t>(y+circle3_[(i+j)%16][1], x+circle3_[(i+j)%16][0]);
                if (tj < min_t)
                    min_t = tj;
            }

            bool did_break = false;
            for (int j=streak_size; j<16; j++)
            {
                const int tj = sae_.at<uint8_t>(y+circle3_[(i+j)%16][1], x+circle3_[(i+j)%16][0]);

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
        for (int i=0; i<20; i++)
        {
            for (int streak_size = 4; streak_size<=8; streak_size++)
            {
                // check that first event is larger than neighbor
                if (sae_.at<uint8_t>(y+circle4_[i][1], x+circle4_[i][0]) <  sae_.at<uint8_t>(y+circle4_[(i-1+20)%20][1], x+circle4_[(i-1+20)%20][0]))
                    continue;

                // check that streak event is larger than neighbor
                if (sae_.at<uint8_t>(y+circle4_[(i+streak_size-1)%20][1], x+circle4_[(i+streak_size-1)%20][0]) < sae_.at<uint8_t>(y+circle4_[(i+streak_size)%20][1], x+circle4_[(i+streak_size)%20][0]))
                    continue;

                int min_t = sae_.at<uint8_t>(y+circle4_[i][1], x+circle4_[i][0]);
                for (int j=1; j<streak_size; j++)
                {
                    const int tj = sae_.at<uint8_t>(y+circle4_[(i+j)%20][1], x+circle4_[(i+j)%20][0]);
                    if (tj < min_t)
                        min_t = tj;
                }

                bool did_break = false;
                for (int j=streak_size; j<20; j++)
                {
                    const int tj = sae_.at<uint8_t>(y+circle4_[(i+j)%20][1], x+circle4_[(i+j)%20][0]);
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