#ifndef LICENSE_PLATE_RECOGNIZER_H
#define LICENSE_PLATE_RECOGNIZER_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "openalpr/alpr.h"
#include "openalpr/config.h"

struct LicensePlateGeometry
{
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
};

class LicensePlateRecognizer
{
public:
    LicensePlateRecognizer();
    ~LicensePlateRecognizer();
    bool get_plate_detection_status();
    void process_frame(cv::Mat frame);
    LicensePlateGeometry getDetectedGeometry();

private:
    std::string country;
    std::string configFile;
    int topn;
    bool debug_mode;
    bool plate_found;
    alpr::Alpr *alpr;
    LicensePlateGeometry detectedGeometry;
};

#endif //LICENSE_PLATE_RECOGNIZER_H