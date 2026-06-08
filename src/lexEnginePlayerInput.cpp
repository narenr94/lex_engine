#include "lexEnginePlayerInput.h"
#include "spaces.h"

#include <random>
#include <algorithm>
#include <iostream>


PlayerInputCli::PlayerInputCli() {}
PlayerInputCli::~PlayerInputCli() {}

unsigned short int rollDie(){
    // 'static' ensures the engine is initialized only ONCE.
    // std::random_device seeds the generator with hardware entropy (if available).
    static std::random_device rd;
    static std::mt19937 generator(rd());
    
    // Define an inclusive range [1, 6]
    std::uniform_int_distribution<int> distribution(1, 6);
    
    return distribution(generator);
}

void PlayerInputCli::roll2d6Dice(unsigned short int& roll){

    PRINT("Enter any key to roll dice!!!");
    std::cin.get(); // Wait for user input

    unsigned short int die1 = rollDie();
    unsigned short int die2 = rollDie();

    roll = die1 + die2;

    PRINT("You rolled a " + std::to_string(die1) + " and a " + std::to_string(die2) + " for a total of " + std::to_string(roll));

}

unsigned int PlayerInputStrategy::calculatePropertySellValue(unsigned short int propertyIndex, unsigned short int houses) {

    // Base sell value is half the cost of the property
    unsigned int baseValue = spacesConfig[propertyIndex].mortgageValue;

    // Add value for any houses/hotel on the property
    if (houses > 0) {
        baseValue += houses * spacesConfig[propertyIndex].houseHotelCost * HOUSE_HOTEL_SELLBACK_RATIO;
    }

    return baseValue;
}

unsigned int PlayerInputStrategy::calculateRailroadSellValue(unsigned short int railroadIndex){
    return spacesConfig[railroadIndex].mortgageValue;
}

unsigned int PlayerInputStrategy::calculateUtilitySellValue(unsigned short int utilityIndex){
    return spacesConfig[utilityIndex].mortgageValue;
}


std::vector<unsigned short int> PlayerInputCli::displayLiquidationMenu(const std::map<unsigned short int, unsigned short int>& properties, 
                                            const std::vector<unsigned short int>& railroads, 
                                            const std::vector<unsigned short int>& utilities) {
    

    std::vector<unsigned short int> ret;
    
    unsigned short int optionNumber = 0;
    // Build menu based on current assets left to sell
    for (const auto& prop : properties) {
        // Add house options to menu
        optionNumber++;
        PRINT("Option " + std::to_string(optionNumber) + ": Sell Property " + std::to_string(prop.first) + " with "
         + std::to_string(prop.second) + " houses - " + "for $" + std::to_string(calculatePropertySellValue(prop.first, prop.second)));
        ret.push_back(prop.first);
    }
    for (const auto& railroad : railroads) {
        // Add railroad options to menu
        optionNumber++;
        PRINT("Option " + std::to_string(optionNumber) + ": Sell Railroad " + std::to_string(railroad) + " - " + "for $" + std::to_string(calculateRailroadSellValue(railroad)));
        ret.push_back(railroad);

    }
    for (const auto& utility : utilities) {
        // Add utility options to menu
        optionNumber++;
        PRINT("Option " + std::to_string(optionNumber) + ": Sell Utility " + std::to_string(utility) + " - " + "for $" + std::to_string(calculateUtilitySellValue(utility)));
        ret.push_back(utility);
    }
    return ret;
}

