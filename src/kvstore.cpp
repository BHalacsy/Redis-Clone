#include <kvstore.hpp>
#include <RESPtype.hpp>
#include <iostream>

KVStore::KVStore()
{
    //Maybe add persistence to rebuild in mem db, using fstream on a file.

}

KVStore::~KVStore()
{
    //Save everything to the disk?
}

std::optional<std::string> KVStore::get(const std::string& k)
{
    auto found = dict.find(k);
    if (found != dict.end()) return found->second;
    return std::nullopt;
}

bool KVStore::set(const std::string& k, const std::string& v)
{
    try
    {
        if (dict.contains(k)) dict[k] = v;
        else dict.insert({k,v});
        return true;
    } catch (std::exception& e) {
        std::cerr << "Fail in set: " << e.what() << std::endl;
        return false;
    }
}

int KVStore::del(const std::vector<std::string>& args)
{
    try
    {
        int deleted = 0;
        for (const auto& k : args)
        {

            if (dict.contains(k))
            {
                dict.erase(k);
                deleted++;
            }
        }
        return deleted;

    } catch (std::exception& e) {
        std::cerr << "Fail in del: " << e.what() << std::endl;
        return 0;
    }
}

int  KVStore::exists(const std::vector<std::string>& args)
{
    try
    {
        int exist = 0;
        for (const auto& k : args)
        {
            if (dict.contains(k))
            {
                exist++;
            }
        }
        return exist;

    } catch (std::exception& e) {
        std::cerr << "Fail in del: " << e.what() << std::endl;
        return 0;
    }
}


