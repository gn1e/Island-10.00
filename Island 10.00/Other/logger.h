#pragma once
#include <iostream>
#include <fstream>
#include <format>
#include <cstdio>

inline bool debug = true;

FILE* stream = nullptr;

void stopfnlogs() {
    freopen_s(&stream, "NUL", "w", stdout);
    freopen_s(&stream, "NUL", "w", stderr);
}

void restorelogs() {
    freopen_s(&stream, "CON", "w", stdout);
    freopen_s(&stream, "CON", "w", stderr); 
}

// Kind of trying to replicate backend level logging, and this is my attempt.

void Log(const std::string& message, const char* level = "INFO") {
    restorelogs(); 

    const char* color =
        (std::string(level) == "INFO") ? "\033[32m" :  
        (std::string(level) == "ERROR") ? "\033[31m" : 
        (std::string(level) == "WARNING") ? "\033[33m" :
        (std::string(level) == "DEBUG") ? "\033[36m" :
        "\033[0m"; 

    std::cout << std::format("{}[{}]\033[0m {}\n", color, level, message);

    stopfnlogs(); 
}

void LogError(const std::string& message) { Log(message, "ERROR"); }
void LogWarning(const std::string& message) { Log(message, "WARNING"); }
void LogDebug(const std::string& message) {
    if (debug) {
        Log(message, "DEBUG");
    }
}
