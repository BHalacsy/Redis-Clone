#pragma once

#include <string>
#include <unordered_map>
#include <tbb/concurrent_hash_map.h>

#include "respvalue.hpp"

class Snapshot{
public:
	explicit Snapshot(const std::string& fileName);
    void save(const tbb::concurrent_hash_map<std::string, RESPValue>& dict) const; //serialize and save to disk
	void load(tbb::concurrent_hash_map<std::string, RESPValue>& dict) const; //deserialize and load in memory

private:
	std::string filePath; //More so file name
};


