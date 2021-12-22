#include "gtest/gtest.h"
#include <iostream>
#include <fstream>

#include <Camera.h>
#include <Config.h>

TEST(CameraTest, test1_init_object)
{
    Camera cd;
}

TEST(ConfgiTest, test_1_init_default_config)
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
