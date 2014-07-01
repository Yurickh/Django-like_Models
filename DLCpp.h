#include <iostream>
#include <utility>
#include <list>
#include <map>
#include <string>
#include <sqlite3.h>

#define DLCPP_LIST(x) 	std::list<std::map<string, x>>
#define DLCPP_MAP(x)	std::map<string, x>

#define DLCPP_LIST_ITER std::list<std::map<string, value_type>>
#define DLCPP_MAP_ITER  std::map<string, value_type>::iterator

namespace models
{

///////////////////RETURNSET//////////////////////////////

	class ReturnSet
	{
		//YEAH NOTHING HERE
	};

	template<typename value_type>
	class SingleSet : public ReturnSet
	{
	private:
		DLCPP_MAP(value_type) value;

	public:
		//OPERATORS
		value_type                operator[](const string& index);
		SingleSet<value_type>&    operator=(const SingleSet<value_type>& x);
		SingleSet<value_type>&    operator=(const DLCPP_MAP(value_type)& x);

		//ITERATORS
		DLCPP_MAP_ITER begin(void);
		DLCPP_MAP_ITER end(void);
		DLCPP_MAP_ITER rbegin(void);
		DLCPP_MAP_ITER rend(void);

		//CAPACITY
		bool 			empty(void)     const;
		unsigned int 	size(void)      const;
		unsigned int 	max_size(void)  const;

		//INSERT
		pair<DLCPP_MAP_ITER, value_type> insert(const value_type& val);
		DLCPP_MAP_ITER                   insert(DLCPP_MAP_ITER pos, const value_type& val);
		// range insert is not currently supported

		// ERASE
		void 			erase(DLCPP_MAP_ITER pos);
		unsigned int 	erase(const string& k);
		void			erase(DLCPP_MAP_ITER first, DLCPP_MAP_ITER last);
		void 			clear(void);

		// Observers not currently supported

		//OPERATIONS
		DLCPP_MAP_ITER			find(const string& k);
		unsigned int 			count(const string& k) const;
		DLCPP_MAP_ITER			lower_bound(const string& k);
		DLCPP_MAP_ITER			upper_bound(const string& k);
		// equal_range not currently supported

		// get_allocator not currently supported
	};

    template<typename value_type>
    class MultipleSet : public ReturnSet
    {
    private:
        DLCPP_LIST(value_type) value;

    public:
        //OPERATORS
        MultipleSet<value_type>&    operator=(const MultipleSet<value_type>& x);
        MultipleSet<value_type>&    operator=(const DLCPP_LIST(value_type)& x);

        //ITERATORS
        DLCPP_LIST_ITER    begin(void);
        DLCPP_LIST_ITER    end(void);
        DLCPP_LIST_ITER    rbegin(void);
        DLCPP_LIST_ITER    rend(void);

        //CAPACITY
        bool            empty(void)     const;
        unsigned int    size(void)      const;
        unsigned int    max_size(void)  const;

        //ELEMENT ACCESS
        value_type& front(void);
        value_type& back(void);

        //MODIFIERS
        void push_front(const value_type& val);
        void pop_front(void);
        void push_back(const value_type& val);
        void pop_back(void);

        //INSERT
        DLCPP_LIST_ITER insert(DLCPP_LIST_ITER position, const value_type& val);
        void            insert(DLCPP_LIST_ITER position, unsigned int n, const value_type& val);
        //range insert not currently supported

        //ERASE
        DLCPP_LIST_ITER erase(DLCPP_LIST_ITER position);
        DLCPP_LIST_ITER erase(DLCPP_LIST_ITER first, DLCPP_LIST_ITER last);
        void            clear(void);

        //OPERATIONS
        void splice(DLCPP_LIST_ITER position, DLCPP_LIST(value_type)& x);
        void splice(DLCPP_LIST_ITER position, DLCPP_LIST(value_type)& x, DLCPP_LIST_ITER i);
        void splice(DLCPP_LIST_ITER position, DLCPP_LIST(value_type)& x, DLCPP_LIST_ITER first, DLCPP_LIST_ITER last);
        void remove(const value_type& val);
        void merge(DLCPP_LIST(value_type)& x);
        void sort(void);
        void reverse(void);
        // remove_if and unique not currently supported

        // get_allocator not currently supported
    };

////////////////////FIELDS////////////////////////////////

	/** Classe responsável por modelar colunas da tabela.
	*/
	class Field
	{
	protected:
		bool __null;
		bool __blank;
		std::list<std::list<string>> __choices;
		string __db_column;
		bool __db_index;
		bool __primary_key;
		bool __unique;

	public:
		string sql;

		Field& null(bool null);
		Field& blank(bool blank);
		Field& choices(const &std::list<std::list<string>>);
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


////////////////////QUERYSET////////////////////////////////

	typedef std::map<string, Field*> FieldMap;

	class QuerySet
	{
		public:
			QuerySet();
	};
}

// .default