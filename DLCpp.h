#ifndef DLCPP_MODULE
#define DLCPP_MODULE

#include <iostream>
#include <utility>
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <sqlite3.h>

#define DLCPP_LIST(x) 	std::list<std::map<std::string, x> >
#define DLCPP_MAP(x)	std::map<std::string, x>

#define DLCPP_LIST_ITER(x) std::list<std::map<std::string, x> >
#define DLCPP_MAP_ITER(x)  std::map<std::string, x>::iterator

namespace models
{

    class QuerySet;
    class Field;
    class Model;

///////////////////QUERYSET//////////////////////////////

	class QuerySet
	{
    private:
        std::stringstream SQLquery;
        std::string database;
        std::string table_name;
        std::string pk_field;
        std::string pk_value;

    public:
        QuerySet();

        void set(std::string column, bool           content);
        void set(std::string column, std::string    content);
        void set(std::string column, float          content);
        void set(std::string column, int            content);
        void set(std::string column, QuerySet&      content);

        void remove(void);
	};

	template<typename value_type>
	class SingleSet : protected QuerySet
	{
	private:
		typename DLCPP_MAP(value_type) value;

	public:
		//OPERATORS
		value_type                operator[](const std::string& index);
		SingleSet<value_type>&    operator=(const SingleSet<value_type>& x);
		SingleSet<value_type>&    operator=(const typename DLCPP_MAP(value_type)& x);

		//ITERATORS
		typename DLCPP_MAP_ITER(value_type) begin(void);
		typename DLCPP_MAP_ITER(value_type) end(void);
		typename DLCPP_MAP_ITER(value_type) rbegin(void);
		typename DLCPP_MAP_ITER(value_type) rend(void);

		//CAPACITY
		bool 			empty(void)     const;
		unsigned int 	size(void)      const;
		unsigned int 	max_size(void)  const;

		//INSERT
		std::pair<typename DLCPP_MAP_ITER(value_type), value_type> insert(const value_type& val);
		typename DLCPP_MAP_ITER(value_type)                   insert(typename DLCPP_MAP_ITER(value_type) pos, const value_type& val);
		// range insert is not currently supported

		//ERASE
		void 			erase(typename DLCPP_MAP_ITER(value_type) pos);
		unsigned int 	erase(const std::string& k);
		void			erase(typename DLCPP_MAP_ITER(value_type) first, typename DLCPP_MAP_ITER(value_type) last);
		void 			clear(void);

		// Observers not currently supported

		//OPERATIONS
		typename DLCPP_MAP_ITER(value_type)			find(const std::string& k);
		unsigned int 			count(const std::string& k) const;
		typename DLCPP_MAP_ITER(value_type)			lower_bound(const std::string& k);
		typename DLCPP_MAP_ITER(value_type)			upper_bound(const std::string& k);
		// equal_range not currently supported

		// get_allocator not currently supported
	};

    template<typename value_type>
    class MultipleSet : protected QuerySet
    {
    private:
        DLCPP_LIST(value_type) value;

    public:
        //OPERATORS
        MultipleSet<value_type>&    operator=(const MultipleSet<value_type>& x);
        MultipleSet<value_type>&    operator=(const DLCPP_LIST(value_type)& x);

        //ITERATORS
        DLCPP_LIST_ITER(value_type)    begin(void);
        DLCPP_LIST_ITER(value_type)    end(void);
        DLCPP_LIST_ITER(value_type)    rbegin(void);
        DLCPP_LIST_ITER(value_type)    rend(void);

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
        DLCPP_LIST_ITER(value_type) insert(DLCPP_LIST_ITER(value_type) position, const value_type& val);
        void            insert(DLCPP_LIST_ITER(value_type) position, unsigned int n, const value_type& val);
        //range insert not currently supported

        //ERASE
        DLCPP_LIST_ITER(value_type) erase(DLCPP_LIST_ITER(value_type) position);
        DLCPP_LIST_ITER(value_type) erase(DLCPP_LIST_ITER(value_type) first, DLCPP_LIST_ITER(value_type) last);
        void            clear(void);

