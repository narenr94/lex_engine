#include "lexEnginePlayerInput.h"
#include "spaces.h"

#include <random>
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>

#define PRINT(text) do { std::cout << text << std::endl; } while (0)

// Mock Data Registry for compilation context
struct PropertyData {
    std::string name;
    unsigned int house_sale_value;
    unsigned int mortgage_value;
};

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

// Helper Struct to map flat CLI menu options (1, 2, 3...) back to concrete actions
struct MenuAction {
    std::string action_type; // "house", "mortgage_prop", "railroad", "utility"
    unsigned short int asset_index;
    unsigned int cash_value;
};

// --- HELPER FUNCTION 1: POPULATE MENU OPTIONS ---
std::map<int, MenuAction> buildLiquidationMenu(
    const std::map<unsigned short int, unsigned short int>& houses,
    const std::vector<unsigned short int>& railroads,
    const std::vector<unsigned short int>& utilities) 
{
    std::map<int, MenuAction> menu;
    int choice_counter = 1;

    // Houses/Hotels
    for (auto const& [idx, count] : houses) {
        if (count > 0) {
            menu[choice_counter++] = { (count == 5 ? "hotel" : "house"), idx, spacesConfig[idx].houseHotelCost * HOUSE_HOTEL_SELLBACK_RATIO };
        }
    }
    // Bare Properties to Mortgage
    for (auto const& [idx, count] : houses) {
        if (count == 0) {
            menu[choice_counter++] = { "mortgage_prop", idx, spacesConfig[idx].mortgageValue };
        }
    }
    // Railroads
    for (auto idx : railroads) {
        menu[choice_counter++] = { "railroad", idx, spacesConfig[idx].mortgageValue };
    }
    // Utilities
    for (auto idx : utilities) {
        menu[choice_counter++] = { "utility", idx, spacesConfig[idx].mortgageValue };
    }

    return menu;
}

// --- HELPER FUNCTION 2: RENDER MENU TO CLI ---
void displayLiquidationMenu(int debt, const std::map<int, MenuAction>& menu) {
    std::cout << "\n--------------------------------------------------\n";
    std::cout << "Remaining Debt: $" << debt << "\n";
    std::cout << "Select an asset to liquidate:\n\n";

    for (auto const& [choice, action] : menu) {
        std::string propName = spacesConfig[action.asset_index].name;
        
        if (action.action_type == "house" || action.action_type == "hotel") {
            std::cout << "  " << choice << ") Sell 1 " << action.action_type << " on " << propName;
        } else {
            std::cout << "  " << choice << ") Mortgage " << propName;
        }
        std::cout << " (Gives +$" << action.cash_value << ")\n";
    }
}

// --- HELPER FUNCTION 3: GET VALID INPUT ---
int getPlayerChoice(int max_choice) {
    int choice = 0;
    std::cout << "\nEnter selection (1-" << max_choice << "): ";
    std::cin >> choice;

    if (std::cin.fail() || choice < 1 || choice > max_choice) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Invalid selection. Please try again.\n";
        return -1; // Flag invalid entry
    }
    return choice;
}

void PlayerInputCli::askToSellForMoney(unsigned int amountNeeded, const SellOptions& options){
    // Local mutable copies of assets left to sell
    auto trackingHouses = options.propertiesToSell;
    auto trackingRailroads = options.railroadsToSell;
    auto trackingUtilities = options.utilitiesToSell;

    int remainingDebt = static_cast<int>(amountNeeded);

    PRINT("==================================================");
    PRINT("          FINANCIAL CRISIS: LIQUIDATION REQUIRED   ");
    PRINT("==================================================");

    while (remainingDebt > 0) {
        // 1. Rebuild options mapping dynamically based on what's left
        auto menu = buildLiquidationMenu(trackingHouses, trackingRailroads, trackingUtilities);

        if (menu.empty()) {
            PRINT("");
            PRINT("You have no assets left to liquidate. BANKRUPT!");
            return;
        }

        // 2. Display options & gather verified input
        displayLiquidationMenu(remainingDebt, menu);
        int choice = getPlayerChoice(menu.size());
        if (choice == -1) continue; // Loop back if they typed garbage

        // 3. Apply the selected modification
        MenuAction selection = menu[choice];
        remainingDebt -= selection.cash_value;

        // Update local state arrays accordingly
        if (selection.action_type == "house" || selection.action_type == "hotel") {
            trackingHouses[selection.asset_index]--;
        } 
        else if (selection.action_type == "mortgage_prop") {
            trackingHouses.erase(selection.asset_index);
        } 
        else if (selection.action_type == "railroad") {
            trackingRailroads.erase(std::remove(trackingRailroads.begin(), trackingRailroads.end(), selection.asset_index), trackingRailroads.end());
        } 
        else if (selection.action_type == "utility") {
            trackingUtilities.erase(std::remove(trackingUtilities.begin(), trackingUtilities.end(), selection.asset_index), trackingUtilities.end());
        }

        PRINT("");
        PRINT("Asset successfully sold. Recalculating balances...");
    }

    PRINT("");
    PRINT("Success! Your debt is completely cleared.");
    PRINT("");


}
