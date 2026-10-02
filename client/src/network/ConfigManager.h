#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include<iostream>
#include<fstream>
#include<unordered_map>
using namespace std;
class ConfigManager
{
private:
    static string SERVER_CONFIG_PATH;
public:
    ConfigManager();
    static unordered_map<string, string> readServerConfig();
};

#endif // CONFIGMANAGER_H
