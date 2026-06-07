#pragma once

#include <string>

#define HOUSE_HOTEL_CONVERSION 5
#define COLOR_MONOPOLY_BONUS 2
#define HOUSE_HOTEL_SELLBACK_RATIO 0.5

class Player; // Forward declaration

enum class SpaceType{
    Special,
    Property,
    CardSpace,
    Tax,
    Railroad,
    Utility
};

enum class SpaceColor{
    Brown,
    LightBlue,
    Magenta,
    Orange,
    Red,
    Yellow,
    Green,
    DarkBlue,
    NA
};

enum class rentType{
    NoHouse,
    OneHouse,
    TwoHouses,
    ThreeHouses,
    FourHouses,
    Hotel
};

struct SpacesConfig{
    unsigned short int index;
    std::string name;
    SpaceType type;
    SpaceColor color;
    unsigned short int cost;
    unsigned short int mortgageValue;
    unsigned short int houseHotelCost;
    unsigned short int rent[static_cast<int>(rentType::Hotel) + 1];

    unsigned short int getRent(rentType type) const {
        return rent[static_cast<int>(type)];
    }



};

struct SellOptions{
    std::map<unsigned short int, unsigned short int> propertiesToSell; // Map of property index to number of houses to sell (0-4 for houses, 5 for hotel)
    std::vector<unsigned short int> railroadsToSell; // List of railroad indices to sell
    std::vector<unsigned short int> utilitiesToSell; // List of utility indices to sell


    void formulateSellOptions(const Player& player);

};