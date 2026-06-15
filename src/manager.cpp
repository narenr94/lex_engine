#include "manager.h"
#include "player.h"
#include "spaces.h"
#include "lexEnginePlayerInput.h"
#include "cards.h"
#include "special.h"

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
        if(isSpaceOwnable(space)){            
            m_unownedSpaces.push_back(std::make_unique<SpacesConfig>(space));
        }
        
    }

    m_playerInputStrategy = new PlayerInputCli(); // Default to CLI input strategy

       
    gameLoop();

}


Manager::~Manager() {
    if(m_playerInputStrategy){
        delete m_playerInputStrategy;
        m_playerInputStrategy = nullptr;
    }
}

void Manager::gameLoop(){

    bool rolledDoubles = false;

    unsigned short int doublesRolledCount = 0;

    while(!endGame){

        try{

            //if not attempting rolling doubles in jail
            unsigned short int tempCount = 0; //dummy
            if(isPlayerInJail(m_players[m_currentPlayerIndex], tempCount)){                

                rolledDoubles = rollAndMoveCurrentPlayer();

                if(!rolledDoubles){
                    m_currentPlayerIndex = (m_currentPlayerIndex + 1) % m_players.size();
                    doublesRolledCount = 0;
                }
                else
                {
                    doublesRolledCount++;
                    if(doublesRolledCount == 3){
                        // Send player to jail
                        advanceToSpace(getSpaceConfigByName("Jail"));
                        m_currentPlayerIndex = (m_currentPlayerIndex + 1) % m_players.size();
                        doublesRolledCount = 0;
                    }
                }

            }
            else{
                Jail();
                m_currentPlayerIndex = (m_currentPlayerIndex + 1) % m_players.size();
            }

        }
        catch(const std::exception& e){
            // Handle exceptions that may arise during the game loop (e.g., player bankruptcy)
            PRINT("Error in game loop: " + std::string(e.what()));
            endGame = true; // End the game on critical errors
        }
        
    }
}

