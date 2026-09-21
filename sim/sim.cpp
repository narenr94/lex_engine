#include "manager.h"
#include <iostream>

#define PRINT_SIM(text) do { std::cout << text << std::endl; } while (0)

int main() {

    PRINT_SIM("Simulator started");

    bool running = true;


    Manager* manager = new Manager({"Player1", "Player2", "Player3", "Player4"}, 1500);



    PRINT_SIM("Simulator exited");

    return 0;
}