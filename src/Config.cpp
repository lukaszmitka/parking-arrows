#include "Config.h"

Config::Config()
{
    load_config();
}

Config::~Config()
{
}

LicensePlateGeometry Config::get_target_geometry()
{
    return target_geometry;
}

bool Config::set_target_geometry(LicensePlateGeometry target)
{
    target_geometry = target;
    return save_config();
}

bool Config::load_config()
{
    std::cout << " --- loading config from file --- " << std::endl;
    std::string home_dir(getenv("HOME"));
    std::string config_file_path(home_dir + "/parking_arrows.cfg");
    FILE *config_fd;
    config_fd = fopen(config_file_path.c_str(), "r");
    if (config_fd == NULL)
    {
        std::cerr << "Config file does not exist" << std::endl;
        init_default_config();
        config_fd = fopen(config_file_path.c_str(), "r");
    }
    int c = 0;
    std::string conf_str;
    while (c != EOF)
    {
        c = fgetc(config_fd);
        conf_str.append(1, c);
    }
    fclose(config_fd);
    auto config_json = nlohmann::json::parse(conf_str.c_str());
    std::cout << std::setw(4) << config_json << std::endl;
    target_geometry.x = config_json["target_geometry"]["x"];
    target_geometry.y = config_json["target_geometry"]["y"];
    target_geometry.width = config_json["target_geometry"]["width"];
    target_geometry.height = config_json["target_geometry"]["height"];
    std::cout << " === loading config from file ===" << std::endl;
    return true;
}

bool Config::save_config()
{
    std::string home_dir(getenv("HOME"));
    std::string config_file_path(home_dir + "/parking_arrows.cfg");
    FILE *config_fd = fopen(config_file_path.c_str(), "w");
    nlohmann::json config_json;
    config_json["target_geometry"]["x"] = target_geometry.x;
    config_json["target_geometry"]["y"] = target_geometry.y;
    config_json["target_geometry"]["width"] = target_geometry.width;
    config_json["target_geometry"]["height"] = target_geometry.height;
    fwrite(config_json.dump().c_str(), sizeof(char), config_json.dump().length() + 1, config_fd);
    fclose(config_fd);
    return true;
}

bool Config::init_default_config()
{
    std::cout << " --- init default config in file --- " << std::endl;
    target_geometry.x = 0;
    target_geometry.y = 0;
    target_geometry.width = 0;
    target_geometry.height = 0;
    std::cout << " === init default config in file === " << std::endl;
    return save_config();
}