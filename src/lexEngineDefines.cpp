#include "lexEngineDefines.h"

#include "player.h"



void SellOptions::formulateSellOptions(const Player& player) {
    
    propertiesToSell.clear();
    railroadsToSell.clear();
    utilitiesToSell.clear();

    auto ownedProperties = player.getOwnedPropertyIndices();
    auto ownedRailroads = player.getOwnedRailways();
    auto ownedUtilities = player.getOwnedUtilities();

    for (const auto& [idx, count] : ownedProperties) {
        propertiesToSell[idx] = count; // Store the number of houses/hotel that can be sold
    }

    for (const auto& idx : ownedRailroads) {
        railroadsToSell.push_back(idx);
    }

    for (const auto& idx : ownedUtilities) {
        utilitiesToSell.push_back(idx);
    }

}


std::string spaceColorToString(SpaceColor sc){
    for(auto& mp : g_spaceColorToString){
        if(mp.first == sc){
            return mp.second;
        }
    }

    return "";
}