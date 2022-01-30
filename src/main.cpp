#include <tclap/CmdLine.h>
#include <Config.h>
#include <Camera.h>
#include <LicensePlateRecognizer.h>
#include <gpiod.hpp>

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
    struct gpiod_chip *chip;
    struct gpiod_line *left_led;
    struct gpiod_line *right_led;
    struct gpiod_line *forward_led;
    struct gpiod_line *stop_led;
    struct gpiod_line *button_input;

    chip = gpiod_chip_open("/dev/gpiochip0");
    left_led = gpiod_chip_get_line(chip, 17);
    right_led = gpiod_chip_get_line(chip, 27);
    forward_led = gpiod_chip_get_line(chip, 22);
    stop_led = gpiod_chip_get_line(chip, 23);
    button_input = gpiod_chip_get_line(chip, 24);
    int result;
    int button_state;
    result = gpiod_line_request_output(left_led, "left_led", 0);
    result = gpiod_line_request_output(right_led, "right_led", 0);
    result = gpiod_line_request_output(forward_led, "forward_led", 0);
    result = gpiod_line_request_output(stop_led, "stop_led", 0);
    result = gpiod_line_request_input(button_input, "button_input");

    while (1)
    {
        if (enable_buttons)
        {
            button_state = gpiod_line_get_value(button_input);
            if (button_state)
            {
                set_destination = true;
            }
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
                    set_destination = false;
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
                        gpiod_line_set_value(forward_led, 1);
                        gpiod_line_set_value(stop_led, 0);
                        if ((target_plate.x - 10) > detected_plate.x)
                        {
                            std::cout << " and turn left";
                            gpiod_line_set_value(left_led, 1);
                            gpiod_line_set_value(right_led, 0);
                        }
                        else if ((target_plate.x + 10) < detected_plate.x + 5)
                        {
                            std::cout << " and turn right";
                            gpiod_line_set_value(left_led, 0);
                            gpiod_line_set_value(right_led, 1);
                        }
                        else
                        {
                            gpiod_line_set_value(left_led, 0);
                            gpiod_line_set_value(right_led, 0);
                        }
                        std::cout << std::endl;
                    }
                    else
                    {
                        std::cout << "STOP" << std::endl;
                        gpiod_line_set_value(forward_led, 0);
                        gpiod_line_set_value(stop_led, 1);
                        gpiod_line_set_value(left_led, 0);
                        gpiod_line_set_value(right_led, 0);
                    }
                }
            }
            else
            {
                // plate not detected
                std::cout << "Wait for plate detection" << std::endl;
                gpiod_line_set_value(forward_led, 0);
                gpiod_line_set_value(stop_led, 1);
                gpiod_line_set_value(left_led, 1);
                gpiod_line_set_value(right_led, 1);
            }
        }
    }
    return 0;
}
