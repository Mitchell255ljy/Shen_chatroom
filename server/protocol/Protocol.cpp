#include"Protocol.h"

bool Protocol:: HasCompletePacket(const std::string& buffer)
{
	if (buffer.size() < protocol::HEADER_SIZE)
	{
		return false;
	}
	protocol::Header header;
	memcpy(&header, buffer.c_str(), sizeof(header));
	if (header.magic != protocol::MAGIC)
	{
		std::cout << "Protocol: Invalid magic number" << header.magic << std::endl;
		return false;
	}
	if (header.bodyLength > protocol::MAX_BODY_LENGTH)
	{
		std::cout << "Protocol: Body length too large" << header.bodyLength << std::endl;
		return false;
	}
	std::size_t totalPacketSize = protocol::HEADER_SIZE + header.bodyLength;
	if (buffer.size() >= static_cast<int>(totalPacketSize))
	{
		return true;
	}
	return false;
}

bool Protocol:: ParsePacket(
	std::string& buffer, protocol::MessageType& type, std::string& body)
{
	if (!HasCompletePacket(buffer))
	{
		return false;
	}

	protocol::Header header;
	memcpy(&header, buffer.c_str(), sizeof(header));
	std::size_t totalPacketSize = protocol::HEADER_SIZE + header.bodyLength;
	std::uint8_t typeValue = header.type;
	type = static_cast<protocol::MessageType>(typeValue);
	if (header.bodyLength > 0)
	{
		body = buffer.substr(protocol::HEADER_SIZE, header.bodyLength);
	}
	else
	{
		body.clear();
	}
	buffer.erase(0, totalPacketSize);
	return true;
}

std::string Protocol::Pack(protocol::MessageType type, const std::string& body)
{
	protocol::Header header;
	std::memset(&header, 0, sizeof(header));
	header.magic = protocol::MAGIC;
	header.type = static_cast<uint8_t>(type);
	header.bodyLength = static_cast<uint32_t>(body.size());
	std::string packet;
	packet.reserve(sizeof(header) + body.size());
	packet.append(reinterpret_cast<const char*>(&header), sizeof(header));
	packet.append(body);
	return packet;
}