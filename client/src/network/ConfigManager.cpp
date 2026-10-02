#include "ConfigManager.h"

ConfigManager::ConfigManager()
{
}
unordered_map<string, string> ConfigManager::readServerConfig(){
    ifstream in(SERVER_CONFIG_PATH, ios::in);
    unordered_map<string, string> configMap;
    if(in.is_open()){
        string line;
        while(getline(in, line)){
            int index = line.find('=');
            configMap[line.substr(0, index)] = line.substr(index+1);
        }
    }
    return configMap;
}
string ConfigManager::SERVER_CONFIG_PATH = "../../server.conf";