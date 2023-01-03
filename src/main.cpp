#include <tclap/CmdLine.h>
#include <Config.h>
#include <Camera.h>
#include <LicensePlateRecognizer.h>
#include <gpiod.hpp>
#include <thread>
#include <chrono>

#define CENTER_RED 24
#define CENTER_GREEN 25
#define CENTER_BLUE 8
#define LEFT_RED 7
#define LEFT_GREEN 1
#define LEFT_BLUE 12
#define RIGHT_RED 16
#define RIGHT_GREEN 20
#define RIGHT_BLUE 21
#define CONFIG_BUTTON 27
#define BUTTON_PRESSED 0
#define BUTTON_RELEASED 1

int main(int argc, char **argv)
{
    std::cout << "parking arrows: main" << std::endl;
    bool set_destination = false;
    bool enable_buttons = true;
    bool single_run = false;
    bool show_preview = false;
    std::cout << "parking arrows: main: processing arguments" << std::endl;
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

    std::cout << "parking arrows: main: GPIO initialization" << std::endl;
    struct gpiod_chip *chip;
    struct gpiod_line *left_led;
    struct gpiod_line *right_led;
    struct gpiod_line *forward_led;
    struct gpiod_line *stop_led;
    struct gpiod_line *wait_led;
    struct gpiod_line *button_input;
    bool blink_led = false;

    chip = gpiod_chip_open("/dev/gpiochip0");
    left_led = gpiod_chip_get_line(chip, LEFT_GREEN);
    right_led = gpiod_chip_get_line(chip, RIGHT_GREEN);
    forward_led = gpiod_chip_get_line(chip, CENTER_GREEN);
    stop_led = gpiod_chip_get_line(chip, CENTER_RED);
    wait_led = gpiod_chip_get_line(chip, CENTER_BLUE);
    button_input = gpiod_chip_get_line(chip, CONFIG_BUTTON);
    int button_state;
    std::cout << "parking arrows: main: GPIO set in/out" << std::endl;
    gpiod_line_request_output(left_led, "left_led", 0);
    gpiod_line_request_output(right_led, "right_led", 0);
    gpiod_line_request_output(forward_led, "forward_led", 0);
    gpiod_line_request_output(stop_led, "stop_led", 0);
    gpiod_line_request_output(wait_led, "wait_led", 0);
    gpiod_line_request_input(button_input, "button_input");

    std::cout << "parking arrows: main: GPIO set values" << std::endl;
    gpiod_line_set_value(forward_led, 1);
    gpiod_line_set_value(stop_led, 1);
    gpiod_line_set_value(wait_led, 1);
    gpiod_line_set_value(left_led, 1);
    gpiod_line_set_value(right_led, 1);

    std::cout << "parking arrows: main: ParkingArrowCamera object" << std::endl;
    ParkingArrowCamera pac = ParkingArrowCamera();
    std::cout << "parking arrows: main: frame object" << std::endl;
    if(pac.is_camera_ready())
    {
        gpiod_line_set_value(forward_led, 0);
        gpiod_line_set_value(stop_led, 0);
        gpiod_line_set_value(wait_led, 0);
        gpiod_line_set_value(left_led, 1);
        gpiod_line_set_value(right_led, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        gpiod_line_set_value(forward_led, 1);
        gpiod_line_set_value(stop_led, 1);
        gpiod_line_set_value(wait_led, 1);
        gpiod_line_set_value(left_led, 0);
        gpiod_line_set_value(right_led, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    gpiod_line_set_value(forward_led, 1);
    gpiod_line_set_value(stop_led, 1);
    gpiod_line_set_value(wait_led, 1);
    gpiod_line_set_value(left_led, 1);
    gpiod_line_set_value(right_led, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    cv::Mat frame(Y_PLANE_HEIGHT, Y_PLANE_WIDTH, CV_8UC3);
    std::cout << "parking arrows: main: LicensePlateRecognizer object" << std::endl;
    LicensePlateRecognizer *lpr;
    lpr = new LicensePlateRecognizer();
    LicensePlateGeometry detected_plate;
    LicensePlateGeometry target_plate;
    std::cout << "parking arrows: main: Init config" << std::endl;
    Config config = Config();
    std::cout << "parking arrows: main: read target" << std::endl;
    target_plate = config.get_target_geometry();

    std::cout << "parking arrows: main: start pac" << std::endl;
    if (pac.start())
    {
        std::cout << "parking arrows: main: Camera started correctly" << std::endl;
    }
    else
    {
        std::cerr << "parking arrows: main: Can not start camera. Shutdown !!!" << std::endl;
        return 1;
    }
    std::cout << "parking arrows: main: while loop" << std::endl;
    while (1)
    {
        gpiod_line_set_value(forward_led, 1);
        gpiod_line_set_value(stop_led, 0);
        gpiod_line_set_value(wait_led, 0);
        gpiod_line_set_value(left_led, 0);
        gpiod_line_set_value(right_led, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        gpiod_line_set_value(forward_led, 0);
        gpiod_line_set_value(stop_led, 1);
        gpiod_line_set_value(wait_led, 0);
        gpiod_line_set_value(left_led, 0);
        gpiod_line_set_value(right_led, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        gpiod_line_set_value(forward_led, 0);
        gpiod_line_set_value(stop_led, 0);
        gpiod_line_set_value(wait_led, 1);
        gpiod_line_set_value(left_led, 0);
        gpiod_line_set_value(right_led, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;

    while (1)
    {
        if (enable_buttons)
        {
            std::cout << "parking arrows: main: loop: check buttons state" << std::endl;
            button_state = gpiod_line_get_value(button_input);
            if (button_state == BUTTON_PRESSED || set_destination == true)
            {
                set_destination = true;
                gpiod_line_set_value(forward_led, 1);
                gpiod_line_set_value(stop_led, 1);
                gpiod_line_set_value(wait_led, 1);
                gpiod_line_set_value(left_led, 1);
                gpiod_line_set_value(right_led, 1);
            }
        }
        std::cout << "parking arrows: main: loop: get next frame" << std::endl;
        if (pac.get_next_frame(&frame))
        {
            std::cout << "parking arrows: main: loop: process frame" << std::endl;
            std::thread frame_process_thread(std::bind(&LicensePlateRecognizer::process_frame, lpr, frame));
            frame_process_thread.join();
            std::cout << "parking arrows: main: loop: get detection state" << std::endl;
            if (lpr->get_plate_detection_status())
            {
                detected_plate = lpr->getDetectedGeometry();
                if (set_destination)
                {
                    config.set_target_geometry(detected_plate);
                    target_plate = config.get_target_geometry();
                    set_destination = false;
                    gpiod_line_set_value(forward_led, 0);
                    gpiod_line_set_value(stop_led, 0);
                    gpiod_line_set_value(wait_led, 0);
                    gpiod_line_set_value(left_led, 1);
                    gpiod_line_set_value(right_led, 1);
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
                        gpiod_line_set_value(wait_led, 0);
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
                        gpiod_line_set_value(wait_led, 0);
                    }
                }
            }
            else
            {
                // plate not detected
                std::cout << "Wait for plate detection" << std::endl;
                blink_led = !blink_led;
                gpiod_line_set_value(forward_led, 0);
                gpiod_line_set_value(stop_led, 0);
                gpiod_line_set_value(left_led, blink_led);
                gpiod_line_set_value(right_led, blink_led);
                gpiod_line_set_value(wait_led, 1);
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    return 0;
}
