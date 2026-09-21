#include "logger.h"

#include <fstream>
#include <time.h>

#define LOGGER_FILE "LE_Log.txt"

LE_Logger* LE_Logger::instance = nullptr;

std::ofstream logFile;

LE_Logger::LE_Logger() {

    // Initialize the log file
    logFile.open(LOGGER_FILE, std::ios::app);
    if (logFile.is_open()) {
        logFile << "Logger initialized." << std::endl;
        logFile.close();
    }
    logFile.close();

}

LE_Logger* LE_Logger::getInstance() {
    if (instance == nullptr) {
        instance = new LE_Logger();
    }
    return instance;
}

void LE_Logger::log(const std::string& message) {
    // Implement logging functionality here
    logFile.open(LOGGER_FILE, std::ios::app);
    if (logFile.is_open()) {

        // Add timestamp to the log message
        time_t now = time(0);
        char* dt = ctime(&now);
        logFile << "[" << dt << "] ";
        logFile << message << std::endl;
        logFile.close();
    }
}

LE_Logger::~LE_Logger() {
    //close the logger if needed
    instance = nullptr;
    if (logFile.is_open()) {
        logFile.close();
    }

}