#include "execute_interface.h"

namespace dbi
{

class DBI_DECL_HIDDEN execute_interface_impl
{
    DBI_DISABLE_COPY_MOVE(execute_interface_impl)

public:
    execute_interface_impl() = default;
    std::size_t m_query_string_buffer_size = 1024;
};

/*------------------------------------------------------------------------------------------------------------------------------*/

execute_interface::execute_interface() :
    m_impl(new execute_interface_impl())
{

}

execute_interface::~execute_interface()
{
    delete m_impl;
}

table_data execute_interface::prepare_query(const std::string &statement, error_code &error)
{
	table_data result;
	result.reserve(64);

	auto set = create_query_work(statement, error);
	if( error )
		return result;

	while( set->next() )
	{
		result.emplace_back();
		auto &vector = result.back();

		vector.reserve(set->column_count());
        auto buf_size = query_string_buffer_size();

		for(std::size_t column=0; column<set->column_count(); column++)
		{
			auto ope = set->get_opt_string(column, buf_size);
			vector.emplace_back(ope.has_value()? cell_data(set->column_name(column), ope.value()) : cell_data(set->column_name(column)));
		}
	}
	return result;
}

execute_interface &execute_interface::set_query_string_buffer_size(std::size_t size)
{
    m_impl->m_query_string_buffer_size = size;
    return *this;
}

std::size_t execute_interface::query_string_buffer_size()
{
    return m_impl->m_query_string_buffer_size;
}

} //namespace dbi
