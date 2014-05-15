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

	class BigIntegerField : public Field
	{
		signed long long __default; 
	};

	class BooleanField : public Field
	{
		bool __default;
	};

	class CharField : public Field
	{
		string __default;
		int __max_length;
	};

	class FloatField : public Field
	{
		float __default;
	};

	class IntegerField : public Field
	{
		int __default;
	};

	class PositiveIntegerField : public Field
	{
		unsigned int __default;
	};

	class PositiveSmallIntegerField : public Field
	{
		unsigned short int __default;
	};

	class SmallIntegerField : public Field
	{
		short int __default;
	};

	class TextField : public Field
	{
		string __default;
	};

	class ForeignKey : public Field
	{
		string __related_name;
		Field& __to_field;
		string __on_delete;

	public:
		ForeignKey(const &Model);
	};

	class ManyToManyField : public Field
	{
		string __related_name;
		bool __symmetrical;
		Model& __through;
		Field& __through_field[2];
		string __db_table;

	public:
		ManyToManyField(const &Model);
	};

	class OneToOneField : public Field
	{
		string __related_name;
		Field& __to_field;
		string __on_delete;

	public:
		OneToOneField(const &Model);
	};
}

// .default