unsigned short int PlayerInputCli::getPlayerChoice(unsigned short int optionsCount) {
    unsigned short int choice = 0;
    while (true) {
        PRINT("Please enter the option number of the asset you wish to sell:");
        if (!(std::cin >> choice)) {
            PRINT("Invalid input. Please enter a number.");
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (choice >= 1 && choice <= optionsCount) {
            break; // Valid choice
        } else {
            PRINT("Invalid option. Please enter a number between 1 and " 
                  + std::to_string(optionsCount) + ".");
        }
    }
    return choice;
}


SellOptions PlayerInputCli::askToSellForMoney(unsigned int amountNeeded, const SellOptions& options){
    // Local mutable copies of assets left to sell
    auto trackingProperties = options.propertiesToSell;
    auto trackingRailroads = options.railroadsToSell;
    auto trackingUtilities = options.utilitiesToSell;

    int remainingDebt = static_cast<int>(amountNeeded);

    SellOptions soldOptions; 

    PRINT("==================================================");
    PRINT("          FINANCIAL CRISIS: LIQUIDATION REQUIRED   ");
    PRINT("==================================================");

    while (remainingDebt > 0) {

        PRINT("");
        PRINT("You need to raise $" + std::to_string(remainingDebt) + " to cover your debts.");

        // 1. Rebuild options mapping dynamically based on what's left
        std::vector<unsigned short int> optionsCount = displayLiquidationMenu(trackingProperties, trackingRailroads, trackingUtilities);

        if(optionsCount.empty()){
            PRINT("Unfortunately, you have no assets left to sell. You are bankrupt.");
            return;
        }

        // 2. Get player's choice
        unsigned short int choice = getPlayerChoice(optionsCount.size());
        

        // 3. Apply the selected modification

        bool assetSold = false;

        for(auto& prop : trackingProperties){
            if(prop.first == optionsCount[choice - 1]){
                // Selling a property
                unsigned int sellValue = calculatePropertySellValue(prop.first, prop.second);
                remainingDebt -= sellValue;
                soldOptions.propertiesToSell[prop.first] = prop.second; // Record the sale of this property with its houses
                trackingProperties.erase(prop.first); // Remove from available options
                assetSold = true;
                break;
            }
        }

        if(!assetSold){
            for(auto& railroad : trackingRailroads){
                if(railroad == optionsCount[choice - 1]){
                    // Selling a railroad
                    unsigned int sellValue = calculateRailroadSellValue(railroad);
                    remainingDebt -= sellValue;
                    soldOptions.railroadsToSell.push_back(railroad); // Record the sale of this railroad
                    trackingRailroads.erase(std::remove(trackingRailroads.begin(), trackingRailroads.end(), railroad), trackingRailroads.end()); // Remove from available options
                    assetSold = true;
                    break;
                }
            }
        }

        if(!assetSold){
            for(auto& utility : trackingUtilities){
                if(utility == optionsCount[choice - 1]){
                    // Selling a utility
                    unsigned int sellValue = calculateUtilitySellValue(utility);
                    remainingDebt -= sellValue;
                    soldOptions.utilitiesToSell.push_back(utility); // Record the sale of this utility
                    trackingUtilities.erase(std::remove(trackingUtilities.begin(), trackingUtilities.end(), utility), trackingUtilities.end()); // Remove from available options
                    assetSold = true;
                    break;
                }
            }
        }



        PRINT("");
        PRINT("Asset successfully sold. Recalculating balances...");
    }

    PRINT("");
    PRINT("Success! Your debt is completely cleared.");
    PRINT("");


}


bool PlayerInputCli::askToBuySpace(SpaceConfig& spConfig){
    PRINT("You have landed on " + spConfig.name + " which is available for purchase.");
    switch(spConfig.type){
        case SpaceType::Property:
            displayPropertyCard(spConfig);
            break;
        case SpaceType::Railroad:
            displayRailroadCard(spConfig);
            break;
        case SpaceType::Utility:
            displayUtilityCard(spConfig);
            break;
        default:
            throw std::runtime_error("Trying to buy un-buyable space type!!!");
    }
    PRINT("Would you like to buy this property?");
    PRINT("Option 1 : yes");
    PRINT("Option 2 : no");

    unsigned int choice = getPlayerChoice(2);

    switch(choice){
        case 1:
            return true;
            break;
        case 2:
            return false;
            break;
        default:
            throw std::runtime_error("Invalid choice!!!");
    }

    return false;
}

void PlayerInputCli::displayPropertyCard(SpaceConfig& spConfig){
    PRINT("=====PROPERTY=========");
    PRINT("Name:" + spConfig.name);
    PRINT("Color:" + spaceColorToString(spConfig.color));
    PRINT("Cost:" + std::to_string(spConfig.cost));
    PRINT("Mortage:" + std::to_string(spConfig.mortgageValue));
    PRINT("House/Hotel build cost:" + std::to_string(spConfig.houseHotelCost));

    PRINT("No House rent:" + std::to_string(spConfig.rent[0]));
    PRINT("1 House rent:" + std::to_string(spConfig.rent[1]));
    PRINT("2 House rent:" + std::to_string(spConfig.rent[2]));
    PRINT("3 House rent:" + std::to_string(spConfig.rent[3]));
    PRINT("4 House rent:" + std::to_string(spConfig.rent[4]));
    PRINT("Hotel rent:" + std::to_string(spConfig.rent[5]));

    PRINT("======================");
}

void PlayerInputCli::displayRailroadCard(SpaceConfig& spConfig){
    PRINT("=====RAILROAD=========");
    PRINT("Name:" + spConfig.name);
    PRINT("Cost:" + std::to_string(spConfig.cost));
    PRINT("Mortage:" + std::to_string(spConfig.mortgageValue));

    PRINT("If 1 railroad owned, rent:" + std::to_string(spConfig.rent[0]));
    PRINT("If 2 railroad owned, rent:" + std::to_string(spConfig.rent[0] * 2));
    PRINT("If 3 railroad owned, rent:" + std::to_string(spConfig.rent[0] * 3));
    PRINT("If 4 railroad owned, rent:" + std::to_string(spConfig.rent[0] * 4));

    PRINT("======================");
}


void PlayerInputCli::displayUtilityCard(SpaceConfig& spConfig){
    PRINT("=====UTILITY=========");
    PRINT("Name:" + spConfig.name);
    PRINT("Cost:" + std::to_string(spConfig.cost));
    PRINT("Mortage:" + std::to_string(spConfig.mortgageValue));

    PRINT("If 1 utility owned, 4x dice roll!!!");
    PRINT("If 2 utility owned, 4x dice roll!!!");

    PRINT("======================");
}


unsigned short int PlayerInputCli::askToBuildHouseHotel(SpaceConfig& spConfig, unsigned short int currentHouses, unsigned short int maxBuyable){

    PRINT("Would you like to build house/hotel on your below property?");
    displayPropertyCard(spConfig);
    unsigned short int moreHouses = HOUSE_HOTEL_CONVERSION - currentHouses;
    if(moreHouses > maxBuyable){
        moreHouses = maxBuyable;
    }
    PRINT("You current have " + std:to_string(currentHouses) + " houses on the property, you can afford/build " + std:to_string(moreHouses) + " more!!!");

    for(unsigned short int i = 0; i < moreHouses; i++){
        PRINT("Option" + std::to_string(i +1) + ": " + std::to_string(i +1) + " house");
    }
    PRINT("At " + std::to_string(HOUSE_HOTEL_CONVERSION) + " houses will auto covert to hotel!!!");

    return getPlayerChoice(moreHouses);

}
