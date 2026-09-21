#pragma once

#include <string>

class LE_Logger{

    static LE_Logger* instance;

    LE_Logger();

    public:

    static LE_Logger* getInstance();

    void log(const std::string& message);

    ~LE_Logger();

};