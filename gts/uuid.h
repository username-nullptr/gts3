#ifndef GTS_HTTP_UUID_H
#define GTS_HTTP_UUID_H

#include <gts/gts_global.h>

namespace gts
{

union GTSCORE_API uuid //version 4
{
public:
	uuid(const std::string &uuid);
	uuid(const uuid &other) = default;

public:
	static uuid generate();

public:
	uuid &operator=(const uuid &other) = default;
	uuid &operator=(const std::string &uuid);

public:
	bool operator==(const uuid &other) const;
	bool operator!=(const uuid &other) const;
	bool operator<(const uuid &other) const;
	bool operator>(const uuid &other) const;

public:
	// aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee
	std::string to_string() const;
	operator std::string() const { return to_string(); }

public:
	uint64_t _wide_integers[2];

	struct _internalData
	{
		uint32_t d0;
		uint16_t d1;
		uint16_t d2;
		uint8_t  d3[8];
	}
	_uuid;

	struct _byteRepresentation
	{
		uint8_t d0[4];
		uint8_t d1[2];
		uint8_t d2[2];
		uint8_t d3[8];
	}
	_bytes;
};

} //namespace gts


#endif //GTS_HTTP_UUID_H
