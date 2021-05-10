// Copyright 2021 Feiler

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <string>
#include <vector>

class IpAddressDeserializer {
public:
    explicit IpAddressDeserializer(const std::string& pathToInitialIpAddressFile,
        const std::string& searchedKey);
    void execute();
    std::vector<std::string> getIpAddresses();

private:
    std::string _pathToInitialIpAddressFile;
    std::string _searchedKey;
    std::vector<std::string> _ipAddresses;
    YAML::Node _node;

    void catchAndPrintErrorsWhenGettingNodeAtSearchedKey();
    void pushIpAddressesIntoVector();
};
