#pragma once

#include <string>

struct SpacesConfig; //Forward Declaration
struct SellOptions; //Forward Declaration

class PlayerInputStrategy{

    public:
        virtual ~PlayerInputStrategy() = default;

        virtual void roll2d6Dice(unsigned short int& roll) = 0;

        virtual SellOptions askToSellForMoney(unsigned int amountNeeded, const SellOptions& options) = 0;

        virtual bool askToBuySpace(SpacesConfig& spConfig) = 0;

        virtual unsigned short int askToBuildHouseHotel(SpacesConfig& spConfig, unsigned short int currentHouses, unsigned short int maxBuyable) = 0;

        unsigned int calculatePropertySellValue(unsigned short int propertyIndex, unsigned short int houses);

        unsigned int calculateRailroadSellValue(unsigned short int railroadIndex);

        unsigned int calculateUtilitySellValue(unsigned short int utilityIndex);
        
};


class PlayerInputCli : public PlayerInputStrategy {

    private:
        std::vector<unsigned short int> PlayerInputCli::displayLiquidationMenu(const std::map<unsigned short int, unsigned short int>& properties, 
                                            const std::vector<unsigned short int>& railroads, 
                                            const std::vector<unsigned short int>& utilities);

        unsigned short int getPlayerChoice(unsigned short int optionsCount);
        void displayPropertyCard(SpacesConfig& spConfig);
        void displayRailroadCard(SpacesConfig& spConfig);
        void displayUtilityCard(SpacesConfig& spConfig);
   
    public:
        PlayerInputCli();
        ~PlayerInputCli();


        void roll2d6Dice(unsigned short int& roll);
        SellOptions askToSellForMoney(unsigned int amountNeeded, const SellOptions& options);

        bool askToBuySpace(SpacesConfig& spConfig);

        unsigned short int askToBuildHouseHotel(SpacesConfig& spConfig, unsigned short int currentHouses, unsigned short int maxBuyable);
};