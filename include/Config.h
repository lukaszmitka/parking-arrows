#ifndef CONFIG_H
#define CONFIG_H

#include <LicensePlateRecognizer.h>
#include <nlohmann/json.hpp>

class Config
{
public:
    Config();
    ~Config();

    LicensePlateGeometry get_target_geometry();
    bool set_target_geometry(LicensePlateGeometry target);
    bool using_default_config();

private:
    LicensePlateGeometry target_geometry;
    bool init_default_config();
    bool load_config();
    bool save_config();
    bool default_config_loaded;
};
#endif // CONFIG_H