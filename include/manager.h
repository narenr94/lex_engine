#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>

class Player; // Forward declaration
class PlayerInputStrategy; // Forward declaration
struct SpacesConfig; // Forward declaration
struct SellOptions; // Forward declaration

class Manager {
    private:
        std::vector<Player> m_players;
        unsigned short int m_currentPlayerIndex;

        std::map<SpaceColor, std::vector<unsigned short int>> m_colorGroups; // Map of color groups to their respective property indices
        PlayerInputStrategy* m_playerInputStrategy = nullptr; // Strategy for handling player input (e.g., CLI)

        std::vector<std::unique_ptr<SpacesConfig>> m_unownedSpaces; 

        void processCurrentPlayerLanding(unsigned short int positionOffset);

        void sellAssetsForMoney(Player& player, SellOptions& soldOptions);

        bool isSpaceOwned(unsigned short int spaceIndex, Player* owner) const;

        unsigned short int calculatePropertyRent(Player& owner, unsigned short int position);

        unsigned short int calculateRailroadRent(Player& owner, unsigned short int position);

        unsigned short int calculateUtilityRent(Player& owner, unsigned short int position, unsigned short int positionOffset);

        void populateColorGroups(); 

        void moneyTransfer(Player& from, Player& to, unsigned int amount);

        void playerBuySpace(Player& player, unsigned short int spaceIndex);

        void executeBankruptcy(Player& bankruptPlayer, Player& creditorPlayer);

    public:
        Manager(std::vector<std::string> playerNames, unsigned int startingMoney);
        ~Manager();

        std::string getCurrentPlayerName() const;

        void moveCurrentPlayer(unsigned short int positionOffset);




};