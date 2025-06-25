#include "includes/cli.h"

#include <iostream>
#include <thread>
#include <sstream>
#include <chrono>


int main() {
    #ifdef NDEBUG
    std::cout << "Build-Typ: Release" << std::endl;
    #else
    std::cout << "Build-Typ: Debug" << std::endl;
    #endif
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    CommandLineInterface cli;
    cli.menu();
    return 0;
}