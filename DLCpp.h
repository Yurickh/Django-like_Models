#include <iostream>
#include <list>
#include <string>
#include <sqlite3.h>

namespace models
{
	class Field
	{
	protected:
		bool __null;
		bool __blank;
		list<list<string>> __choices;
		string __db_column;
		bool __db_index;
		bool __primary_key;
		bool __unique;
	};

	class BooleanField : public Field
	{
		bool __default;
	};
}

// .default