        //OPERATIONS
        void splice(DLCPP_LIST_ITER(value_type) position, DLCPP_LIST(value_type)& x);
        void splice(DLCPP_LIST_ITER(value_type) position, DLCPP_LIST(value_type)& x, DLCPP_LIST_ITER(value_type) i);
        void splice(DLCPP_LIST_ITER(value_type) position, DLCPP_LIST(value_type)& x, DLCPP_LIST_ITER(value_type) first, DLCPP_LIST_ITER(value_type) last);
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
		bool __notNull;
		bool __blank;
		std::list<std::list< std::string> > __choices;
		std::string __db_column;
		bool __db_index;
		bool __primary_key;
		bool __unique;

	public:
		std::string sql;

		Field& null(bool null);
		Field& blank(bool blank);
		Field& choices(const std::map<std::string, std::string>&);
		Field& db_column(std::string);
		Field& db_index(bool);
		Field& primary_key(bool);
		Field& unique(bool);
	};

	/** Para campos booleanos (TRUE-FALSE) 
		ENUM('TRUE', 'FALSE') 
		*/
	class BooleanField : public Field
	{
		bool __standard;

	public:
		bool returnObj;
		BooleanField& standard(bool);
	};

	/** Para campos de texto.
		max_length é necessário para a construção deste campo. 
		VARCHAR(max_length)
		*/
	class CharField : public Field
	{
		std::string __standard;
		int __max_length;

	public:
		std::string returnObj;
		CharField& standard(std::string);
		CharField& max_length(int);
	};

	/** Para campos de números reais. 
		FLOAT(size, d)
		*/
	class FloatField : public Field
	{
		float __standard;

	public:
		float returnObj;
		FloatField& size(int);
		FloatField& d(int);
		FloatField& standard(float);
	};

	/** Para campos de números inteiros.
		INT(size)
		*/
	class IntegerField : public Field
	{
		int __standard;

	public:
		int returnObj;
		IntegerField& size(int);
		IntegerField& standard(int);
	};

	/** Para telacionamentos N-1.
		O SQL gerado varia de acordo com o Model referenciado, associando com a PK definida.
		*/
	class ForeignKey : public Field
	{
		std::string __related_name;
		Field& __to_field;
		std::string __on_delete;

	public:
		ForeignKey(const Model&);
		ForeignKey& related_name(std::string);
		ForeignKey& to_field(const Field&);
		ForeignKey& on_delete(std::string);
	};

	/** Para relacionamentos N-N.
		Uma tabela intermediária será gerada, com os campos necessários para relacioná-las.
		*/
	class ManyToManyField : public Field
	{
		std::string __related_name;
		bool __symmetrical;
		Model* __through;
		Field* __through_field[2];
		std::string __db_table;

	public:
		ManyToManyField(const Model&);
		ManyToManyField& related_name(std::string);
		ManyToManyField& symmetrical(bool);
		ManyToManyField& through(const Model&);
		ManyToManyField& through_field(const Field&, const Field&);
		ManyToManyField& db_table(std::string);
	};

	/** Para relacionamentos 1-1.
		A SQL gerada dependerá da chave primária da tabela relacionada.
		*/
	class OneToOneField : public Field
	{
		std::string __related_name;
		Field& __to_field;
		std::string __on_delete;

	public:
		OneToOneField(const Model&);
		OneToOneField& related_name(std::string);
		OneToOneField& to_field(const Field&);
		OneToOneField& on_delete(std::string);
	};

////////////////////MODELS////////////////////////////////

    class Model
    {
    protected:
        DLCPP_MAP(Field*) column;

    public:
        static void DROP(void);

        QuerySet* filter(std::stringstream requestr, bool);
        QuerySet* filter(std::stringstream requestr, std::string);
        QuerySet* filter(std::stringstream requestr, float);
        QuerySet* filter(std::stringstream requestr, int);

        QuerySet* get(std::stringstream requestr, bool);
        QuerySet* get(std::stringstream requestr, std::string);
        QuerySet* get(std::stringstream requestr, float);
        QuerySet* get(std::stringstream requestr, int);

        QuerySet* insert(void);
    };

}

#endif