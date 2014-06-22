#include <iostream>
#include <list>
#include <string>
#include <sqlite3.h>


namespace models
{
	/** Classe responsável por modelar colunas da tabela.
	*/
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
		string sql;

		Field& null(bool null);
		Field& blank(bool blank);
		Field& choices(const &list<list<string>>);
		Field& db_column(string);
		Field& db_index(bool);
		Field& primary_key(bool);
		Field& unique(bool);
	};

	/** Para campos booleanos (TRUE-FALSE) 
		ENUM('TRUE', 'FALSE') 
		*/
	class BooleanField : public Field
	{
		bool __default;

	public:
		bool returnObj;
		BooleanField& default(bool);
	};

	/** Para campos de texto.
		max_length é necessário para a construção deste campo. 
		VARCHAR(max_length)
		*/
	class CharField : public Field
	{
		string __default;
		int __max_length;

	public:
		string returnObj;
		CharField& default(string);
		CharField& max_length(int);
	};

	/** Para campos de números reais. 
		FLOAT(size, d)
		*/
	class FloatField : public Field
	{
		float __default;

	public:
		float returnObj;
		FloatField& size(int);
		FloatField& d(int);
		FloatField& default(float);
	};

	/** Para campos de números inteiros.
		INT(size)
		*/
	class IntegerField : public Field
	{
		int __default;

	public:
		int returnObj;
		IntegerField& size(int);
		IntegerField& default(int);
	};

	/** Para telacionamentos N-1.
		O SQL gerado varia de acordo com o Model referenciado, associando com a PK definida.
		*/
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

	/** Para relacionamentos N-N.
		Uma tabela intermediária será gerada, com os campos necessários para relacioná-las.
		*/
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

	/** Para relacionamentos 1-1.
		A SQL gerada dependerá da chave primária da tabela relacionada.
		*/
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

	/** Classe responsável por modelar tabelas.
		Sua construção funciona como no exemplo abaixo:

		
}

// .default