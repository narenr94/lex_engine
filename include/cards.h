#pragma once

#define CHANCE_CARDS_COUNT 16
#define COMMUNITY_CHEST_CARDS_COUNT 16

enum class ChanceCardType{
    Advance_to_GO,
    Advance_to_Boardwalk,
    Advance_to_illinois_Avenue,
    Advance_to_St_Charles_Place,
    Advance_to_nearest_Railroad_1,
    Advance_to_nearest_Railroad_2,
    Advance_to_nearest_Utility,
    Take_a_trip_to_Reading_Railroad,
    Go_Back_3_Spaces,
    Go_Directly_to_Jail,
    Get_Out_of_Jail_Free,
    Make_General_Repairs,
    Speeding_Fine,
    You_have_been_elected_Chairman_of_the_Board,
    Bank_pays_you_dividend,
    Your_building_loan_matures
};

enum class CommunityChestCardType{
    Advance_to_GO,
    Bank_error_in_your_favor,
    Doctor_fees,
    From_sale_of_stock,
    Get_Out_of_Jail_Free,
    Go_Directly_to_Jail,
    Grand_Opera_Night,
    Holiday,
    Income_tax_refund,
    It_is_your_birthday,
    Life_insurance_matures,
    Pay_hospital_fees,
    Pay_school_fees,
    Receive_consultancy,
    Assessed_for_street_repairs,
    You_have_won_second_prize_in_a_beauty_contest,
    You_inherit_money
};