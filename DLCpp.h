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

	public:
		string createSQL;

		Field& null(bool null);
		Field& blank(bool blank);
		Field& choices(const &list<list<string>>);
		Field& db_column(string);
		Field& db_index(bool);
		Field& primary_key(bool);
		Field& unique(bool);
	};

	class BigIntegerField : public Field
	{
		signed long long __default;

	public:
		signed long long returnObj;
		BigIntegerField& default(signed long long);
	};

	class BooleanField : public Field
	{
		bool __default;

	public:
		bool returnObj;
		BooleanField& default(bool);
	};

	class CharField : public Field
	{
		string __default;
		int __max_length;

	public:
		string returnObj;
		CharField& default(string);
		CharField& max_length(int);
	};

	class FloatField : public Field
	{
		float __default;

	public:
		float returnObj;
		FloatField& default(float);
	};

	class IntegerField : public Field
	{
		int __default;

	public:
		int returnObj;
		IntegerField& default(int);
	};

	class PositiveIntegerField : public Field
	{
		unsigned int __default;

	public:
		unsigned int returnObj;
		PositiveIntegerField& default(unsigned int);
	};

	class PositiveSmallIntegerField : public Field
	{
		unsigned short int __default;

	public:
		unsigned short int returnObj;
		PositiveSmallIntegerField& default(unsigned short int);
	};

	class SmallIntegerField : public Field
	{
		short int __default;

	public:
		short int returnObj;
		SmallIntegerField& default(short int);
	};

	class TextField : public Field
	{
		string __default;

	public:
		string returnObj;
		TextField& default(string);
	};

	class ForeignKey : public Field
	{
		string __related_name;
		Field& __to_field;
		string __on_delete;

	public:
		ForeignKey(const &Model);
		ForeignKey& related_name(string);
		ForeignKey& to_field(const &Field);
		ForeignKey& on_delete(string);
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
		ManyToManyField& related_name(string);
		ManyToManyField& symmetrical(bool);
		ManyToManyField& through(const &Model);
		ManyToManyField& through_field(const Field&, const Field&);
		ManyToManyField& db_table(string);
	};

	class OneToOneField : public Field
	{
		string __related_name;
		Field& __to_field;
		string __on_delete;

	public:
		OneToOneField(const &Model);
		OneToOneField& related_name(string);
		OneToOneField& to_field(const &Field);
		OneToOneField& on_delete(string);
	};
}

// .default