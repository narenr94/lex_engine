#pragma once

#include <string>

class PlayerInputStrategy{

    public:
        virtual ~PlayerInputStrategy() = default;

        virtual void roll2d6Dice(unsigned short int& roll) = 0;

        virtual void askToSellForMoney(unsigned int amountNeeded, const SellOptions& options) = 0; 
        
};


class PlayerInputCli : public PlayerInputStrategy {
   
    public:
        PlayerInputCli();
        ~PlayerInputCli();


        void roll2d6Dice(unsigned short int& roll);
        void askToSellForMoney(unsigned int amountNeeded, const SellOptions& options);
};