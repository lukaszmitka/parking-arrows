#include <tclap/CmdLine.h>
#include <Config.h>
#include <Camera.h>
#include <LicensePlateRecognizer.h>

int main(int argc, char **argv)
{
    bool set_destination = false;
    bool enable_buttons = true;
    bool single_run = false;
    bool show_preview = false;
    try
    {
        TCLAP::CmdLine cmd("Command description message", ' ', "0.0.1");
        TCLAP::SwitchArg setDestSwitch("t", "target", "Set target license plate position and exit", cmd, false);
        TCLAP::SwitchArg buttonsSwitch("b", "buttons", "Enable target setting buttons", cmd, true);
        TCLAP::SwitchArg previewSwitch("p", "preview", "Show camera preview", cmd, false);
        cmd.parse(argc, argv);
        set_destination = setDestSwitch.getValue();
        enable_buttons = buttonsSwitch.getValue();
        show_preview = previewSwitch.getValue();
    }
    catch (TCLAP::ArgException &e) // catch exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        return 0;
    }

    if (set_destination)
    {
        single_run = true;
    }

    Camera cd = Camera();
    cv::Mat frame;
    LicensePlateRecognizer *lpr;
    lpr = new LicensePlateRecognizer();
    LicensePlateGeometry detected_plate;
    LicensePlateGeometry target_plate;
    Config config = Config();
    target_plate = config.get_target_geometry();

    while (1)
    {
        if (enable_buttons)
        {
            // check gpio states
            // set_destination = true;
        }
        if (cd.get_next_frame(&frame))
        {
            if (show_preview)
            {
                cv::imshow("Camera preview", frame);
            }
            if (lpr->process_frame(frame))
            {
                detected_plate = lpr->getDetectedGeometry();
                std::cout << "Frame found at " << detected_plate.x << ", " << detected_plate.y << " with size " << detected_plate.width << ", " << detected_plate.height << std::endl;
                if (set_destination)
                {
                    std::cout << "save detected position as target" << std::endl;
                    config.set_target_geometry(detected_plate);
                    target_plate = config.get_target_geometry();
                    if (single_run)
                    {
                        return 0;
                    }
                }
                else
                {
                    if (target_plate.width > detected_plate.width)
                    {
                        std::cout << "Go forward";
                        if ((target_plate.x - 10) > detected_plate.x)
                        {
                            std::cout << " and turn left";
                        }
                        else if ((target_plate.x + 10) < detected_plate.x + 5)
                        {
                            std::cout << "and turn right";
                        }
                        std::cout << std::endl;
                    }
                    else
                    {
                        std::cout << "STOP" << std::endl;
                    }
                }
            }
        }
    }
    return 0;
}
