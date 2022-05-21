#include <string>

class Printer {
    public:
        void writeToFile(std::string fileName, double data);
        void writeToFile(std::string fileName, std::string data);
        void clearFile(std::string fileName);
};