#include "DLCpp.h"

///////////////////QUERYSET//////////////////////////////

template<typename value_type>
std::stringstream updateStatement(	std::stringstream &SQLquery,
									std::string &table_name, 
									value_type	&column, 
									std::string	&content, 
									std::string &pk_field,
									std::string &pk_value )
{
	SQLquery << "UPDATE ";
	SQLquery << table_name;
	SQLquery << " SET ";
	SQLquery << column;
	SQLquery << "=";
	SQLquery << content;
	SQLquery << " WHERE ";
	SQLquery << pk_field;
	SQLquery << "=";
	SQLquery << pk_value;
	SQLquery << ";";

	return &SQLquery;
}

std::stringstream deleteStatement(	std::string &table_name,
									std::string &pk_field,
									std::string &pk_value )
{
	std::stringstream SQLquery;

	SQLquery << "DELETE FROM ";
	SQLquery << table_name;
	SQLquery << " WHERE ";
	SQLquery << pk_field;
	SQLquery << "=";
	SQLquery << pk_value;
	SQLquery << ";";
}
									

void models::QuerySet::set(std::string column, bool content)
{
	SQLquery = updateStatement<bool>(
							SQLquery,
							table_name,
							column,
							content,
							pk_field,
							pk_value );
}


void models::QuerySet::set(std::string column, std::string content)
{
	SQLquery = updateStatement<std::string>(
							SQLquery,
							table_name,
							column,
							content,
							pk_field,
							pk_value );
}


void models::QuerySet::set(std::string column, float content)
{
	SQLquery = updateStatement<float>(
							SLQquery,
							table_name,
							column,
							content,
							pk_field,
							pk_value );
}


void models::QuerySet::set(std::string column, int content)
{
	SQLquery = updateStatement<int>(
							SLQquery,
							table_name,
							column,
							content,
							pk_field,
							pk_value );
}


void models::QuerySet::set(std::string column, Model& content)
{
	std::string pk = content.pk;

	
}

void models::QuerySet::remove()
{
	SQLquery = deleteStatement(
							table_name,
							pk_field,
							pk_value );
}