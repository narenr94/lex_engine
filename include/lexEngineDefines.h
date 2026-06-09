#pragma once

#include <string>
#include <map>
#include <iostream>

#define HOUSE_HOTEL_CONVERSION 5
#define COLOR_MONOPOLY_BONUS 2
#define HOUSE_HOTEL_SELLBACK_RATIO 0.5

#define INCOME_TAX 200
#define LUXURY_TAX 100

#define GO_PAY 200

#define PER_HOUSE_REPAIR_COST 25
#define SPEEDING_FINE 15
#define CHAIRMAN_OF_BOARD_AMOUNT 50
#define BANK_DIVIDENT_AMOUNT 50
#define BUILDING_LOAN_MATURES_AMOUNT 150

#define PRINT(text) do { std::cout << text << std::endl; } while (0)

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

const std::vector<std::pair<SpaceColor, std::string>> g_spaceColorToString = {
    {SpaceColor::Brown, "brown"},
    {SpaceColor::LightBlue, "light blue"},
    {SpaceColor::Magenta, "magenta"},
    {SpaceColor::Orange, "orange"},
    {SpaceColor::Red, "red"},
    {SpaceColor::Yellow, "yellow"},
    {SpaceColor::Green, "green"},
    {SpaceColor::DarkBlue, "dark blue"},
    {SpaceColor::NA, "NA"}
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

    bool operator== (const SpacesConfig& other) const{
        return (name == other.name);
    }

};

struct SellOptions{
    std::map<unsigned short int, unsigned short int> propertiesToSell; // Map of property index to number of houses to sell (0-4 for houses, 5 for hotel)
    std::vector<unsigned short int> railroadsToSell; // List of railroad indices to sell
    std::vector<unsigned short int> utilitiesToSell; // List of utility indices to sell


    void formulateSellOptions(const Player& player);

};

std::string spaceColorToString(SpaceColor sc);