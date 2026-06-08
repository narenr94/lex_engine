#include "manager.h"
#include "player.h"
#include "spaces.h"
#include "lexEnginePlayerInput.h"

#include <stdexcept>
#include <algorithm>



Manager::Manager(std::vector<std::string> playerNames, unsigned int startingMoney)
    : m_playerInputStrategy(0) 
{
    for (unsigned short int i = 0; i < playerNames.size(); ++i) {
        m_players.emplace_back(playerNames[i], 0, startingMoney);
    }

    for(const auto& space : spacesConfig){
        m_unownedSpaces.push_back(std::make_unique<SpacesConfig>(space));
    }

    m_playerInputStrategy = new PlayerInputCli(); // Default to CLI input strategy
}


Manager::~Manager() {
    if(m_playerInputStrategy){
        delete m_playerInputStrategy;
        m_playerInputStrategy = nullptr;
    }
}

std::string Manager::getCurrentPlayerName() const {
    if(m_players.empty()){
        throw std::runtime_error("No players in the game.");
    }
    return m_players[m_currentPlayerIndex].getName();
}

void Manager::moveCurrentPlayer(unsigned short int positionOffset) {
    if(m_players.empty()){
        throw std::runtime_error("No players in the game.");
    }
    m_players[m_currentPlayerIndex].updatePosition(positionOffset);

    processCurrentPlayerLanding(positionOffset);

}

bool Manager::isSpaceOwned(unsigned short int spaceIndex, Player* owner) const {

    for(auto& player : m_players){
        if(player.ownsSpace(spaceIndex)){
            owner = const_cast<Player*>(&player); // Set the owner pointer to the player who owns the space
            return true;
        }
    }
    return false;
}

void Manager::processCurrentPlayerLanding(unsigned short int positionOffset) {
    SpaceType spaceType = spacesConfig[m_players[m_currentPlayerIndex].getPosition()].type;

    try{
        switch(spaceType){
            case SpaceType::Property:
                {
                    Player* owner;
                    if (isSpaceOwned(m_players[m_currentPlayerIndex].getPosition(), owner)) {
                        // Handle rent payment logic here
                        unsigned short int rent = calculatePropertyRent(*owner, m_players[m_currentPlayerIndex].getPosition());
                        moneyTransfer(m_players[m_currentPlayerIndex], *owner, rent);
                    } else {
                        // Handle property purchase logic here
                        playerBuySpace(m_players[m_currentPlayerIndex], m_players[m_currentPlayerIndex].getPosition());
                    }
                    break;
                }
            case SpaceType::Railroad:
                {
                    Player* owner;
                    if (isSpaceOwned(m_players[m_currentPlayerIndex].getPosition(), owner)) {
                        // Handle rent payment logic here
                        unsigned short int rent = calculateRailroadRent(*owner, m_players[m_currentPlayerIndex].getPosition());
                        moneyTransfer(m_players[m_currentPlayerIndex], *owner, rent);
                    } else {
                        // Handle property purchase logic here
                        playerBuySpace(m_players[m_currentPlayerIndex], m_players[m_currentPlayerIndex].getPosition());
                    }
                    break;
                }
            case SpaceType::Utility:
                {
                    Player* owner;
                    if (isSpaceOwned(m_players[m_currentPlayerIndex].getPosition(), owner)) {
                        // Handle rent payment logic here
                        unsigned short int rent = calculateUtilityRent(*owner, m_players[m_currentPlayerIndex].getPosition(), positionOffset);
                        moneyTransfer(m_players[m_currentPlayerIndex], *owner, rent);
                    } else {
                        // Handle property purchase logic here
                        playerBuySpace(m_players[m_currentPlayerIndex], m_players[m_currentPlayerIndex].getPosition());
                    }
                    break;
                }
            case SpaceType::Tax:
                // Handle tax payment logic here
                break;
            case SpaceType::CardSpace:
                // Handle card drawing logic here
                break;
            case SpaceType::Special:
                // Handle special space logic here (e.g., Go, Jail, Free Parking)
                break;
        }
    }
    catch(const std::exception& e){
        // Handle exceptions that may arise during landing processing (e.g., player bankruptcy)
        PRINT("Error processing landing: " + std::string(e.what()));

    }
}

unsigned short int Manager::calculateRailroadRent(Player& owner, unsigned short int position){
    unsigned short int rent = spacesConfig[position].rent[0];
    unsigned short int railroadsOwned = 0;
    for(const auto& space : spacesConfig){
        if(space.type == SpaceType::Railroad && owner.ownsSpace(space.index)){
            railroadsOwned++;
        }
    }

    return rent * railroadsOwned;
}

unsigned short int Manager::calculateUtilityRent(Player& owner, unsigned short int position,unsigned short int positionOffset){

    unsigned short int utilitiesOwned = 0;
    for(const auto& space : spacesConfig){
        if(space.type == SpaceType::Utility && owner.ownsSpace(space.index)){
            utilitiesOwned++;
        }
    }

    switch(utilitiesOwned){
        case 1:
            return 4 * positionOffset;
        case 2:
            return 10 * positionOffset;
        default:
            throw std::runtime_error("Invalid number of utilities owned.");
            break;
    }

    return 4 * positionOffset;
}

void Manager::populateColorGroups() {
    for (const auto& space : spacesConfig) {
        if (space.type == SpaceType::Property) {
            m_colorGroups[space.color].push_back(space.index);
        }
    }
}

