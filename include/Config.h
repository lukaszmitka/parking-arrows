#ifndef CONFIG_H
#define CONFIG_H

#include <LicensePlateRecognizer.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <iomanip>

class Config
{
public:
    Config();
    ~Config();

    LicensePlateGeometry get_target_geometry();
    bool set_target_geometry(LicensePlateGeometry target);

private:
    LicensePlateGeometry target_geometry;
    bool init_default_config();
    bool load_config();
    bool save_config();
};
#endif // CONFIG_H