bool Manager::rollAndMoveCurrentPlayer(){
    unsigned short int roll = 0;

    bool rolledDoubles = askCurrentPlayerToRoll2D6(roll);

    try{
        moveCurrentPlayer(roll);
    }
    catch(const std::exception& e){
        PRINT("Error in moving player: " + std::string(e.what()));
    }    

    return rolledDoubles;
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
                case SpaceType::Property:{
                        unsigned short int currentHouses = m_players[m_currentPlayerIndex].getPropertyHouses(m_players[m_currentPlayerIndex].getPosition());
                        if(currentHouses < HOUSE_HOTEL_CONVERSION 
                            && ownsAllColor(spacesConfig[m_players[m_currentPlayerIndex].getPosition()].color, m_players[m_currentPlayerIndex])){
                            playerBuildHouseHotel(m_players[m_currentPlayerIndex], m_players[m_currentPlayerIndex].getPosition(), currentHouses);
                        }
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
                processSpecialSpace();
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

void Manager::moneyTransferBank(Player& from, unsigned int amount) {
    
    if (from.getMoney() > amount) {
        from.updateMoney(-amount);
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
            } else {
                throw std::runtime_error("Mistake to sellable networth calculation!!!");
            }

        }
        else{
            //declare bankruptcy
            executeBankruptcyViaBank(from);
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

    removeDefeatedPlayerFromGame(bankruptPlayer);
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

    removeDefeatedPlayerFromGame(bankruptPlayer);
}

bool Manager::askCurrentPlayerToRoll2D6(unsigned short int& roll){
    PRINT("Player to roll:" + m_players[m_currentPlayerIndex].getName());
    return m_playerInputStrategy->roll2d6Dice(roll);
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
                advanceToSpace(getSpaceConfigByName("Boardwalk"));
                break;
            case ChanceCardType::Advance_to_illinois_Avenue:
                advanceToSpace(getSpaceConfigByName("Illinois Avenue"));
                break;
            case ChanceCardType::Advance_to_St_Charles_Place:
                advanceToSpace(getSpaceConfigByName("St. Charles Place"));
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
                advanceToSpace(getSpaceConfigByName("Reading Railroad"));
                break;
            case ChanceCardType::Go_Back_3_Spaces:
                goBack3Spaces();
                break;
            case ChanceCardType::Go_Directly_to_Jail:
                advanceToSpace(getSpaceConfigByName("Jail"));
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
                throw std::runtime_error("Unaccounted for ChanceCardType: " + std::to_string(randNum) + "!!!");
                break;
        }
    }
    else{

        switch(static_cast<CommunityChestCardType>(randNum)){
            case CommunityChestCardType::Advance_to_GO:
                advanceToGo();
                break;
            case CommunityChestCardType::Bank_error_in_your_favor:
                m_players[m_currentPlayerIndex].updateMoney(BANK_ERROR_IN_YOUR_FAVOR_AMOUNT);
                break;
            case CommunityChestCardType::Doctor_fees:
                moneyTransferBank(m_players[m_currentPlayerIndex], DOCTOR_FEES_AMOUNT);
                break;
            case CommunityChestCardType::From_sale_of_stock:
                m_players[m_currentPlayerIndex].updateMoney(SALE_OF_STOCK_AMOUNT);
                break;
            case CommunityChestCardType::Get_Out_of_Jail_Free:
                m_players[m_currentPlayerIndex].incrementGetoutofJail();
                break;
            case CommunityChestCardType::Go_Directly_to_Jail:
                advanceToSpace(getSpaceConfigByName("Jail"));
                break;
            case CommunityChestCardType::Grand_Opera_Night:
                grandOperaNight();
                break;
            case CommunityChestCardType::Holiday:
                m_players[m_currentPlayerIndex].updateMoney(HOLIDAY_AMOUNT);
                break;
            case CommunityChestCardType::Income_tax_refund:
                m_players[m_currentPlayerIndex].updateMoney(INCOME_TAX_REFUND_AMOUNT);
                break;
            case CommunityChestCardType::It_is_your_birthday:
                birthday();
                break;
            case CommunityChestCardType::Life_insurance_matures:
                m_players[m_currentPlayerIndex].updateMoney(LIFE_INSURANCE_MATURES_AMOUNT);
                break;
            case CommunityChestCardType::Pay_hospital_fees:
                moneyTransferBank(m_players[m_currentPlayerIndex], PAY_HOSPITAL_FEES_AMOUNT);
                break;
            case CommunityChestCardType::Pay_school_fees:
                moneyTransferBank(m_players[m_currentPlayerIndex], PAY_SCHOOL_FEES_AMOUNT);
                break;
            case CommunityChestCardType::Receive_consultancy:
                m_players[m_currentPlayerIndex].updateMoney(RECEIVE_CONSULTANCY_AMOUNT);
                break;
            case CommunityChestCardType::Assessed_for_street_repairs:
                AssessedStreetRepairs();
                break;
            case CommunityChestCardType::You_have_won_second_prize_in_a_beauty_contest:
                m_players[m_currentPlayerIndex].updateMoney(BEAUTY_CONTEST_PRIZE_AMOUNT);
                break;
            case CommunityChestCardType::You_inherit_money:
                m_players[m_currentPlayerIndex].updateMoney(INHERIT_MONEY_AMOUNT);
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
        if(!(sConfig.name == "Jail")){
            m_players[m_currentPlayerIndex].updateMoney(GO_PAY);
        }        
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

const SpacesConfig& Manager::getSpaceConfigByName(const std::string& spaceName){

    for(auto& sc : spacesConfig){
        if(sc.name == spaceName){
            return sc;
        }
    }

    throw std::runtime_error("Space name not found in spacesConfig!!!");
    
}

void Manager::grandOperaNight(){

    for(auto& pl : m_players){
        if(pl == m_players[m_currentPlayerIndex]){
            continue;
        }
        moneyTransfer(pl, m_players[m_currentPlayerIndex], GRAND_OPERA_NIGHT_AMOUNT);
    }
}

void Manager::birthday(){

    for(auto& pl : m_players){
        if(pl == m_players[m_currentPlayerIndex]){
            continue;
        }
        moneyTransfer(m_players[m_currentPlayerIndex], pl, BIRTHDAY_AMOUNT);
    }
}

void Manager::AssessedStreetRepairs(){

    unsigned int amount = 0;

    SellOptions props;

    props.formulateSellOptions(m_players[m_currentPlayerIndex]);

    for(auto& pr : props.propertiesToSell){

        if(pr.second > 0){
            if(pr.second < HOUSE_HOTEL_CONVERSION){
                amount += pr.second * ASSESSED_STREET_REPAIRS_HOUSE_COST;
            }
            else{
                amount += ASSESSED_STREET_REPAIRS_HOTEL_COST;
            }
        }
    }

    moneyTransferBank(m_players[m_currentPlayerIndex], amount);

}

void Manager::processSpecialSpace(){

    SpecialSpaceType sType = stringToSpecialSpaceType(spacesConfig[m_players[m_currentPlayerIndex].getPosition()].name);

    switch(sType){
        case SpecialSpaceType::Go:
            //do nothing
            //handled during movement calculation
            break;
        case SpecialSpaceType::Jail:
            Jail();
            break;
        case SpecialSpaceType::FreeParking:
            //do nothing
            break;
        case SpecialSpaceType::GoToJail:
            advanceToSpace(getSpaceConfigByName("Jail"));
            break;
        default:
            throw std::runtime_error("Unaccounted for SpecialSpaceType" + std::to_string(static_cast<int>(sType)) + "!!!");
            break;

    }

}

void Manager::Jail(){

    unsigned short int count = 0;
    
    //trying to roll doubles to get out of jail
    if(isPlayerInJail(m_players[m_currentPlayerIndex], count)){

        unsigned short int roll = 0;
        bool rolledDouble = askCurrentPlayerToRoll2D6(roll);

        if(!rolledDouble){
            count += 1;
            if(count >= 3){
                
                removePlayerFromJailList(m_players[m_currentPlayerIndex]);
                std::string name = m_players[m_currentPlayerIndex].getName();
                if(m_players[m_currentPlayerIndex].canGetoutofJail()){
                    m_players[m_currentPlayerIndex].decrementGetoutofJail();
                }
                else{
                    moneyTransferBank(m_players[m_currentPlayerIndex], JAIL_RELEASE_AMOUNT);
                }             
            }
            else{

                //update failed double roll attempts count
                for(auto& jl : m_jail){
                    if(jl.first == m_players[m_currentPlayerIndex].getName()){
                        jl.second = count;
                    }
                }
            }
        }
        else{
            moveCurrentPlayer(roll);
        }        

    }
    else{ //new jailee
        unsigned short int choice = m_playerInputStrategy->JailOptions(m_players[m_currentPlayerIndex].canGetoutofJail());

        switch(choice){
            case 0: // Pay to get out of jail
                moneyTransferBank(m_players[m_currentPlayerIndex], JAIL_RELEASE_AMOUNT);
                break;
            case 1: // Attempt to roll doubles
                m_jail.push_back({m_players[m_currentPlayerIndex].getName(), 0}); // Initialize jail turn count
                break;
            case 2: // Use get out of jail card
                m_players[m_currentPlayerIndex].decrementGetoutofJail();
                break;

        }
    }

}


bool Manager::isPlayerInJail(const Player& player, unsigned short int& count){
    for(auto& jl : m_jail){
        if(jl.first == player.getName()){
            count = jl.second;
            return true;
        }
    }

    return false;
}

void Manager::removePlayerFromJailList(const Player& player){

    unsigned int i = 0;
    for(auto& jl : m_jail){
        if(jl.first == player.getName()){
            m_jail.erase(m_jail.begin() + i);
            return;
        }
        i++;
    }

    throw std::runtime_error("Player: " + player.getName() + " not found in jail to release!!!");
    return;
}

void Manager::removeDefeatedPlayerFromGame(const Player& player){

    unsigned int i = 0;

    for(auto& pl : m_players){
        if(pl == player){
            m_players.erase(m_players.begin() + i);
            return;
        }
        i++;
    }

}

bool Manager::isSpaceOwnable(const SpacesConfig& spaceConfig){

    return ((spaceConfig.type == SpaceType::Property) ||                
            (spaceConfig.type == SpaceType::Utility) ||
            (spaceConfig.type == SpaceType::Railroad));

}