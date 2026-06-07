#include "player.h"
#include "spaces.h"

Player::Player(std::string name, unsigned short int position, unsigned int money)
    : name(name), position(position), money(money) {}

Player::~Player() {}

void Player::updatePosition(unsigned short int positionOffset) {
    position = (position + positionOffset) % spacesConfig.size(); 
}

void Player::updateMoney(int amountOffset) {
    if (amountOffset < 0 && money < static_cast<unsigned int>(-amountOffset)) {
        money = 0; 
    } else {
        money += amountOffset;
    }
}

void Player::addProperty(unsigned short int propertyIndex, unsigned short int houses) {
    if (!ownsProperty(propertyIndex)) {
        ownedProperties[propertyIndex] = houses;
    }
}

void Player::removeProperty(unsigned short int propertyIndex) {
    if (ownsProperty(propertyIndex)) {
        ownedProperties.erase(propertyIndex);
    }
}

void Player::addRailroad(unsigned short int railroadIndex) {
    if (!ownsRailroad(railroadIndex)) {
        ownedRailroads.push_back(railroadIndex);
    }
}

void Player::removeRailroad(unsigned short int railroadIndex) {
    if (ownsRailroad(railroadIndex)) {
        ownedRailroads.erase(std::remove(ownedRailroads.begin(), ownedRailroads.end(), railroadIndex), ownedRailroads.end());
    }
}

void Player::addUtility(unsigned short int utilityIndex) {
    if (!ownsUtility(utilityIndex)) {
        ownedUtilities.push_back(utilityIndex);
    }
}

void Player::removeUtility(unsigned short int utilityIndex) {
    if (ownsUtility(utilityIndex)) {
        ownedUtilities.erase(std::remove(ownedUtilities.begin(), ownedUtilities.end(), utilityIndex), ownedUtilities.end());
    }
}

std::string Player::getName() const {
    return name;
}

unsigned short int Player::getPosition() const {
    return position;
}

unsigned int Player::getMoney() const {
    return money;
}

bool Player::ownsProperty(unsigned short int propertyIndex) const {
    return ownedProperties.find(propertyIndex) != ownedProperties.end();
}

bool Player::ownsRailroad(unsigned short int railroadIndex) const {
    return std::find(ownedRailroads.begin(), ownedRailroads.end(), railroadIndex) != ownedRailroads.end();
}

bool Player::ownsUtility(unsigned short int utilityIndex) const {
    return std::find(ownedUtilities.begin(), ownedUtilities.end(), utilityIndex) != ownedUtilities.end();
}

unsigned short int Player::getPropertyHouses(unsigned short int propertyIndex) const {
    auto it = ownedProperties.find(propertyIndex);
    if (it != ownedProperties.end()) {
        return it->second;
    }
    return 0; 
}

void Player::addHousesToProperty(unsigned short int propertyIndex, unsigned short int housesToAdd) {
    if (ownsProperty(propertyIndex)) {
        ownedProperties[propertyIndex] += housesToAdd;
        if(ownedProperties[propertyIndex] > HOUSE_HOTEL_CONVERSION) {
            ownedProperties[propertyIndex] = HOUSE_HOTEL_CONVERSION; // Maximum of 5 houses (4 houses + 1 hotel)
        }
    }
}

void Player::removeHousesFromProperty(unsigned short int propertyIndex, unsigned short int housesToRemove) {
    if (ownsProperty(propertyIndex)) {
        if (ownedProperties[propertyIndex] >= housesToRemove) {
            ownedProperties[propertyIndex] -= housesToRemove;
        } else {
            ownedProperties[propertyIndex] = 0; // Can't have negative houses
        }
    }
    return;
}

unsigned int Player::getSellableNetWorth() const {
    unsigned int netWorth = money;

    for (const auto& property : ownedProperties) {
        unsigned short int propertyIndex = property.first;
        unsigned short int houses = property.second;

        netWorth += spacesConfig[propertyIndex].mortgageValue; // Add property mortgageValue
        netWorth += houses * spacesConfig[propertyIndex].houseHotelCost * HOUSE_HOTEL_SELLBACK_RATIO; // Add value of houses/hotel at sellback ratio
    }

    for (const auto& railroad : ownedRailroads) {
        netWorth += spacesConfig[railroad].mortgageValue; // Add railroad mortgageValue
    }

    for (const auto& utility : ownedUtilities) {
        netWorth += spacesConfig[utility].mortgageValue; // Add utility mortgageValue
    }

    return netWorth;
}

unsigned int Player::getTotalNetWorth() const {
    unsigned int netWorth = money;

    for (const auto& property : ownedProperties) {
        unsigned short int propertyIndex = property.first;
        unsigned short int houses = property.second;

        netWorth += spacesConfig[propertyIndex].cost; // Add property cost
        netWorth += houses * spacesConfig[propertyIndex].houseHotelCost; // Add value of houses/hotel at full cost
    }

    for (const auto& railroad : ownedRailroads) {
        netWorth += spacesConfig[railroad].cost; // Add railroad cost
    }

    for (const auto& utility : ownedUtilities) {
        netWorth += spacesConfig[utility].cost; // Add utility cost
    }

    return netWorth;
}

void Player::askToSellForMoney(unsigned int amountNeeded) {
     
}
