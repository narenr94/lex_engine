#include "player.h"
#include "spaces.h"

#include <algorithm>

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

void Player::addProperty(std::unique_ptr<SpacesConfig> property, unsigned short int houses) {
    ownedProperties[std::move(property)] = houses;
}

std::unique_ptr<SpacesConfig> Player::removeProperty(unsigned short int propertyIndex) {
    
    auto it = std::find_if(ownedProperties.begin(), ownedProperties.end(), 
        [propertyIndex](const std::pair<std::unique_ptr<SpacesConfig>, unsigned short int>& prop) {
            return prop.first->index == propertyIndex;
        });

    if (it != ownedProperties.end()) {
        std::unique_ptr<SpacesConfig> ret = std::move(const_cast<std::unique_ptr<SpacesConfig>&>(it->first));
        
        ownedProperties.erase(it);
        
        return ret; 
    }

    return nullptr;
}

void Player::addRailroad(std::unique_ptr<SpacesConfig> railroadIndex) {
    ownedRailroads.push_back(std::move(railroadIndex));
}

std::unique_ptr<SpacesConfig>  Player::removeRailroad(unsigned short int railroadIndex) {
    
    for(auto& rail : ownedRailroads){
        if(rail->index == railroadIndex){
            std::unique_ptr<SpacesConfig> ret = std::move(rail);
            ownedRailroads.erase(std::remove(ownedRailroads.begin(), ownedRailroads.end(), rail), ownedRailroads.end());
            return ret;
        }
    }

    return nullptr;
}

void Player::addUtility(std::unique_ptr<SpacesConfig> utility) {
    ownedUtilities.push_back(std::move(utility));
}

std::unique_ptr<SpacesConfig> Player::removeUtility(unsigned short int utilityIndex) {
    for(auto& util : ownedUtilities){
        if(util->index == utilityIndex){
            std::unique_ptr<SpacesConfig> ret = std::move(util);
            ownedUtilities.erase(std::remove(ownedUtilities.begin(), ownedUtilities.end(), util), ownedUtilities.end());
            return ret;
        }
    }
    return nullptr;
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
    for(auto& prop : ownedProperties){
        if(prop.first->index == propertyIndex){
            return true;
        }
    }

    return false;
}

bool Player::ownsRailroad(unsigned short int railroadIndex) const {
    for(auto& rail : ownedRailroads){
        if(rail->index == railroadIndex){
            return true;
        }
    }

    return false;
}

bool Player::ownsUtility(unsigned short int utilityIndex) const {
    
    for(auto& util : ownedUtilities){
        if(util->index == utilityIndex){
            return true;
        }
    }

    return false;
}

unsigned short int Player::getPropertyHouses(unsigned short int propertyIndex) const {
    for(auto& prop : ownedProperties){
        if(prop.first->index == propertyIndex){
            return prop.second;
        }
    }
    return 0; 
}

void Player::addHousesToProperty(unsigned short int propertyIndex, unsigned short int housesToAdd) {
    
    for(auto& prop : ownedProperties){
        if(prop.first->index == propertyIndex){
            prop.second += housesToAdd;
            if(prop.second > HOUSE_HOTEL_CONVERSION){
                prop.second = prop.second;
            }
        }
    }
}

void Player::removeHousesFromProperty(unsigned short int propertyIndex, unsigned short int housesToRemove) {
    for(auto& prop : ownedProperties){
        if(prop.first->index == propertyIndex){
            if(housesToRemove > prop.second){
                prop.second = 0;
            }
            else{
                prop.second -= housesToRemove;
            }
        }
    }
    return;
}

unsigned int Player::getSellableNetWorth() const {
    unsigned int netWorth = money;

    for (const auto& property : ownedProperties) {
        
        netWorth += property.first->mortgageValue; // Add property mortgageValue
        netWorth += property.second * property.first->houseHotelCost * HOUSE_HOTEL_SELLBACK_RATIO; // Add value of houses/hotel at sellback ratio
    }

    for (const auto& railroad : ownedRailroads) {
        netWorth += railroad->mortgageValue; // Add railroad mortgageValue
    }

    for (const auto& utility : ownedUtilities) {
        netWorth += utility->mortgageValue; // Add utility mortgageValue
    }

    return netWorth;
}

unsigned int Player::getTotalNetWorth() const {
    unsigned int netWorth = money;

    for (const auto& property : ownedProperties) {
        
        netWorth += property.first->cost; // Add property cost
        netWorth += property.second * property.first->houseHotelCost; // Add value of houses/hotel at full cost
    }

    for (const auto& railroad : ownedRailroads) {
        netWorth += railroad->cost; // Add railroad cost
    }

    for (const auto& utility : ownedUtilities) {
        netWorth += utility->cost; // Add utility cost
    }

    return netWorth;
}

bool Player::ownsSpace(unsigned short int spaceIndex) const {
    return ownsProperty(spaceIndex) || ownsRailroad(spaceIndex) || ownsUtility(spaceIndex);
}

std::vector<std::pair<unsigned short int, unsigned short int>> Player::getOwnedPropertyIndices() const{
    std::vector<std::pair<unsigned short int, unsigned short int>> propertyIndices;

    for (const auto& prop : ownedProperties) {
        propertyIndices.push_back({prop.first->index, prop.second}); // Store property index and number of houses/hotel
    }

    return propertyIndices;
}
std::vector<unsigned short int> Player::getOwnedRailways() const{

    std::vector<unsigned short int> railwayIndices;

    for(const auto& rail : ownedRailroads){
        railwayIndices.push_back(rail->index);
    }

    return railwayIndices;

}
std::vector<unsigned short int> Player::getOwnedUtilities() const{

    std::vector<unsigned short int> utilityIndices;

    for(const auto& util : ownedUtilities){
        utilityIndices.push_back(util->index);
    }

    return utilityIndices;

}
