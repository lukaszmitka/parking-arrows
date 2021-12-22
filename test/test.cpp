#include "gtest/gtest.h"
#include <iostream>
#include <fstream>

#include <Camera.h>
#include <Config.h>

TEST(CameraTest, test1_init_object)
{
    Camera cd;
}

TEST(ConfigTest, test_1_init_default_config)
{
    std::string home_path(getenv("HOME"));
    std::string config_file_path = home_path + "/parking_arrows.cfg";
    std::remove(config_file_path.c_str());
    Config c = Config();
    LicensePlateGeometry g = c.get_target_geometry();
    EXPECT_EQ(g.x, 0);
    EXPECT_EQ(g.y, 0);
    EXPECT_EQ(g.width, 0);
    EXPECT_EQ(g.height, 0);
}

TEST(ConfigTest, test_2_init_load_config)
{
    std::string home_path(getenv("HOME"));
    std::string config_file_path = home_path + "/parking_arrows.cfg";
    std::ofstream config_file;
    config_file.open(config_file_path, std::ios::out | std::ios::trunc);
    config_file << "{\"target_geometry\":{\"height\":84,\"width\":355,\"x\":353,\"y\":222}}\0" << std::ends;
    config_file.close();
    Config c = Config();
    LicensePlateGeometry g = c.get_target_geometry();
    EXPECT_EQ(g.x, 353);
    EXPECT_EQ(g.y, 222);
    EXPECT_EQ(g.width, 355);
    EXPECT_EQ(g.height, 84);
}

TEST(ConfigTest, test_2_set_config)
{
    std::string home_path(getenv("HOME"));
    std::string config_file_path = home_path + "/parking_arrows.cfg";
    std::remove(config_file_path.c_str());
    Config c = Config();
    LicensePlateGeometry custom_geometry = {
        .x = 100,
        .y = 50,
        .width = 124,
        .height = 43};
    EXPECT_TRUE(c.set_target_geometry(custom_geometry));
    LicensePlateGeometry expected_geometry = c.get_target_geometry();
    EXPECT_EQ(expected_geometry.x, 100);
    EXPECT_EQ(expected_geometry.y, 50);
    EXPECT_EQ(expected_geometry.width, 124);
    EXPECT_EQ(expected_geometry.height, 43);
}
