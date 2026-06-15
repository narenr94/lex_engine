#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <utility>

//-------------- Forward declaration
class Player;
class PlayerInputStrategy;
struct SpacesConfig;
struct SellOptions;
enum class SpaceColor;
enum class SpaceType;

//--------------

class Manager {
    private:
        std::vector<Player> m_players;
        unsigned short int m_currentPlayerIndex;

        std::map<SpaceColor, std::vector<unsigned short int>> m_colorGroups; // Map of color groups to their respective property indices
        
        std::vector<std::unique_ptr<SpacesConfig>> m_unownedSpaces;

        std::vector<std::pair<std::string, unsigned short int>> m_jail; //player name, number of double rolls tried

        void processCurrentPlayerLanding(unsigned short int positionOffset);

        void processOwnableSpace(unsigned short int positionOffset, SpaceType type);

        void processTaxSpace();

        void processCardSpace(bool isChance);

        void processSpecialSpace();

        void sellAssetsForMoney(Player& player, SellOptions& soldOptions);

        bool isSpaceOwned(unsigned short int spaceIndex, Player* owner) const;

        bool isSpaceOwnable(const SpacesConfig& spaceConfig);

        unsigned short int calculatePropertyRent(Player& owner, unsigned short int position);

        unsigned short int calculateRailroadRent(Player& owner, unsigned short int position);

        unsigned short int calculateUtilityRent(Player& owner, unsigned short int position, unsigned short int positionOffset);

        void populateColorGroups(); 

        void moneyTransfer(Player& from, Player& to, unsigned int amount);

        void moneyTransferBank(Player& from, unsigned int amount);

        void playerBuySpace(Player& player, unsigned short int spaceIndex);

        void executeBankruptcyViaPlayer(Player& bankruptPlayer, Player& creditorPlayer);

        void executeBankruptcyViaBank(Player& bankruptPlayer);

        void playerBuildHouseHotel(Player& player, unsigned short int spaceIndex);

        bool ownsAllColor(SpaceColor sc, Player& owner);

        bool askCurrentPlayerToRoll2D6(unsigned short int& roll);

        void moveCurrentPlayer(unsigned short int positionOffset);

        const SpacesConfig& getClosestSpaceType(SpaceType sType);

        void payUpOrBankrupt(unsigned int amount);

        bool doesPlayerExist(const std::string& plName);

        const SpacesConfig& getSpaceConfigByName(const std::string& spaceName);

        void grandOperaNight();

        void birthday();

        void AssessedStreetRepairs();

        void gameLoop();

        void Jail();

        bool rollAndMoveCurrentPlayer();

        //card actions

        void advanceToGo();

        void advanceToSpace(const SpacesConfig& sConfig);

        void goBack3Spaces();

        void makeGeneralRepairs();

        void speedingFine();

        void chairmanOfBoard();

        bool isPlayerInJail(const Player& player, unsigned short int& count);

        void removePlayerFromJailList(const Player& player);

        void removeDefeatedPlayerFromGame(const Player& player);


    public:
        Manager(std::vector<std::string> t_playerNames, unsigned int t_startingMoney);
        ~Manager();

        std::string getCurrentPlayerName() const;

        




};