unsigned short int Manager::calculatePropertyRent(Player& owner, unsigned short int position) {
    //property has house/hotel
    if(owner.ownsSpace(position)){
        unsigned short int houses = owner.getPropertyHouses(position);
        if(houses > 0){
            if(houses < HOUSE_HOTEL_CONVERSION){
                return spacesConfig[position].getRent(static_cast<rentType>(houses));
            } else {
                return spacesConfig[position].getRent(rentType::Hotel);
            }
        }
    }
    else{
        throw std::runtime_error("Player does not own the property at the given position.");
    }
    
    //does player own all properties in the color group?
    for(const auto& group : m_colorGroups){
        if(std::find(group.second.begin(), group.second.end(), position) != group.second.end()){
            bool ownsAll = true;
            for(const auto& propertyIndex : group.second){
                if(!owner.ownsSpace(propertyIndex)){
                    ownsAll = false;
                    break;
                }
            }
            if(ownsAll){
                return spacesConfig[position].getRent(rentType::NoHouse) * COLOR_MONOPOLY_BONUS; // Double rent for owning all properties in the color group
            }
        }
    }

    //basic rent with no houses and no color group monopoly
    return spacesConfig[position].getRent(rentType::NoHouse);
}


void Manager::moneyTransfer(Player& from, Player& to, unsigned int amount) {
    
    if (from.getMoney() > amount) {
        from.updateMoney(-amount);
        to.updateMoney(amount);
    }
    else{

        //can from player make up the difference?
        bool canMakeUpDifference = (from.getSellableNetWorth() >= amount);
        if(canMakeUpDifference){
            //ask from player to sell assets to make up the difference
            SellOptions options;
            options.formulateSellOptions(from);
            SellOptions soldOptions = m_playerInputStrategy->askToSellForMoney(amount - from.getMoney(), options);
            sellAssetsForMoney(from, soldOptions);

            if(from.getMoney() >= amount){
                from.updateMoney(-amount);
                to.updateMoney(amount);
            } else {
                throw std::runtime_error("Mistake to sellable networth calculation!!!");
            }

        }
        else{
            //declare bankruptcy
            executeBankruptcy(from, to);
        }

        
    }
    
}



void Manager::sellAssetsForMoney(Player& from, SellOptions& soldOptions) {
    // Process sold properties
    if(!soldOptions.propertiesToSell.empty()){
        for (const auto& prop : soldOptions.propertiesToSell) {
            if(prop.second != 0){
                from.removeHousesFromProperty(prop.first, prop.second);
                from.updateMoney(prop.second * spacesConfig[prop.first].houseHotelCost * HOUSE_HOTEL_SELLBACK_RATIO);
            }
            m_unownedSpaces.push_back(from.removeProperty(prop.first));
            from.updateMoney(spacesConfig[prop.first].mortgageValue);
            
        }
    }

    if(!soldOptions.railroadsToSell.empty()){
        for(auto& rail : soldOptions.railroadsToSell){
            m_unownedSpaces.push_back(from.removeRailroad(rail));
            from.updateMoney(spacesConfig[rail].mortgageValue);
        }
    }

    if(!soldOptions.utilitiesToSell.empty()){
        for(auto& util : soldOptions.utilitiesToSell){
            m_unownedSpaces.push_back(from.removeUtility(util));
            from.updateMoney(spacesConfig[util].mortgageValue);
        }
    }

}

void Manager::executeBankruptcy(Player& bankruptPlayer, Player& creditorPlayer){

    SellOptions currAssets;
    currAssets.formulateSellOptions(bankruptPlayer);

    for(auto& prop : currAssets.propertiesToSell){

        creditorPlayer.addProperty(bankruptPlayer.removeProperty(prop.first), prop.second);

    }

    for(auto& rail : currAssets.railroadsToSell){
        creditorPlayer.addRailroad(bankruptPlayer.removeRailroad(rail));
    }

    for(auto& util : currAssets.utilitiesToSell){
        creditorPlayer.addUtility(bankruptPlayer.removeUtility(util));
    }

    creditorPlayer.updateMoney(bankruptPlayer.getMoney());
    bankruptPlayer.updateMoney(-static_cast<int>(bankruptPlayer.getMoney()));

    m_players.erase(std::remove_if(m_players.begin(), m_players.end(), 
        [&bankruptPlayer](const Player& player) { return player.getName() == bankruptPlayer.getName(); }), m_players.end());

}

void Manager::playerBuySpace(Player& player, unsigned short int spaceIndex){

    bool buy = m_playerInputStrategy->askToBuySpace(spacesConfig[spaceIndex]);

    if(buy){
        for(auto& unOwned : m_unownedSpaces){
            if(unOwned->index == spaceIndex){
                std::unique_ptr<SpacesConfig> temp = std::move(unOwned);
                m_unownedSpaces.erase(std::remove(m_unownedSpaces.begin(), m_unownedSpaces.end(), unOwned), m_unownedSpaces.end());
                switch(temp->type){
                    case SpaceType::Property:
                        player.addProperty(temp);
                        break;
                    case SpaceType::Railroad:
                        player.addRailroad(temp);
                        break;
                    case SpaceType::Utility:
                        player.addUtility(temp);
                        break;
                    default:
                        throw std::runtime_error("Bought un-buyable space type!!!");
                }
            }
        }
    }

}