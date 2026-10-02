#include"ConfigManager.h"
ConfigManager::ConfigManager()
{

}

std::unordered_map<std::string, std::string> ConfigManager::ReadServerConfig()
{
	std::ifstream in(SERVER_CONFIG_PATH, std::ios::in);
	std::unordered_map<std::string, std::string> configmap;
	if (in.is_open())
	{
		std::string line;
		while (std::getline(in, line))
		{
			int index = line.find('=');
			configmap[line.substr(0, index)] = line.substr(index + 1);
		}
	}
	return configmap;
}

std::unordered_map<std::string, std::string> ConfigManager::ReadMysqlConfig()
{
	std::ifstream in(MYSQL_CONFIG_PATH, std::ios::in);
	std::unordered_map<std::string, std::string> configmap;
	if (in.is_open())
	{
		std::string line;
		while (std::getline(in, line))
		{
			int index = line.find('=');
			configmap[line.substr(0, index)] = line.substr(index + 1);
		}
	}
	return configmap;
}

std::string ConfigManager::SERVER_CONFIG_PATH = "../config/server.conf";
std::string ConfigManager::MYSQL_CONFIG_PATH = "../config/mysql.conf";