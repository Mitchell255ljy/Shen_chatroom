#include<iostream>
#include<unistd.h>
#include<sys/stat.h>
#include"net/Service.h"
int main()
{

	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != nullptr)
	{
		std::cout << "当前工作目录:" << cwd << std::endl;
	}
	struct stat buffer;
	bool exists01 = (stat("../config/server.conf", &buffer) == 0);
	std::cout << "server.conf是否存在:" << (exists01 ? "true" : "false") << std::endl;
	bool exists02 = (stat("../config/mysql.conf", &buffer) == 0);
	std::cout << "mysql.conf是否存在:" << (exists02 ? "true" : "false") << std::endl;

	Service m_service(5, 5);
	m_service.Run();
	return 0;
}