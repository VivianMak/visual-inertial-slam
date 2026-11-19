#include <exception>
#include <iostream>
#include <print>

#include "slam.hpp"



int main(){
    // dont need arguments for now

    try{
        SLAM slam;
        slam.init();
        slam.run();
    } catch (const std::exception& e) {
        std::cout << "Caught error: " << e.what() << '\n';
        return 1;
    } 

    std::println("Finished executing visual-interial SLAM.");

    reutrn 0;

}