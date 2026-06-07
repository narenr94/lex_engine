#pragma once

#include <vector>
#include <map>

class Player {

    private:
        std::string name;
        unsigned short int position;
        unsigned int money;

        std::map<unsigned short int, unsigned short int> ownedProperties; // Map of property index to number of houses (0-4) or hotel (5)
        std::vector<unsigned short int> ownedRailroads;
        std::vector<unsigned short int> ownedUtilities;

    public:
        Player(std::string name, unsigned short int position, unsigned int money);
        ~Player();
    

        void updatePosition(unsigned short int positionOffset);

        void updateMoney(int amountOffset);

        void addProperty(unsigned short int propertyIndex, unsigned short int houses = 0);
        void removeProperty(unsigned short int propertyIndex);

        void addRailroad(unsigned short int railroadIndex);
        void removeRailroad(unsigned short int railroadIndex);

        void addUtility(unsigned short int utilityIndex);
        void removeUtility(unsigned short int utilityIndex);

        std::string getName() const;

        unsigned short int getPosition() const;

        unsigned int getMoney() const;

        unsigned short int getPropertyHouses(unsigned short int propertyIndex) const;

        void addHousesToProperty(unsigned short int propertyIndex, unsigned short int housesToAdd);
        void removeHousesFromProperty(unsigned short int propertyIndex, unsigned short int housesToRemove);

        bool ownsProperty(unsigned short int propertyIndex) const;

        bool ownsRailroad(unsigned short int railroadIndex) const;

        bool ownsUtility(unsigned short int utilityIndex) const;

        unsigned int getSellableNetWorth() const;

        unsigned int getTotalNetWorth() const;

        void askToSellForMoney(unsigned int amountNeeded);
};