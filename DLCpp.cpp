#include "DLCpp.h"

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

	temp << sql << " DEFAULT " << s;

	sql = temp.str();

	return *this;
}

models::IntegerField::IntegerField()
{
	__notNull = true;
	__blank = false;
	__primary_key = false;
	__unique = false;
	__db_index = false;
	
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

int models::cb_single(void* p_data, int num_fields, char**p_fields, char**p_col_names)
{
		DLCPP_MAP(std::string)* temp = static_cast<DLCPP_MAP(std::string)*>(p_data);

		if(!temp->empty())
			return 1;
		
        for(int i=0; i<num_fields; ++i)
            (*temp)[p_col_names[i]] = p_fields[i];

		return 0;
	}