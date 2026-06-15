#pragma once

#include <string>
#include <vector>
#include <map>


//------------Forward Declaration
struct SpacesConfig; 
struct SellOptions;
class Player;

//-------------------------------

class PlayerInputStrategy{

    protected:

        Player* m_player;

    public:

        PlayerInputStrategy(Player* t_player);

        virtual ~PlayerInputStrategy() = default;

        virtual bool roll2d6Dice(unsigned short int& roll) = 0;

        virtual SellOptions askToSellForMoney(unsigned int amountNeeded, const SellOptions& options) = 0;

        virtual bool askToBuySpace(SpacesConfig& spConfig) = 0;

        virtual unsigned short int JailOptions(bool getOutOfJailCardAvailable) = 0;

        virtual unsigned short int askToBuildHouseHotel(SpacesConfig& spConfig, unsigned short int currentHouses, unsigned short int maxBuyable) = 0;

        unsigned int calculatePropertySellValue(unsigned short int propertyIndex, unsigned short int houses);

        unsigned int calculateRailroadSellValue(unsigned short int railroadIndex);

        unsigned int calculateUtilitySellValue(unsigned short int utilityIndex);        
        
};


class PlayerInputCli : public PlayerInputStrategy {

    private:
        std::vector<unsigned short int> displayLiquidationMenu(const std::map<unsigned short int, unsigned short int>& properties, 
                                            const std::vector<unsigned short int>& railroads, 
                                            const std::vector<unsigned short int>& utilities);

        unsigned short int getPlayerChoice(unsigned short int optionsCount);
        void displayPropertyCard(SpacesConfig& spConfig);
        void displayRailroadCard(SpacesConfig& spConfig);
        void displayUtilityCard(SpacesConfig& spConfig);
   
    public:
        PlayerInputCli(Player* t_player);
        ~PlayerInputCli();


        bool roll2d6Dice(unsigned short int& roll);
        SellOptions askToSellForMoney(unsigned int amountNeeded, const SellOptions& options);

        bool askToBuySpace(SpacesConfig& spConfig);

        unsigned short int askToBuildHouseHotel(SpacesConfig& spConfig, unsigned short int currentHouses, unsigned short int maxBuyable);

        unsigned short int JailOptions(bool getOutOfJailCardAvailable);
};