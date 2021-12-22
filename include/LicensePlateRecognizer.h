#ifndef LICENSE_PLATE_RECOGNIZER_H
#define LICENSE_PLATE_RECOGNIZER_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "openalpr/alpr.h"
#include "openalpr/config.h"
#include <iostream>

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
    bool readAndProcessFile(std::string filename);
    bool process_frame(cv::Mat f);
    LicensePlateGeometry getDetectedGeometry();

private:
    std::string country;
    std::string configFile;
    int topn;
    bool debug_mode;
    alpr::Alpr *alpr;
    cv::Mat frame;
    bool process_frame(std::string region);
    LicensePlateGeometry detectedGeometry;
};

#endif //LICENSE_PLATE_RECOGNIZER_H