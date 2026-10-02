#pragma once
#include<iostream>
#include<string>
#include<cstring>
#include"../../common/protocol.h"

class Protocol
{
public:
	Protocol() = default;
	static bool HasCompletePacket(const std::string& buffer);
	static bool ParsePacket(
		std::string& buffer, protocol::MessageType& type, std::string& body);
	static std::string Pack(protocol::MessageType type, const std::string& body);
};