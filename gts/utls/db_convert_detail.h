#ifndef GTS_UTLS_DB_CONVERT_DETAIL_H
#define GTS_UTLS_DB_CONVERT_DETAIL_H

namespace gts { namespace dbi { namespace convert_detail
{

template <typename T>
void set_property_by_cell(T &obj, const rttr::property &property, const cell_data &cell)
{
	if( property.get_type() == GTS_RTTR_TYPE(bool) )
		property.set_value(obj, cell.to_bool());

	else if( property.get_type() == GTS_RTTR_TYPE(std::string) )
		property.set_value(obj, cell.to_string());

	else if( property.get_type() == GTS_RTTR_TYPE(signed char) )
		property.set_value(obj, cell.get<signed char>());

	else if( property.get_type() == GTS_RTTR_TYPE(unsigned char) )
		property.set_value(obj, cell.get<unsigned char>());

	else if( property.get_type() == GTS_RTTR_TYPE(short) )
		property.set_value(obj, cell.get<short>());

	else if( property.get_type() == GTS_RTTR_TYPE(unsigned short) )
		property.set_value(obj, cell.get<unsigned short>());

	else if( property.get_type() == GTS_RTTR_TYPE(int) )
		property.set_value(obj, cell.get<int>());

	else if( property.get_type() == GTS_RTTR_TYPE(unsigned int) )
		property.set_value(obj, cell.get<unsigned int>());

	else if( property.get_type() == GTS_RTTR_TYPE(long) )
		property.set_value(obj, cell.get<long>());

	else if( property.get_type() == GTS_RTTR_TYPE(unsigned long) )
		property.set_value(obj, cell.get<unsigned long>());

	else if( property.get_type() == GTS_RTTR_TYPE(long long) )
		property.set_value(obj, cell.get<long long>());

	else if( property.get_type() == GTS_RTTR_TYPE(unsigned long long) )
		property.set_value(obj, cell.get<unsigned long long>());

	else if( property.get_type() == GTS_RTTR_TYPE(float) )
		property.set_value(obj, cell.get<float>());

	else if( property.get_type() == GTS_RTTR_TYPE(double) )
		property.set_value(obj, cell.get<double>());

	else if( property.get_type() == GTS_RTTR_TYPE(long double) )
		property.set_value(obj, cell.get<long double>());

	else throw exception(-1, "db_convert: '无效的字段类型'.");
}

template <typename T>
void cell_to_struct_by_index(const row_vector &row, T &obj, rttr::array_range<rttr::property> &array, std::size_t column)
{
	assert(column == 0);
	auto it = array.begin();
	for(; it!=array.end() and column<row.size(); ++it, ++column)
		set_property_by_cell(obj, *it, row[column]);
}

template <typename T>
void cell_to_struct_by_index(const row_vector &row, T &obj, rttr::array_range<rttr::property> &array,
							 std::size_t column, std::size_t index)
{
	if( index >= array.size() or column >= row.size() )
		throw exception(-2, "db_convert: '超出列的范围'.");
	else
	{
		auto it = std::next(array.begin(), index);
		set_property_by_cell(obj, *it, row[column]);
	}
}

template <typename T, typename...Tail>
void cell_to_struct_by_index(const row_vector &row, T &obj, rttr::array_range<rttr::property> &array,
							 std::size_t column, std::size_t index, Tail...indexes)
{
	cell_to_struct_by_index(row, obj, array, column++, index);
	cell_to_struct_by_index(row, obj, array, column++, indexes...);
}

}}} //namespace gts::dbi::convert_detail


#endif //GTS_UTLS_DB_CONVERT_DETAIL_H
