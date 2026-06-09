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

        void processOwnableSpace(unsigned short int positionOffset, SpaceType type);

        void processTaxSpace();

        void processCardSpace(bool isChance);

        void sellAssetsForMoney(Player& player, SellOptions& soldOptions);

        bool isSpaceOwned(unsigned short int spaceIndex, Player* owner) const;

        unsigned short int calculatePropertyRent(Player& owner, unsigned short int position);

        unsigned short int calculateRailroadRent(Player& owner, unsigned short int position);

        unsigned short int calculateUtilityRent(Player& owner, unsigned short int position, unsigned short int positionOffset);

        void populateColorGroups(); 

        void moneyTransfer(Player& from, Player& to, unsigned int amount);

        void playerBuySpace(Player& player, unsigned short int spaceIndex);

        void executeBankruptcyViaPlayer(Player& bankruptPlayer, Player& creditorPlayer);

        void executeBankruptcyViaBank(Player& bankruptPlayer);

        void playerBuildHouseHotel(Player& player, unsigned short int spaceIndex, unsigned short int currentHouses);

        bool ownsAllColor(SpaceColor sc, Player& owner);

        unsigned short int askCurrentPlayerToRoll2D6();

        void moveCurrentPlayer(unsigned short int positionOffset);

        const SpacesConfig& getClosestSpaceType(SpaceType sType);

        void payUpOrBankrupt(unsigned int amount);

        bool doesPlayerExist(const std::string& plName);

        //card actions

        void advanceToGo();

        void advanceToSpace(const SpacesConfig& sConfig);

        void goBack3Spaces();

        void makeGeneralRepairs();

        void speedingFine();

        void chairmanOfBoard();


    public:
        Manager(std::vector<std::string> playerNames, unsigned int startingMoney);
        ~Manager();

        std::string getCurrentPlayerName() const;

        




};