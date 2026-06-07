#include "lexEngineDefines.h"

#include "player.h"



void SellOptions::formulateSellOptions(const Player& player) {
    
    propertiesToSell.clear();
    railroadsToSell.clear();
    utilitiesToSell.clear();

    for (const auto& [idx, count] : player.ownedProperties) {
        if (count > 0) {
            propertiesToSell[idx] = count; // Store the number of houses/hotel that can be sold
        }
    }

    for (const auto& idx : player.ownedRailroads) {
        railroadsToSell.push_back(idx);
    }

    for (const auto& idx : player.ownedUtilities) {
        utilitiesToSell.push_back(idx);
    }

}