#include "player.h"
#include "spaces.h"

#include <algorithm>

Player::Player(std::string name, unsigned short int position, unsigned int money)
    : name(name), position(position), money(money), m_getOutOfJail(0) {}

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
    ownedProperties.push_back({std::move(property), houses});
}

std::unique_ptr<SpacesConfig> Player::removeProperty(unsigned short int propertyIndex) {
    
    for (auto it = ownedProperties.begin(); it != ownedProperties.end(); ++it) {
        if (it->property->index == propertyIndex) {
            std::unique_ptr<SpacesConfig> ret = std::move(it->property);
            ownedProperties.erase(it);  // erase by iterator
            return ret;
        }
    }

    return nullptr;
}

void Player::addRailroad(std::unique_ptr<SpacesConfig> railroadIndex) {
    ownedRailroads.push_back(std::move(railroadIndex));
}

std::unique_ptr<SpacesConfig>  Player::removeRailroad(unsigned short int railroadIndex) {
    
    for (auto it = ownedRailroads.begin(); it != ownedRailroads.end(); ++it) {
        if ((*it)->index == railroadIndex) {
            std::unique_ptr<SpacesConfig> ret = std::move(*it);
            ownedRailroads.erase(it);  // erase by iterator
            return ret;
        }
    }

    return nullptr;
}

void Player::addUtility(std::unique_ptr<SpacesConfig> utility) {
    ownedUtilities.push_back(std::move(utility));
}

std::unique_ptr<SpacesConfig> Player::removeUtility(unsigned short int utilityIndex) {
    for(auto it = ownedUtilities.begin(); it != ownedUtilities.end(); it++){
        if((*it)->index == utilityIndex){
            std::unique_ptr<SpacesConfig> ret = std::move(*it);
            ownedUtilities.erase(it);
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
        if(prop.property->index == propertyIndex){
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
        if(prop.property->index == propertyIndex){
            return prop.houses;
        }
    }
    return 0; 
}

void Player::addHousesToProperty(unsigned short int propertyIndex, unsigned short int housesToAdd) {
    
    for(auto& prop : ownedProperties){
        if(prop.property->index == propertyIndex){
            prop.houses += housesToAdd;
            if(prop.houses > HOUSE_HOTEL_CONVERSION){
                prop.houses = HOUSE_HOTEL_CONVERSION;
            }
        }
    }
}

void Player::removeHousesFromProperty(unsigned short int propertyIndex, unsigned short int housesToRemove) {
    for(auto& prop : ownedProperties){
        if(prop.property->index == propertyIndex){
            if(housesToRemove > prop.houses){
                prop.houses = 0;
            }
            else{
                prop.houses -= housesToRemove;
            }
        }
    }
    return;
}

unsigned int Player::getSellableNetWorth() const {
    unsigned int netWorth = money;

    for (const auto& property : ownedProperties) {
        
        netWorth += property.property->mortgageValue; // Add property mortgageValue
        netWorth += property.houses * property.property->houseHotelCost * HOUSE_HOTEL_SELLBACK_RATIO; // Add value of houses/hotel at sellback ratio
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
        netWorth += property.property->cost; // Add property cost
        netWorth += property.houses * property.property->houseHotelCost; // Add value of houses/hotel at full cost
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
        propertyIndices.push_back({prop.property->index, prop.houses}); // Store property index and number of houses/hotel
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

bool Player::canGetoutofJail() const{
    return (m_getOutOfJail > 0) ? true : false;
}

void Player::incrementGetoutofJail(){
    m_getOutOfJail++;
}

void Player::decrementGetoutofJail(){
    m_getOutOfJail--;
}

bool Player::operator==(const Player& other) const {
    return name == other.getName();
}