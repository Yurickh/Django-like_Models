#include "DLCpp.h"

models::Field& models::Field::null(bool null)
{
    this->__notNull = null;

    size_t pos = sql.find("NULL");

    // if already not null
    if(sql.substr(pos-4, pos).find("NOT") != std::string::npos)
    {
        if(null)
            sql.replace(pos-4, 4, "");
    } else {
        if(!null)
            sql.insert(pos, "NOT ");
    }

    return *this;
}

models::Field& models::Field::db_column(std::string db_column)
{
    this->__db_column = db_column;

    return *this;
}

models::Field& models::Field::db_index(bool index)
{
    this->__db_index = index;

    return *this;
}

models::Field& models::Field::primary_key(bool pk)
{
    __primary_key = pk;

    if(!pk)
    {
        //if it is a primary key
        if(sql.find("PRIMARY") != std::string::npos)
            sql.replace(sql.find("PRIMARY KEY"), 11, " ");
    }
    else
        if(sql.find("PRIMARY") == std::string::npos)
            sql.insert(sql.size(), " PRIMARY KEY");

    return *this;
}

models::Field& models::Field::unique(bool unique)
{
    __unique = unique;

    if(!unique)
    {
        if(sql.find("UNIQUE") != std::string::npos)
            sql.replace(sql.find("UNIQUE"), 6, " ");
    } else if(sql.find("UNIQUE") == std::string::npos)
            sql.insert(sql.size(), " UNIQUE");

    return *this;
}

models::BooleanField& models::BooleanField::standard(bool s)
{
    std:: stringstream temp;

    temp << sql << " DEFAULT '" << s << "'";
    sql = temp.str();

    return *this;
}

models::CharField& models::CharField::standard(std::string s)
{
    sql.insert(sql.size(), " DEFAULT '" + s + "'");

    return *this;
}

models::CharField& models::CharField::max_length(int ml)
{
    size_t pos = sql.find("VARCHAR")+7;
    
    sql.insert(pos, "(" + tostring(ml) + ")");

    return *this;
}

models::FloatField& models::FloatField::size_d(int s, int d)
{
    size_t pos = sql.find("FLOAT")+5;

    sql.insert(pos, "("+tostring(s)+","+tostring(d)+")");

    return *this;
}

models::FloatField& models::FloatField::standard(float s)
{
    sql.insert(sql.size(), std::string(" DEFAULT '") + tostring(s) + "'");

    return *this;
}

models::IntegerField& models::IntegerField::size(int s)
{
	std::size_t found;
	std::stringstream t1;
	std::string t2;

	t1 << "(" << s << ")";
	t2 = t1.str();

	found = sql.find(std::string(" INT"));
	found += 4;

	sql.insert(found, t2);

	return *this;
}

models::IntegerField& models::IntegerField::standard(int s)
{
	std::stringstream temp;

	temp << sql << " DEFAULT '" << s << "'";

	sql = temp.str();

	return *this;
}

models::BooleanField::BooleanField()
{
    __notNull      = true;
    __primary_key  = false;
    __unique       = false;
    __db_index     = false;

    sql = "BOOL NOT NULL";
}

models::CharField::CharField()
{
    __notNull       = true;
    __primary_key   = false;
    __unique        = false;
    __db_index      = false;

    sql = "VARCHAR NOT NULL";
}

models::FloatField::FloatField()
{
    __notNull       = true;
    __primary_key   = false;
    __unique        = false;
    __db_index      = false;

    sql = "FLOAT NOT NULL";
}

models::IntegerField::IntegerField()
{
	__notNull      = true;
	__primary_key  = false;
	__unique       = false;
	__db_index     = false;
	
	sql = "INT NOT NULL";
}

std::string models::getime()
{
	time_t now;

	do now = time(0); while(now == -1);
	std::string dt = ctime(&now);
	dt = dt.substr(0, dt.size()-1);

	return dt;
}

std::string models::tostring(int value)
{
	std::stringstream temp;

	temp << value;

	return temp.str();
}

std::string models::tostring(bool value)
{
	return (value)? "TRUE" : "FALSE";
}

std::string models::tostring(float value)
{
	std::stringstream temp;

	temp << value;

	return temp.str();
}

std::string models::tostring(std::string value)
{
    return std::string(value);
}

int models::cb_single(void* p_data, int num_fields, char**p_fields, char**p_col_names)
{
	DLCPP_MAP* temp = static_cast<DLCPP_MAP*>(p_data);

	if(!temp->empty())
		return 1;
	
    for(int i=0; i<num_fields; ++i)
        (*temp)[p_col_names[i]] = p_fields[i];

		return 0;
} 

int models::cb_multiple(void* p_data, int num_fields, char**p_fields, char **p_col_names)
{
	DLCPP_MAP t;
	DLCPP_LIST* temp = static_cast<DLCPP_LIST*>(p_data);

	for(int i=0; i<num_fields; ++i)
		t[p_col_names[i]] = p_fields[i];

	temp->push_back(t);

	return 0;
}