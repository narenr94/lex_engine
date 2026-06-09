#pragma once

#include <vector>
#include <map>
#include <memory>

struct SpacesConfig; // Forward declaration

class Player {

    private:
        std::string name;
        unsigned short int position;
        unsigned int money;

        unsigned short int m_getOutOfJail;

        std::map<std::unique_ptr<SpacesConfig>, unsigned short int> ownedProperties; // Map of property index to number of houses (0-4) or hotel (5)
        std::vector<std::unique_ptr<SpacesConfig>> ownedRailroads;
        std::vector<std::unique_ptr<SpacesConfig>> ownedUtilities;

        bool ownsProperty(unsigned short int propertyIndex) const;

        bool ownsRailroad(unsigned short int railroadIndex) const;

        bool ownsUtility(unsigned short int utilityIndex) const;

    public:
        Player(std::string name, unsigned short int position, unsigned int money);
        ~Player();
    

        void updatePosition(unsigned short int positionOffset);

        void updateMoney(int amountOffset);

        void addProperty(std::unique_ptr<SpacesConfig> property, unsigned short int houses = 0);
        std::unique_ptr<SpacesConfig> removeProperty(unsigned short int propertyIndex);

        void addRailroad(std::unique_ptr<SpacesConfig> railroadIndex);
        std::unique_ptr<SpacesConfig>  removeRailroad(unsigned short int railroadIndex);

        void addUtility(std::unique_ptr<SpacesConfig> utilityIndex);
        std::unique_ptr<SpacesConfig>  removeUtility(unsigned short int utilityIndex);

        std::string getName() const;

        unsigned short int getPosition() const;

        unsigned int getMoney() const;

        unsigned short int getPropertyHouses(unsigned short int propertyIndex) const;

        void addHousesToProperty(unsigned short int propertyIndex, unsigned short int housesToAdd);
        void removeHousesFromProperty(unsigned short int propertyIndex, unsigned short int housesToRemove);

        bool ownsSpace(unsigned short int spaceIndex) const;

        unsigned int getSellableNetWorth() const;

        unsigned int getTotalNetWorth() const;

        bool canGetoutofJail() const;

        std::vector<std::pair<unsigned short int, unsigned short int>> getOwnedPropertyIndices() const;
        std::vector<unsigned short int> getOwnedRailways() const;
        std::vector<unsigned short int> getOwnedUtilities() const;


        bool canGetoutofJail() const;

        void incrementGetoutofJail();

        void decrementGetoutofJail();
    
};