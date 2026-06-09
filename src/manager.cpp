#include "manager.h"
#include "player.h"
#include "spaces.h"
#include "lexEnginePlayerInput.h"
#include "cards.h"

#include <stdexcept>
#include <algorithm>
#include <random>

bool endGame = false;


Manager::Manager(std::vector<std::string> playerNames, unsigned int startingMoney)
    : m_playerInputStrategy(0) , m_currentPlayerIndex(0)
{
    for (unsigned short int i = 0; i < playerNames.size(); ++i) {
        m_players.emplace_back(playerNames[i], 0, startingMoney);
    }

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(m_players.begin(), m_players.end(), g);

    PRINT("Player order:");
    for(auto& pl : m_players){
        PRINT(pl.getName());
    }

    for(const auto& space : spacesConfig){
        m_unownedSpaces.push_back(std::make_unique<SpacesConfig>(space));
    }

    m_playerInputStrategy = new PlayerInputCli(); // Default to CLI input strategy

    while(!endGame){
        moveCurrentPlayer(askCurrentPlayerToRoll2D6());
        m_currentPlayerIndex = (m_currentPlayerIndex + 1) % m_players.size();
    }
    

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

    if((m_players[m_currentPlayerIndex].getPosition() + positionOffset) > (spacesConfig.size() - 1)){
        m_players[m_currentPlayerIndex].updateMoney(GO_PAY); 
    }

    try{

        m_players[m_currentPlayerIndex].updatePosition(positionOffset);

        processCurrentPlayerLanding(positionOffset);

    }
    catch(const std::exception& e){
        // Handle exceptions that may arise during landing processing (e.g., player bankruptcy)
        PRINT("Error moving current player " + std::string(e.what()));

    }

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

void Manager::processOwnableSpace(unsigned short int positionOffset, SpaceType type){
    Player* owner;
    if (isSpaceOwned(m_players[m_currentPlayerIndex].getPosition(), owner)) {

        if(owner == &m_players[m_currentPlayerIndex]){

            switch(type){
                case SpaceType::Property:
                    unsigned short int currentHouses = m_players[m_currentPlayerIndex].getPropertyHouses(m_players[m_currentPlayerIndex].getPosition());
                    if(currentHouses < HOUSE_HOTEL_CONVERSION 
                        && ownsAllColor(spacesConfig[m_players[m_currentPlayerIndex].getPosition()].color, m_players[m_currentPlayerIndex])){
                        playerBuildHouseHotel(m_players[m_currentPlayerIndex], m_players[m_currentPlayerIndex].getPosition(), currentHouses);
                    }
                    return;
                    break;
                case SpaceType::Railroad:
                    //can do nothing
                    return;
                    break;
                case SpaceType::Utility:
                    //can do nothing
                    return;
                    break;
                default:
                    throw std::runtime_error("Processing unownable space!!!");
            }

        }
        else{
            // Handle rent payment logic here
            unsigned short int rent = calculatePropertyRent(*owner, m_players[m_currentPlayerIndex].getPosition());
            moneyTransfer(m_players[m_currentPlayerIndex], *owner, rent);
        }
        
    } else {
        // Handle property purchase logic here
        playerBuySpace(m_players[m_currentPlayerIndex], m_players[m_currentPlayerIndex].getPosition());
    }
    return;
}

void Manager::processTaxSpace(){
    std::string taxName = spacesConfig[m_players[m_currentPlayerIndex].getPosition()].name;

    if(taxName == "Income Tax"){
        payUpOrBankrupt(INCOME_TAX);
    }
    else if(taxName == "Luxury Tax"){
        payUpOrBankrupt(LUXURY_TAX);        
    }
    else{
        throw std::runtime_error("Invalid tax!!!");
    }
}

void Manager::processCurrentPlayerLanding(unsigned short int positionOffset) {
    SpaceType spaceType = spacesConfig[m_players[m_currentPlayerIndex].getPosition()].type;

    try{
        switch(spaceType){
            case SpaceType::Property:
                processOwnableSpace(positionOffset, SpaceType::Property);
                break;
            case SpaceType::Railroad:
                processOwnableSpace(positionOffset, SpaceType::Railroad);
                break;
            case SpaceType::Utility:
                processOwnableSpace(positionOffset, SpaceType::Utility);
                break;
            case SpaceType::Tax:
                processTaxSpace();
                break;
            case SpaceType::CardSpace:
                processCardSpace((spacesConfig[m_players[m_currentPlayerIndex].getPosition()].name == "Chance" ));
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
    if(ownsAllColor(spacesConfig[position].color, owner)){
        return spacesConfig[position].getRent(rentType::NoHouse) * COLOR_MONOPOLY_BONUS; // Double rent for owning all properties in the color group
    }

    //basic rent with no houses and no color group monopoly
    return spacesConfig[position].getRent(rentType::NoHouse);
}

bool Manager::ownsAllColor(SpaceColor sc, Player& owner){
    
    if(m_colorGroups.count(sc) > 0){
        std::vector<unsigned short int> colorSpaceIndices =  m_colorGroups[sc];
        for(auto& cSpaceIndex : colorSpaceIndices){
            if(!owner.ownsSpace(cSpaceIndex)){
                return false;
            }
        }
    }
    else{
        throw std::runtime_error("color not found in m_colorGroups!!!");
    }

    return true;
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
            executeBankruptcyViaPlayer(from, to);
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

void Manager::executeBankruptcyViaPlayer(Player& bankruptPlayer, Player& creditorPlayer){

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

    bool buy = false;
    
    if(player.getMoney() < spacesConfig[spaceIndex].cost){
        PRINT("Not enough money to try buy!!!");
        return;
    }
    
    buy = m_playerInputStrategy->askToBuySpace(const_cast<SpacesConfig&>(spacesConfig[spaceIndex]));

    if(buy){
        for(auto& unOwned : m_unownedSpaces){
            if(unOwned->index == spaceIndex){
                std::unique_ptr<SpacesConfig> temp = std::move(unOwned);
                m_unownedSpaces.erase(std::remove(m_unownedSpaces.begin(), m_unownedSpaces.end(), unOwned), m_unownedSpaces.end());
                switch(temp->type){
                    case SpaceType::Property:
                        player.addProperty(std::move(temp));
                        break;
                    case SpaceType::Railroad:
                        player.addRailroad(std::move(temp));
                        break;
                    case SpaceType::Utility:
                        player.addUtility(std::move(temp));
                        break;
                    default:
                        throw std::runtime_error("Bought un-buyable space type!!!");
                }
            }
        }
    }

}


void Manager::playerBuildHouseHotel(Player& player, unsigned short int spaceIndex, unsigned short int currentHouses){

    unsigned short int maxBuyable = player.getMoney() / spacesConfig[spaceIndex].houseHotelCost;

    if(maxBuyable > HOUSE_HOTEL_CONVERSION){
        maxBuyable = HOUSE_HOTEL_CONVERSION;
    }

    unsigned short int housesToBuild = m_playerInputStrategy->askToBuildHouseHotel(const_cast<SpacesConfig&>(spacesConfig[spaceIndex]), currentHouses, maxBuyable);

    player.addHousesToProperty(spaceIndex, housesToBuild);

    player.updateMoney(-static_cast<int>(housesToBuild * spacesConfig[spaceIndex].houseHotelCost));

}

void Manager::executeBankruptcyViaBank(Player& bankruptPlayer){
    SellOptions currAssets;
    currAssets.formulateSellOptions(bankruptPlayer);

    for(auto& prop : currAssets.propertiesToSell){

        m_unownedSpaces.push_back(std::move(bankruptPlayer.removeProperty(prop.first)));

    }

    for(auto& rail : currAssets.railroadsToSell){
        m_unownedSpaces.push_back(std::move(bankruptPlayer.removeRailroad(rail)));
    }

    for(auto& util : currAssets.utilitiesToSell){
        m_unownedSpaces.push_back(std::move(bankruptPlayer.removeUtility(util)));
    }

    bankruptPlayer.updateMoney(-static_cast<int>(bankruptPlayer.getMoney()));

    m_players.erase(std::remove_if(m_players.begin(), m_players.end(), 
        [&bankruptPlayer](const Player& player) { return player.getName() == bankruptPlayer.getName(); }), m_players.end());
}

unsigned short int Manager::askCurrentPlayerToRoll2D6(){
    PRINT("Player to roll:" + m_players[m_currentPlayerIndex].getName());
    unsigned short int roll;
    m_playerInputStrategy->roll2d6Dice(roll);
    return roll;
}


void Manager::processCardSpace(bool isChance){

    std::random_device rd;
    std::mt19937 generator(rd());
    
    // Define an inclusive range [0, 15]
    std::uniform_int_distribution<int> distribution(0, 15);
    
    unsigned short int randNum = distribution(generator);



    if(isChance){
        switch(static_cast<ChanceCardType>(randNum)){
            case ChanceCardType::Advance_to_GO:
                advanceToGo();
                break;
            case ChanceCardType::Advance_to_Boardwalk:
                for(auto& sc : spacesConfig){
                    if(sc.name == "Boardwalk"){
                        advanceToSpace(sc);
                        break;
                    }
                }
                
                break;
            case ChanceCardType::Advance_to_illinois_Avenue:
                for(auto& sc : spacesConfig){
                    if(sc.name == "Illinois Avenue"){
                        advanceToSpace(sc);
                        break;
                    }
                }
                break;
            case ChanceCardType::Advance_to_St_Charles_Place:
                for(auto& sc : spacesConfig){
                    if(sc.name == "St. Charles Place"){
                        advanceToSpace(sc);
                        break;
                    }
                }
                break;
            case ChanceCardType::Advance_to_nearest_Railroad_1:
                advanceToSpace(getClosestSpaceType(SpaceType::Railroad));
                break;
            case ChanceCardType::Advance_to_nearest_Railroad_2:
                advanceToSpace(getClosestSpaceType(SpaceType::Railroad));
                break;
            case ChanceCardType::Advance_to_nearest_Utility:
                advanceToSpace(getClosestSpaceType(SpaceType::Utility));
                break;
            case ChanceCardType::Take_a_trip_to_Reading_Railroad:
                for(auto& sc : spacesConfig){
                    if(sc.name == "Reading Railroad"){
                        advanceToSpace(sc);
                        break;
                    }
                }
                break;
            case ChanceCardType::Go_Back_3_Spaces:
                goBack3Spaces();
                break;
            case ChanceCardType::Go_Directly_to_Jail:
                for(auto& sc : spacesConfig){
                    if(sc.name == "Jail"){
                        advanceToSpace(sc);
                        break;
                    }
                }
                break;
            case ChanceCardType::Get_Out_of_Jail_Free:
                m_players[m_currentPlayerIndex].incrementGetoutofJail();
                break;
            case ChanceCardType::Make_General_Repairs:
                makeGeneralRepairs();
                break;
            case ChanceCardType::Speeding_Fine:
                speedingFine();
                break;
            case ChanceCardType::You_have_been_elected_Chairman_of_the_Board:
                chairmanOfBoard();
                break;
            case ChanceCardType::Bank_pays_you_dividend:
                m_players[m_currentPlayerIndex].updateMoney(BANK_DIVIDENT_AMOUNT);
                break;
            case ChanceCardType::Your_building_loan_matures:
                m_players[m_currentPlayerIndex].updateMoney(BUILDING_LOAN_MATURES_AMOUNT);
                break;
            default:
                throw std::runtime_error("Unaccounted for ChanceCardType!!!");
                break;
        }
    }
    else{

        switch(static_cast<CommunityChestCardType>(randNum)){
            case CommunityChestCardType::Advance_to_GO:
                advanceToGo();
                break;
            case CommunityChestCardType::Bank_error_in_your_favor:
                break;
            case CommunityChestCardType::Doctor_fees:
                break;
            case CommunityChestCardType::From_sale_of_stock:
                break;
            case CommunityChestCardType::Get_Out_of_Jail_Free:
                break;
            case CommunityChestCardType::Go_Directly_to_Jail:
                break;
            case CommunityChestCardType::Grand_Opera_Night:
                break;
            case CommunityChestCardType::Holiday:
                break;
            case CommunityChestCardType::Income_tax_refund:
                break;
            case CommunityChestCardType::It_is_your_birthday:
                break;
            case CommunityChestCardType::Life_insurance_matures:
                break;
            case CommunityChestCardType::Pay_hospital_fees:
                break;
            case CommunityChestCardType::Pay_school_fees:
                break;
            case CommunityChestCardType::Receive_consultancy:
                break;
            case CommunityChestCardType::Assessed_for_street_repairs:
                break;
            case CommunityChestCardType::You_have_won_second_prize_in_a_beauty_contest:
                break;
            case CommunityChestCardType::You_inherit_money:
                break;
            default:
                throw std::runtime_error("Unaccounted for CommunityChestCardType!!!");
                break;
        }

    }
}

void Manager::advanceToGo(){

    unsigned short int offset = spacesConfig.size() - m_players[m_currentPlayerIndex].getPosition(); 
    m_players[m_currentPlayerIndex].updatePosition(offset);
    m_players[m_currentPlayerIndex].updateMoney(GO_PAY);
}

void Manager::advanceToSpace(const SpacesConfig& sConfig){

    unsigned short int spaceIndex;

    for(auto& sc : spacesConfig){
        if(sc == sConfig){
            spaceIndex = sc.index;
        }
    }

    unsigned short int offset;

    if(spaceIndex >= m_players[m_currentPlayerIndex].getPosition()){
        offset = spaceIndex - m_players[m_currentPlayerIndex].getPosition();
    }
    else{
        offset = (spacesConfig.size() - m_players[m_currentPlayerIndex].getPosition()) + spaceIndex;
    }

    if(offset >= (spacesConfig.size() - m_players[m_currentPlayerIndex].getPosition())){
        m_players[m_currentPlayerIndex].updateMoney(GO_PAY);
    }

    m_players[m_currentPlayerIndex].updatePosition(offset);

    processCurrentPlayerLanding(offset);
}

const SpacesConfig& Manager::getClosestSpaceType(SpaceType sType){

    unsigned short int currPosition = m_players[m_currentPlayerIndex].getPosition();
    unsigned short int maxPosition = spacesConfig.size();

    unsigned short int posIterator = 0;

    while(posIterator < maxPosition){

        unsigned short int pos = currPosition + posIterator + 1;
        pos %= maxPosition;
        if(spacesConfig[currPosition + posIterator + 1].type == sType){
            return spacesConfig[currPosition + posIterator + 1];
        }

        posIterator++;
    }
    throw std::runtime_error("Couldnt find closest railroad!!!");
    
}


void Manager::goBack3Spaces(){

    m_players[m_currentPlayerIndex].updatePosition(spacesConfig.size() - 3);
    processCurrentPlayerLanding(1);

}


void Manager::makeGeneralRepairs(){
    SellOptions props;

    props.formulateSellOptions(m_players[m_currentPlayerIndex]);

    unsigned int amount = 0;

    for(auto& pr : props.propertiesToSell){
        amount += PER_HOUSE_REPAIR_COST * pr.second;
    }

    payUpOrBankrupt(amount);

}


void Manager::payUpOrBankrupt(unsigned int amount){
    if(m_players[m_currentPlayerIndex].getMoney() >= amount){
        m_players[m_currentPlayerIndex].updateMoney(-1 * amount);
    }
    else{
        executeBankruptcyViaBank(m_players[m_currentPlayerIndex]);
    }
}

void Manager::speedingFine(){
    payUpOrBankrupt(SPEEDING_FINE);
}


void Manager::chairmanOfBoard(){

    std::string playerName = m_players[m_currentPlayerIndex].getName();

    for(auto& pl : m_players){
        if(pl == m_players[m_currentPlayerIndex]){
            continue;
        }
        moneyTransfer(m_players[m_currentPlayerIndex], pl, CHAIRMAN_OF_BOARD_AMOUNT);
        if(!doesPlayerExist(playerName)){
            break;
        }
    }
}

bool Manager::doesPlayerExist(const std::string& plName){
    for(auto& p : m_players){
        if(p.getName() == plName){
            return true;
        }
    }

    return false;
}
