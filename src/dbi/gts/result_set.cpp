#include "result_set.h"

namespace dbi
{

result_iterator::~result_iterator()
{

}

#define GET_XX_BY_NAME(_return, _func_name) \
    _return result_iterator::_func_name(const std::string &column_name) const { \
        for(std::size_t column=0; column<column_count(); column++) { \
            if( this->column_name(column) == column_name ) \
                return _func_name(column); \
        } \
        throw exception(-1, "dbi::result_set::" #_func_name ": Invalid column name."); \
    }

GET_XX_BY_NAME(cpp::optional<bool>    , get_opt_bool  )
GET_XX_BY_NAME(cpp::optional<int8_t>  , get_opt_char  )
GET_XX_BY_NAME(cpp::optional<uint8_t> , get_opt_uchar )
GET_XX_BY_NAME(cpp::optional<int16_t> , get_opt_short )
GET_XX_BY_NAME(cpp::optional<uint16_t>, get_opt_ushort)
GET_XX_BY_NAME(cpp::optional<int32_t> , get_opt_int   )
GET_XX_BY_NAME(cpp::optional<uint32_t>, get_opt_uint  )
GET_XX_BY_NAME(cpp::optional<int64_t> , get_opt_long  )
GET_XX_BY_NAME(cpp::optional<uint64_t>, get_opt_ulong )
GET_XX_BY_NAME(cpp::optional<float>   , get_opt_float )
GET_XX_BY_NAME(cpp::optional<double>  , get_opt_double)

cpp::optional<std::string> result_iterator::get_opt_string(const std::string &column_name, std::string maxlen) const
{
    for(std::size_t column=0; column<column_count(); column++)
    {
        if( this->column_name(column) == column_name )
            return get_opt_string(column, maxlen);
    }
    throw exception(-1, "dbi::result_set::get_opt_string: Invalid column name.");
}

#define GET_XX_BY_INDEX(_return, _basic_func_name) \
	_return result_iterator::get_##_basic_func_name(std::size_t column) const { \
		auto opt = get_opt_##_basic_func_name(column); \
		if( opt.has_value() ) \
			return opt.value(); \
		else \
			throw exception(-1, "dbi::result_set::get_" #_return ": Optional is empty."); \
	}

GET_XX_BY_INDEX(bool    , bool  )
GET_XX_BY_INDEX(int8_t  , char  )
GET_XX_BY_INDEX(uint8_t , uchar )
GET_XX_BY_INDEX(int16_t , short )
GET_XX_BY_INDEX(uint16_t, ushort)
GET_XX_BY_INDEX(int32_t , int   )
GET_XX_BY_INDEX(uint32_t, uint  )
GET_XX_BY_INDEX(int64_t , long  )
GET_XX_BY_INDEX(uint64_t, ulong )
GET_XX_BY_INDEX(float   , float )
GET_XX_BY_INDEX(double  , double)

std::string result_iterator::get_string(std::size_t column, std::size_t maxlen) const
{
	auto opt = get_opt_string(column, maxlen);
	if( opt.has_value() )
		return opt.value();
	else
		throw exception(-1, "dbi::result_set::get_string: Optional is empty.");
}

GET_XX_BY_NAME(bool    , get_bool  )
GET_XX_BY_NAME(int8_t  , get_char  )
GET_XX_BY_NAME(uint8_t , get_uchar )
GET_XX_BY_NAME(int16_t , get_short )
GET_XX_BY_NAME(uint16_t, get_ushort)
GET_XX_BY_NAME(int32_t , get_int   )
GET_XX_BY_NAME(uint32_t, get_uint  )
GET_XX_BY_NAME(int64_t , get_long  )
GET_XX_BY_NAME(uint64_t, get_ulong )
GET_XX_BY_NAME(float   , get_float )
GET_XX_BY_NAME(double  , get_double)

std::string result_iterator::get_string(const std::string &column_name, std::size_t maxlen) const
{
	for(std::size_t column=0; column<column_count(); column++)
	{
		if( this->column_name(column) == column_name )
			return get_string(column, maxlen);
	}
	throw exception(-1, "dbi::result_set::get_string: Invalid column name.");
}

int result_iterator::row_id() const
{
	auto res = get_opt_int(0);
	return res? *res : -1;
}

} //namespace dbi
