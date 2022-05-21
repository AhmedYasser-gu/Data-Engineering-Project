#include "Printer.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <math.h> 

// Method to write to a csv file.
void Printer::writeToFile(std::string fileName, double data){

    if(isnan(data)){
        data = 0;
    }

    std::ofstream file(fileName, std::ios_base::app);
    if (file.is_open())
    {   
        file << data << ";" << std::endl;
        file.close();
    } else {
        std::cout << "Unable to open csv file\n";
    }
}
//Create an overloaded constructor for writeToFile so we can write the first row of the csv file
void Printer::writeToFile(std::string fileName, std::string data){

    if(data == ""){
        data = "";
    }

    std::ofstream file(fileName, std::ios_base::app);
    if (file.is_open())
    {   
        file << data << ";" << std::endl;
        file.close();
    } else {
        std::cout << "Unable to open csv file\n";
    }
}

void Printer::clearFile(std::string fileName){
    std::ofstream file(fileName, std::ios_base::out);
    if (file.is_open()) {
        file << "" ;
        file.close();
    } else {
        std::cout << "Unable to open and clear csv file\n";
    }
}