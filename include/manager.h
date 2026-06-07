#pragma once

#include <vector>
#include <string>

class Player; // Forward declaration
class PlayerInputStrategy; // Forward declaration

class Manager {
    private:
        std::vector<Player> players;
        unsigned short int currentPlayerIndex;

        std::map<SpaceColor, std::vector<unsigned short int>> colorGroups; // Map of color groups to their respective property indices
        PlayerInputStrategy* m_playerInputStrategy = nullptr; // Strategy for handling player input (e.g., CLI)


        void processCurrentPlayerLanding();

        bool isSpacePropertyOwned(unsigned short int spaceIndex, Player& owner) const;
        bool isSpaceRailroadOwned(unsigned short int spaceIndex, Player& owner) const;
        bool isSpaceUtilityOwned(unsigned short int spaceIndex, Player& owner) const;

        unsigned short int calculatePropertyRent(Player& owner, unsigned short int position);

        void populateColorGroups(); 

        void moneyTransfer(Player& from, Player& to, unsigned int amount);

    public:
        Manager(std::vector<std::string> playerNames, unsigned int startingMoney);
        ~Manager();

        std::string getCurrentPlayerName() const;

        void moveCurrentPlayer(unsigned short int positionOffset);




};