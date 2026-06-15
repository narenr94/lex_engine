#include "special.h"

#include <stdexcept>

std::string specialSpaceTypeToString(SpecialSpaceType sst) {

    switch(sst){
        case SpecialSpaceType::Go:
            return "Go";
            break;
        case SpecialSpaceType::Jail:
            return "Jail";
            break;
        case SpecialSpaceType::FreeParking:
            return "Free Parking";
            break;
        case SpecialSpaceType::GoToJail:
            return "Go To Jail";
            break;
        default:
            throw std::runtime_error("Invalid special space type" + std::to_string(static_cast<int>(sst)) + "!!!");
    }

}


SpecialSpaceType stringToSpecialSpaceType(const std::string& str){

    if(str == "Go"){
        return SpecialSpaceType::Go;
    }
    else if(str == "Jail"){
        return SpecialSpaceType::Jail;
    }
    else if(str == "Free Parking"){
        return SpecialSpaceType::FreeParking;
    }
    else if(str == "Go To Jail"){
        return SpecialSpaceType::GoToJail;
    }
    else{
        throw std::runtime_error("Invalid special space type string: " + str + "!!!");
    }

    return SpecialSpaceType::Go; 


}