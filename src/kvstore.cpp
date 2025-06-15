#include <kvstore.hpp>
#include <RESPtype.hpp>

KVStore::KVStore()
{
    //Maybe add persistence to rebuild in mem db, using fstream on a file.
}

KVStore::~KVStore()
{
    //Save everything to the disk?
}

RESPValue KVStore::get(std::string k)
{
    //TODO implement get
    return {0};
}

void KVStore::set(std::string k, RESPValue v)
{
    //TODO implement set
}


