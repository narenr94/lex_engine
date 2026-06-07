#include "manager.h"
#include "player.h"
#include "spaces.h"
#include "lexEnginePlayerInput.h"



Manager::Manager(std::vector<std::string> playerNames, unsigned int startingMoney)
    : currentPlayerIndex(0) 
{
    for (unsigned short int i = 0; i < playerNames.size(); ++i) {
        players.emplace_back(playerNames[i], 0, startingMoney);
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
    if(players.empty()){
        throw std::runtime_error("No players in the game.");
    }
    return players[currentPlayerIndex].getName();
}

void Manager::moveCurrentPlayer(unsigned short int positionOffset) {
    if(players.empty()){
        throw std::runtime_error("No players in the game.");
    }
    players[currentPlayerIndex].updatePosition(positionOffset);

    processCurrentPlayerLanding();

}

bool Manager::isSpacePropertyOwned(unsigned short int spaceIndex, Player& owner) const {
    for (const auto& player : players) {
        if (player.ownsProperty(spaceIndex)) {
            owner = const_cast<Player&>(player);
            return true;
        }
    }
    return false;
}

bool Manager::isSpaceRailroadOwned(unsigned short int spaceIndex, Player& owner) const {
    for (const auto& player : players) {
        if (player.ownsRailroad(spaceIndex)) {
            owner = const_cast<Player&>(player);
            return true;
        }
    }
    return false;
}

bool Manager::isSpaceUtilityOwned(unsigned short int spaceIndex, Player& owner) const {
    for (const auto& player : players) {
        if (player.ownsUtility(spaceIndex)) {
            owner = const_cast<Player&>(player);
            return true;
        }
    }
    return false;
}

void Manager::processCurrentPlayerLanding() {
    SpaceType spaceType = spacesConfig[players[currentPlayerIndex].getPosition()].type;

    switch(spaceType){
        case SpaceType::Property:
             {
                Player owner;
                if (isSpacePropertyOwned(players[currentPlayerIndex].getPosition(), owner)) {
                    // Handle rent payment logic here
                    unsigned short int rent = calculatePropertyRent(owner, players[currentPlayerIndex].getPosition());
                    // Deduct rent from current player and add to owner
                    // Ensure current player has enough money to pay rent
                    players[currentPlayerIndex].updateMoney(-rent);
                    owner.updateMoney(rent);
                } else {
                    // Handle property purchase logic here
                }
                break;
            }
        case SpaceType::Railroad:
             {
                Player owner;
                if (isSpaceRailroadOwned(players[currentPlayerIndex].getPosition(), owner)) {
                    // Handle rent payment logic here
                    unsigned short int rent = calculatePropertyRent(owner, players[currentPlayerIndex].getPosition());
                    // Deduct rent from current player and add to owner
                    // Ensure current player has enough money to pay rent
                    players[currentPlayerIndex].updateMoney(-rent);
                    owner.updateMoney(rent);
                } else {
                    // Handle property purchase logic here
                }
                break;
            }
        case SpaceType::Utility:
             {
                Player owner;
                if (isSpaceUtilityOwned(players[currentPlayerIndex].getPosition(), owner)) {
                    // Handle rent payment logic here
                    unsigned short int rent = calculatePropertyRent(owner, players[currentPlayerIndex].getPosition());
                    // Deduct rent from current player and add to owner
                    // Ensure current player has enough money to pay rent
                    players[currentPlayerIndex].updateMoney(-rent);
                    owner.updateMoney(rent);
                } else {
                    // Handle property purchase logic here
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

void Manager::populateColorGroups() {
    for (const auto& space : spacesConfig) {
        if (space.type == SpaceType::Property) {
            colorGroups[space.color].push_back(space.index);
        }
    }
}

unsigned short int Manager::calculatePropertyRent(Player& owner, unsigned short int position) {
    //property has house/hotel
    if(owner.ownsProperty(position)){
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
    for(const auto& group : colorGroups){
        if(std::find(group.second.begin(), group.second.end(), position) != group.second.end()){
            bool ownsAll = true;
            for(const auto& propertyIndex : group.second){
                if(!owner.ownsProperty(propertyIndex)){
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
            m_playerInputStrategy->askToSellForMoney(amount - from.getMoney(), options);
        }
        else{
            //declare bankruptcy
        }

        
    }
    
}