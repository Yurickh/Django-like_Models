#ifndef DLCPP_MODULE
#define DLCPP_MODULE

#include <ctime>
#include <sqlite3.h>

#include <iostream>
#include <utility>
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <stdexcept>
#include <fstream>

extern "C" {
#include "DLCpp_SETTINGS.h"
}

#define DLCPP_LIST(x) 	std::list<std::map<std::string, x> >
#define DLCPP_MAP(x)	std::map<std::string, x>

#define DLCPP_LIST_ITER(x) std::list<std::map<std::string, x> >
#define DLCPP_MAP_ITER(x)  std::map<std::string, x>::iterator

//For correctely CRTPing Models.
#define DLCPP_NEW_MODEL(Type) class Type : public models::Model<Type>

namespace models
{

    class QuerySet;
    class Field;

	template<class Derived>
    class Model;

	std::string getime(void);

/*
///////////////////QUERYSET//////////////////////////////

	class QuerySet
	{
    private:
        std::stringstream SQLquery;
		Model* table;
        std::pair<std::string, std::string> __pk;

    public:
        QuerySet();
		
		std::string pk(void);

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

		/*Field& null(bool null);
		Field& blank(bool blank);
		Field& choices(const std::map<std::string, std::string>&);
		Field& db_column(std::string);
		Field& db_index(bool);
		Field& primary_key(bool);
		Field& unique(bool);*/
	};

	/** Para campos booleanos (TRUE-FALSE) 
		ENUM('TRUE', 'FALSE') 
		*
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
		*
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
		*
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
		IntegerField(void);

		int returnObj;
		IntegerField& size(int);
		IntegerField& standard(int);
	};
/*
	/** Para telacionamentos N-1.
		O SQL gerado varia de acordo com o Model referenciado, associando com a PK definida.
		*
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
		*
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
		*
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
/*
////////////////////MODELS////////////////////////////////
*/

	template<class Derived> // Simulated Dynamic Binding
    class Model
    {
	protected:
		DLCPP_MAP(Field) column; // Columns
		std::string ref; // TableName

	public:
		static sqlite3* db;

		~Model(void){ 
			int err;

			err = sqlite3_close(db);

			if(DLCPP_VERBOSE_LEVEL == 2)
			{
				std::fstream log_file;

				log_file.open(DLCPP_LOGFILE, std::fstream::out | std::fstream::app);

				if(err != SQLITE_OK)
				{
					log_file << "[   models::~Model()    ][" << getime() << "] ERROR DISCONNECTING TO DATABASE; SQLITE ERROR CODE ";
					log_file << sqlite3_errcode(db);
					log_file << "." << std::endl;
				}
				else
				{
					log_file << "[   models::~Model()    ][" << getime() << "] SUCCESSFULLY DISCONNECTED." << std::endl;
				}
			}

		}
		Model() throw (std::runtime_error)
		{
			int err;
			std::string classname;

			classname = __PRETTY_FUNCTION__;
			classname = classname.substr(classname.find("Derived = ")+10, classname.size());
			classname = classname.substr(0, classname.size()-1);

			ref = classname;

			err = sqlite3_open(DLCPP_PATH_2_DB, &db);

			if(DLCPP_VERBOSE_LEVEL == 2)
			{
				std::fstream log_file;

				log_file.open(DLCPP_LOGFILE, std::fstream::out | std::fstream::app);
				if(err != SQLITE_OK)
				{
					log_file << "[    models::Model()    ][" << getime() <<"] ERROR CONNECTING TO DATABASE; SQLITE ERROR CODE ";
					log_file << sqlite3_errcode(db);
					log_file << "." << std::endl;

					log_file.close();
					throw std::runtime_error("Connection error.");
				}
				else
				{
					log_file << "[    models::Model()    ][" << getime() <<"] CONNECTION SUCCESSFUL." << std::endl;
				}
				log_file.close();
			}
			else
			{
				if (err)
					throw std::runtime_error("Connection error.");
			}
		}

		Field* pk;

		static void CREATE(void);
        static void DROP(void);

		void retrieve(DLCPP_MAP(Field)&, std::string&);

        /*QuerySet* filter(std::stringstream requestr, bool);
        QuerySet* filter(std::stringstream requestr, std::string);
        QuerySet* filter(std::stringstream requestr, float);
        QuerySet* filter(std::stringstream requestr, int);

        QuerySet* get(std::stringstream requestr, bool);
        QuerySet* get(std::stringstream requestr, std::string);
        QuerySet* get(std::stringstream requestr, float);
        QuerySet* get(std::stringstream requestr, int);

        QuerySet* insert(void);*/
    };

	template<class Derived>
	sqlite3* Model<Derived>::db = NULL;

	template<class Derived>
	void Model<Derived>::retrieve(DLCPP_MAP(Field) &a, std::string &c)
	{
		a = this->column;
		c = this->ref;
	}

	template<class Derived>
	void Model<Derived>::CREATE()
	{
		DLCPP_MAP_ITER(Field) it;
		DLCPP_MAP(Field) column;
		std::stringstream createStt;
		std::string temp;
		std::string ref;
		char** zErrMsg;
		models::Model<Derived>* m;
		int err;

		m = new Derived; // already opens a connection with database through db;

		
		m->retrieve(column, ref);

		createStt << "CREATE TABLE IF NOT EXISTS " << ref << "(\n";

		for(it = column.begin(); it != column.end();)
		{
			createStt << it->first << " " << it->second.sql;
			++it;
			if(it != column.end())
				createStt << ",";
			createStt << "\n";
		}

		createStt << ");";

		temp = createStt.str();

		err = sqlite3_exec(Derived::db, temp.c_str(), NULL, NULL, zErrMsg);

		if(DLCPP_VERBOSE_LEVEL)
		{
			std::fstream log_file;
			log_file.open(DLCPP_LOGFILE, std::fstream::out | std::fstream::app);


			if(err != SQLITE_OK)
			{
				if(DLCPP_VERBOSE_LEVEL == 2)
					log_file << "[models::Model::CREATE()][" << getime() << "] ERROR WHILE CREATING TABLE. SQLITE ERROR CODE " << sqlite3_errcode(Derived::db) << zErrMsg << ".\n";
				else
					log_file << "CREATE CLAUSE FAILED.\n";

				log_file << "[models::Model::CREATE()][" << getime() << "] SQL:";
			}
			
			log_file << "\n========\n" << temp << "\n========\n" <<std::endl;

			sqlite3_free(zErrMsg);
			
			log_file.close();
		}

		delete m; // closes connection
	}

	template<class Derived>
	void Model<Derived>::DROP()
	{
		std::stringstream deleteStt;
		std::string temp;
		char** zErrMsg;
		Model<Derived>* m = new Derived;
		DLCPP_MAP(Field) f;
		int rc;

		m->retrieve(f, temp);

		deleteStt << "DROP TABLE " << temp << ";";
		temp = deleteStt.str();
		rc = sqlite3_exec(Derived::db, temp.c_str(), NULL, NULL, zErrMsg);

		if(DLCPP_VERBOSE_LEVEL)
		{
			std::fstream log_file;
			log_file.open(DLCPP_LOGFILE, std::fstream::out | std::fstream::app);

			if(rc != SQLITE_OK)
			{
				if(DLCPP_VERBOSE_LEVEL == 2)
					log_file << "[ models::Model::DROP() ][" + getime() << "] ERROR WHILE DROPPING TABLE. SQLITE ERROR CODE " << sqlite3_errcode(Derived::db) << ": " << zErrMsg <<"\n";
				else
					log_file << "DROP CLAUSE FAILEDn\n";

				log_file << "[ models::Model::DROP() ][" + getime() << "] SQL:";

				sqlite3_free(zErrMsg);
			}

			log_file << "\n========\n" << temp << "\n========\n" <<std::endl;

			log_file.close();
		}

		delete m;
	}
}
#endif