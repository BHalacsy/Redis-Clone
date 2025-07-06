#pragma once

#include <string>
#include <unordered_map>
#include <tbb/concurrent_hash_map.h>

#include "RESPtype.hpp"

class Snapshot{
public:
	explicit Snapshot(const std::string& fileName);
    void save(const tbb::concurrent_hash_map<std::string, RESPValue>& dict) const;
	void load(tbb::concurrent_hash_map<std::string, RESPValue>& dict) const;
	void clear() const;

private:
	std::string filePath;
};


