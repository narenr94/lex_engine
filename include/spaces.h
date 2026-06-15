#pragma once

#include "defines.h"

#include <vector>

const std::vector<SpacesConfig> spacesConfig = {
    {0, "Go", SpaceType::Special, SpaceColor::Brown, 0, 0, 0, {0, 0, 0, 0, 0, 0}},
    {1, "Mediterranean Avenue", SpaceType::Property, SpaceColor::Brown, 60, 30, 50, {2, 10, 30, 90, 160, 250}},
    {2, "Community Chest", SpaceType::CardSpace, SpaceColor::NA, 0, 0, 0, {0, 0, 0, 0, 0, 0}},
    {3, "Baltic Avenue", SpaceType::Property, SpaceColor::Brown, 60, 30, 50, {4, 20, 60, 180, 320, 450}},
    {4, "Income Tax", SpaceType::Tax, SpaceColor::NA, 200, 0, 0, {0, 0, 0, 0, 0, 0}},
    {5, "Reading Railroad", SpaceType::Railroad, SpaceColor::NA, 200, 100, 0,{25, 0, 0, 0, 0, 0}},
    {6,"Oriental Avenue",SpaceType::Property ,SpaceColor::LightBlue ,100 ,50 ,50 ,{6 ,30 ,90 ,270 ,400 ,550}},
    {7,"Chance",SpaceType::CardSpace ,SpaceColor::NA ,0 ,0 ,0 ,{0, 0, 0, 0, 0, 0}},
    {8,"Vermont Avenue",SpaceType::Property ,SpaceColor::LightBlue ,100 ,50 ,50 ,{6 ,30 ,90 ,270 ,400 ,550}},
    {9,"Connecticut Avenue",SpaceType::Property ,SpaceColor::LightBlue ,120 ,60 ,50 ,{8 ,40 ,100 ,300 ,450 ,600}},
    {10,"Jail",SpaceType::Special, SpaceColor::NA, 0, 0, 0, {0, 0, 0, 0, 0, 0}},
    {11,"St. Charles Place",SpaceType::Property, SpaceColor::Magenta, 140, 70, 100, {10, 50, 150, 450, 625, 750}},
    {12,"Electric Company",SpaceType::Utility, SpaceColor::NA, 150, 75, 0,{0, 0, 0, 0, 0, 0}},
    {13,"States Avenue",SpaceType::Property, SpaceColor::Magenta, 140, 70, 100,{10, 50, 150, 450, 625, 750}},
    {14,"Virginia Avenue",SpaceType::Property, SpaceColor::Magenta,160 ,80 ,100,{12 ,60 ,180 ,500 ,700 ,900}},
    {15,"Pennsylvania Railroad",SpaceType::Railroad, SpaceColor::NA,200 ,100 ,0,{25, 0, 0, 0, 0, 0}},
    {16,"St. James Place",SpaceType::Property, SpaceColor::Orange,180 ,90 ,100,{14 ,70 ,200 ,550 ,750 ,950}},
    {17,"Community Chest",SpaceType::CardSpace, SpaceColor::NA,0 ,0 ,0,{0, 0, 0, 0, 0, 0}},
    {18,"Tennessee Avenue",SpaceType::Property, SpaceColor::Orange,180 ,90 ,100,{14 ,70 ,200 ,550 ,750 ,950}},
    {19,"New York Avenue",SpaceType::Property, SpaceColor::Orange,200 ,100 ,100,{16 ,80 ,220 ,600 ,800 ,1000}},
    {20,"Free Parking",SpaceType::Special, SpaceColor::NA, 0, 0, 0, {0, 0, 0, 0, 0, 0}},
    {21,"Kentucky Avenue",SpaceType::Property, SpaceColor::Red,220 ,110 ,150,{18 ,90 ,250 ,700 ,875 ,1050}},
    {22,"Chance",SpaceType::CardSpace, SpaceColor::NA, 0, 0, 0, {0, 0, 0, 0, 0, 0}},
    {23,"Indiana Avenue",SpaceType::Property, SpaceColor::Red,220 ,110 ,150,{18 ,90 ,250 ,700 ,875 ,1050}},
    {24,"Illinois Avenue",SpaceType::Property, SpaceColor::Red,240 ,120 ,150,{20 ,100 ,300 ,750 ,925 ,1100}},
    {25,"B&O Railroad",SpaceType::Railroad, SpaceColor::NA,200 ,100 ,0,{25, 0, 0, 0, 0, 0}},
    {26,"Atlantic Avenue",SpaceType::Property, SpaceColor::Yellow,260 ,130 ,150,{22 ,110 ,330 ,800 ,975 ,1150}},
    {27,"Ventnor Avenue",SpaceType::Property, SpaceColor::Yellow,260 ,130 ,150,{22 ,110 ,330 ,800 ,975 ,1150}},
    {28,"Water Works",SpaceType::Utility, SpaceColor::NA,150 ,75 ,0,{0, 0, 0, 0, 0, 0}},
    {29,"Marvin Gardens",SpaceType::Property, SpaceColor::Yellow,280 ,140 ,150,{24 ,120 ,360 ,850 ,1025 ,1200}},
    {30,"Go To Jail",SpaceType::Special, SpaceColor::NA, 0, 0, 0,{0, 0, 0, 0, 0, 0}},
    {31,"Pacific Avenue",SpaceType::Property, SpaceColor::Green,300 ,150 ,200,{26 ,130 ,390 ,900 ,1100 ,1275}},
    {32,"North Carolina Avenue",SpaceType::Property, SpaceColor::Green,300 ,150 ,200,{26 ,130 ,390 ,900 ,1100 ,1275}},
    {33,"Community Chest",SpaceType::CardSpace, SpaceColor::NA, 0, 0, 0,{0, 0, 0, 0, 0, 0}},
    {34,"Pennsylvania Avenue",SpaceType::Property, SpaceColor::Green,320 ,160 ,200,{28 ,150 ,450 ,1000 ,1200 ,1400}},
    {35,"Short Line",SpaceType::Railroad, SpaceColor::NA,200 ,100 ,0,{25, 0, 0, 0, 0, 0}},
    {36,"Chance",SpaceType::CardSpace, SpaceColor::NA, 0, 0, 0,{0, 0, 0, 0, 0, 0}},
    {37,"Park Place",SpaceType::Property, SpaceColor::DarkBlue,350 ,175 ,200,{35 ,175 ,500 ,1100 ,1300 ,1500}},
    {38,"Luxury Tax",SpaceType::Tax, SpaceColor::NA,100 ,0 ,0,{0, 0, 0, 0, 0, 0}},
    {39,"Boardwalk",SpaceType::Property, SpaceColor::DarkBlue,400 ,200 ,200,{50 ,200 ,600 ,1400 ,1700 ,2000}}
};