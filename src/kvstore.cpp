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

std::string KVStore::get(const std::string& k)
{
    auto found = dict.find(k);
    if (found != dict.end()) return found->second;
    return {0};
}

void KVStore::set(const std::string& k, const std::string& v)
{
    if (this->exists(k))
    {
        dict[k] = v;
    }
    else
    {
        dict.insert({k,v});
    }
}

void KVStore::del(const std::string& k)
{
    dict.erase(k);
}

bool KVStore::exists(const std::string& k)
{
    return dict.find(k) != dict.end();
}


