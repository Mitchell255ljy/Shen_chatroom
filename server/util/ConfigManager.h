#pragma once
#include<iostream>
#include<fstream>
#include<unordered_map>
#include<string>

class ConfigManager
{
private:
	static std::string SERVER_CONFIG_PATH;
	static std::string MYSQL_CONFIG_PATH;

public:
	ConfigManager();
	static std::unordered_map<std::string, std::string> ReadServerConfig();
	static std::unordered_map<std::string, std::string> ReadMysqlConfig();
};