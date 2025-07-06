#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <filesystem>
#include <iostream>

#include "snapshot.hpp"

Snapshot::Snapshot(const std::string& fileName) : filePath((std::filesystem::path("data") / fileName).string())
{

}

void Snapshot::load(std::unordered_map<std::string, RESPValue>& dict) const
{
    try
    {
        std::ifstream readFromFile(filePath, std::ios::binary);
        if (!readFromFile) return;
        boost::archive::binary_iarchive binFile(readFromFile);
        binFile >> dict;
    }
    catch (std::exception& e) {
        std::cerr << "Fail in snapshot load(): " << e.what() << std::endl;
    }
}

void Snapshot::save(const std::unordered_map<std::string, RESPValue>& dict) const
{
    try
    {
        std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
        std::ofstream writeToFile(filePath, std::ios::binary | std::ios::trunc);
        std::cout << "Saving to file: " << filePath << std::endl; //remove when done
        boost::archive::binary_oarchive binFile(writeToFile);
        binFile << dict;
    }
    catch (std::exception& e) {
        std::cerr << "Fail in snapshot save(): " << e.what() << std::endl;
    }
}

void Snapshot::clear() const
{
    std::ofstream clearDisk(filePath, std::ios::binary | std::ios::trunc);
}
