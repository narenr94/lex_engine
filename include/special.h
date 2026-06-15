#pragma once

#include <string>

enum class SpecialSpaceType {
    Go,
    Jail,
    FreeParking,
    GoToJail
};


std::string specialSpaceTypeToString(SpecialSpaceType sst);

SpecialSpaceType stringToSpecialSpaceType(const std::string& str);