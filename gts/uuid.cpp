#include "uuid.h"
#include <random>
#include <cstdio>
#include <cinttypes>

namespace gts
{

uuid::uuid(const std::string &uuid)
{
	if( uuid.size() != 38 ) //aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee
	{
		memset(this, 0, sizeof(uuid));
		return;
	}
	sscanf(uuid.c_str(), "%08" SCNx32 "-%04" SCNx16 "-%04" SCNx16 "-%02" SCNx8 "%02" SCNx8 "-%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8,
		   &_uuid.d0, &_uuid.d1, &_uuid.d2, &_uuid.d3[0], &_uuid.d3[1], &_uuid.d3[2], &_uuid.d3[3], &_uuid.d3[4], &_uuid.d3[5], &_uuid.d3[6], &_uuid.d3[7]);
}

uuid uuid::generate()
{
#if !defined(__APPLE__) && !defined(__clang__)
	thread_local std::random_device rd;
	thread_local auto gen = std::mt19937_64(rd());
#else
	std::random_device rd;
	std::mt19937_64 gen(rd());
#endif
	std::uniform_int_distribution<uint64_t> dis64;
	uuid obj("");

	obj._wide_integers[0] = dis64(gen);
	obj._wide_integers[1] = dis64(gen);

	obj._bytes.d3[0] = (obj._bytes.d3[0] & 0x3F) | static_cast<uint8_t>(0x80);
	obj._bytes.d2[1] = (obj._bytes.d2[1] & 0x0F) | static_cast<uint8_t>(0x40);

	return obj;
}

uuid &uuid::operator=(const std::string &uuid)
{
	if( uuid.size() == 38 ) //aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee
	{
		sscanf(uuid.c_str(), "%08" SCNx32 "-%04" SCNx16 "-%04" SCNx16 "-%02" SCNx8 "%02" SCNx8 "-%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8,
			   &_uuid.d0, &_uuid.d1, &_uuid.d2, &_uuid.d3[0], &_uuid.d3[1], &_uuid.d3[2], &_uuid.d3[3], &_uuid.d3[4], &_uuid.d3[5], &_uuid.d3[6], &_uuid.d3[7]);
	}
	return *this;
}

bool uuid::operator==(const uuid &other) const
{
	return memcmp(&other, this, sizeof(uuid)) == 0;
}

bool uuid::operator!=(const uuid &other) const
{
	return !operator==(other);
}

bool uuid::operator<(const uuid &other) const
{
	return memcmp(this, &other, sizeof(uuid)) < 0;
}

bool uuid::operator>(const uuid &other) const
{
	return memcmp(this, &other, sizeof(uuid)) > 0;
}

std::string uuid::to_string() const
{
	char buffer[39]; //aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee
	sprintf(buffer, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			_uuid.d0, _uuid.d1, _uuid.d2, _uuid.d3[0], _uuid.d3[1], _uuid.d3[2], _uuid.d3[3], _uuid.d3[4], _uuid.d3[5], _uuid.d3[6], _uuid.d3[7]);
	return buffer;
}

} //namespace gts
