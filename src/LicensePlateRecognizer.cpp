#include <LicensePlateRecognizer.h>

LicensePlateRecognizer::LicensePlateRecognizer()
{
    topn = 5;
    debug_mode = false;

    configFile = "";
    country = "eu";
    alpr = new alpr::Alpr(country, configFile);
    alpr->setTopN(topn);

    if (debug_mode)
    {
        alpr->getConfig()->setDebug(true);
    }
}

LicensePlateRecognizer::~LicensePlateRecognizer()
{
}

LicensePlateGeometry LicensePlateRecognizer::getDetectedGeometry()
{
    return detectedGeometry;
}

void LicensePlateRecognizer::process_frame(cv::Mat frame)
{
    std::vector<alpr::AlprRegionOfInterest> regionsOfInterest;
    regionsOfInterest.push_back(alpr::AlprRegionOfInterest(0, 0, frame.cols, frame.rows));
    alpr::AlprResults results;
    results = alpr->recognize(frame.data, frame.elemSize(), frame.cols, frame.rows, regionsOfInterest);
    if (results.plates.size() > 0)
    {
        detectedGeometry.x = (results.plates[0].plate_points[0].x + results.plates[0].plate_points[1].x + results.plates[0].plate_points[2].x + results.plates[0].plate_points[3].x) / 4;
        detectedGeometry.y = (results.plates[0].plate_points[0].y + results.plates[0].plate_points[1].y + results.plates[0].plate_points[2].y + results.plates[0].plate_points[3].y) / 4;
        detectedGeometry.width = (results.plates[0].plate_points[1].x + results.plates[0].plate_points[2].x - results.plates[0].plate_points[0].x - results.plates[0].plate_points[3].x) / 2;
        detectedGeometry.height = (results.plates[0].plate_points[2].y + results.plates[0].plate_points[3].y - results.plates[0].plate_points[0].y - results.plates[0].plate_points[1].y) / 2;
        plate_found = true;
    } else 
    {
        plate_found = false;
    }
}

bool LicensePlateRecognizer::get_plate_detection_status()
{
    return plate_found;
}
