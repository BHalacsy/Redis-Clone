#include "kvstore.hpp"

KVStore::KVStore()
{
    //Maybe add persistence to rebuild in mem db, using fstream on a file.
}

KVStore::~KVStore()
{
    //Save everything to the disk?
}

values KVStore::get(std::string k)
{
    //TODO implement get
    return 0;
}

void KVStore::set(std::string k, values v)
{
    //TODO implement set
}


