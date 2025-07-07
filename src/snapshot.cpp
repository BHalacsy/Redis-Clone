#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <filesystem>
#include <iostream>

#include "snapshot.hpp"
#include "util.hpp"

Snapshot::Snapshot(const std::string& fileName) : filePath((std::filesystem::path("data") / fileName).string())
{

}

void Snapshot::load(tbb::concurrent_hash_map<std::string, RESPValue>& dict) const
{
    try
    {
        std::unordered_map<std::string, RESPValue> readMap;
        std::ifstream readFromFile(filePath, std::ios::binary);
        if (!readFromFile) return;

        boost::archive::binary_iarchive binFile(readFromFile);
        binFile >> readMap;

        dict = convertToConcurrentMap(readMap);

    }
    catch (std::exception& e) {
        std::cerr << "Fail in snapshot load(): " << e.what() << std::endl;
    }
}

void Snapshot::save(const tbb::concurrent_hash_map<std::string, RESPValue>& dict) const
{
    try
    {
        std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
        std::ofstream writeToFile(filePath, std::ios::binary | std::ios::trunc);

        std::unordered_map<std::string, RESPValue> writeMap = convertToUnorderedMap(dict);

        boost::archive::binary_oarchive binFile(writeToFile);
        binFile << writeMap;
    }
    catch (std::exception& e) {
        std::cerr << "Fail in snapshot save(): " << e.what() << std::endl;
    }
}

void Snapshot::clear() const
{
    std::ofstream clearDisk(filePath, std::ios::binary | std::ios::trunc);
}
