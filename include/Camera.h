#ifndef CAMERA_H
#define CAMERA_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/videoio.hpp>

class Camera
{
public:
    Camera();
    ~Camera();
    bool get_next_frame(cv::Mat *f);

private:
    cv::VideoCapture *cam;
    cv::Mat *frame;
};

#endif //CAMERA_H