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
	time_t now = time(0);
	std::string dt = ctime(&now);
	dt = dt.substr(0, dt.size()-1);

	return dt;
}