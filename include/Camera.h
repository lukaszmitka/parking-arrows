#ifndef PARKING_ARROW_CAMERA_H
#define PARKING_ARROW_CAMERA_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <libcamera/libcamera.h>
#include <sys/mman.h>

#include <iostream>

#define U_PLANE_HEIGHT 120
#define U_PLANE_WIDTH 160
#define Y_PLANE_HEIGHT 2 * U_PLANE_HEIGHT
#define Y_PLANE_WIDTH 2 * U_PLANE_WIDTH
#define FRAME_SIZE U_PLANE_WIDTH * U_PLANE_HEIGHT * 6

class ParkingArrowCamera
{
public:
    ParkingArrowCamera();
    ~ParkingArrowCamera();
    bool get_next_frame(cv::Mat *f);

    bool start();

private:
    std::vector<std::unique_ptr<libcamera::Request>> requests;
    std::unique_ptr<libcamera::CameraManager> cm;
    std::shared_ptr<libcamera::Camera> camera;
    libcamera::FrameBufferAllocator *allocator;
    libcamera::Stream *stream;
    void requestComplete(libcamera::Request *request);
    cv::Mat *frame;
    bool new_frame_received;
    bool camera_ready;
};

#endif //PARKING_ARROW_CAMERA_H