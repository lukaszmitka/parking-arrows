#include <Camera.h>

#include <iostream>

Camera::Camera()
{
    cam = new cv::VideoCapture();
    // try to open first device in the system
    cam->open(0);
    if (cam->isOpened())
    {
        std::cout << "Camera succesfully opened" << std::endl;
        cam->set(cv::CAP_PROP_BUFFERSIZE, 1);
        cam->set(cv::CAP_PROP_FRAME_WIDTH, 320);
        cam->set(cv::CAP_PROP_FRAME_HEIGHT, 240);
    }
}

bool Camera::get_next_frame(cv::Mat *f)
{
    if (cam->isOpened())
    {
        return cam->read(*f);
    }
    return false;
}

Camera::~Camera()
{
    if (cam->isOpened())
    {
        cam->release();
    }
}