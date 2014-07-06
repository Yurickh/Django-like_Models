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
	template<class Derived>
    class QuerySet;

    class Field;

	template<class Derived>
    class Model;

	class BaseException
	{
	protected:
		std::string CAUSE;

	public:
		BaseException(std::string why)
		{
			CAUSE = why;
		}		

		std::string what()
		{
			return CAUSE;
		}
	};

	class MultipleObjectsReturned 	: public BaseException {};
	class ObjectDoesNotExist 		: public BaseException {};

	std::string getime(void);


///////////////////QUERYSET//////////////////////////////

	template<class Derived>
	class QuerySet
	{
    private:
        std::stringstream SQLquery;
		Derived* __table;
		std::string __pk;

    public:
        QuerySet(std::string pk, std::string sql)
		{
			__table = new Derived();
			SQLquery << sql.substr(0, sql.size() - sql.substr(sql.find(";"), sql.size()).size());
			this->__pk = pk;
		}
		~QuerySet()
		{
			delete __table;
		}
		
		std::string pk(void);
/*
        void set(std::string column, bool           content);
        void set(std::string column, std::string    content);
        void set(std::string column, float          content);*/
        void set(std::string column, int            content);
//        void set(std::string column, QuerySet&      content);

//        void remove(void);

		void save(void);
	};

	template<typename value_type, class Derived>
	class SingleSet : protected QuerySet<Derived>
	{
	private:
		typename DLCPP_MAP(value_type) value;

	public:
		DLCPP_MAP(value_type)& getv(void) { return value; }

		//OPERATORS
		value_type                operator[](const std::string& index){ return value[index]; }
		SingleSet<value_type, Derived>&    operator=(const SingleSet<value_type, Derived>& x){ value = x.getv(); return *this;}
		SingleSet<value_type, Derived>&    operator=(const typename DLCPP_MAP(value_type)& x){ value = x; return *this;}

		//ITERATORS
		typename DLCPP_MAP_ITER(value_type) begin(void)   { return value.begin(); }
		typename DLCPP_MAP_ITER(value_type) end(void) 	  { return value.end();   }
		typename DLCPP_MAP_ITER(value_type) rbegin(void) { return value.rbegin();}
		typename DLCPP_MAP_ITER(value_type) rend(void)   { return value.rend();  }

		//CAPACITY
		bool 			empty(void)     const { return value.empty();   }
		unsigned int 	size(void)      const { return value.size();    }
		unsigned int 	max_size(void)  const { return value.max_size();}

		//INSERT
		std::pair<typename DLCPP_MAP_ITER(value_type), value_type> insert(const value_type& val){ return value.insert(val); }
		typename DLCPP_MAP_ITER(value_type)                   insert(typename DLCPP_MAP_ITER(value_type) pos, const value_type& val){ return value.insert(pos, val); }
		// range insert is not currently supported

		//ERASE
		void 			erase(typename DLCPP_MAP_ITER(value_type) pos){ return value.erase(pos); }
		unsigned int 	erase(const std::string& k){ return value.erase(k); }
		void			erase(typename DLCPP_MAP_ITER(value_type) first, typename DLCPP_MAP_ITER(value_type) last){ return value.erase(first, last); }
		void 			clear(void){ value.clear(); }

		// Observers not currently supported

		//OPERATIONS
		typename DLCPP_MAP_ITER(value_type)			find(const std::string& k){ return value.find(k); }
		unsigned int 			count(const std::string& k) const{ return value.count(k); }
		typename DLCPP_MAP_ITER(value_type)			lower_bound(const std::string& k){ return value.lower_bound(k); }
		typename DLCPP_MAP_ITER(value_type)			upper_bound(const std::string& k){ return value.upper_bound(k); }
		// equal_range not currently supported

		// get_allocator not currently supported
	};
/*
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
	public:
		bool __notNull;
		bool __blank;
		std::list<std::list< std::string> > __choices;
		std::string __db_column;
		bool __db_index;
		bool __primary_key;
		bool __unique;

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
		sqlite3* db;
		static std::pair<std::string, Field*>* pk;

		~Model(void){ 
			int err;

			err = SQLITE_OK;//sqlite3_close(db); 
			db = NULL;

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

				log_file.close();
			}
		}
		Model() throw (std::runtime_error)
		{
			int err;
			std::string classname;

			// REF INSTANTIATION
			classname = __PRETTY_FUNCTION__;
			classname = classname.substr(classname.find("Derived = ")+10, classname.size());
			classname = classname.substr(0, classname.size()-1);

			ref = classname;

			// DB INSTANTIATION
			if(!db)
				err = sqlite3_open(DLCPP_PATH_2_DB, &db);
			else
				err = SQLITE_OK;

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
					log_file << "[    models::Model()    ][" << getime() <<"] CONNECTION TO " << ref << " SUCCESSFUL." << std::endl;
				}
				log_file.close();
			}
			else
			{
				if (err)
					throw std::runtime_error("Connection error.");
			}
		}

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
*/
        QuerySet<Derived>* insert(void);
    };

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
			if(it->second.__primary_key)
				pk = new std::pair<std::string, Field*>(it->first, &(it->second));
			++it;
			if(it != column.end())
				createStt << ",";
			createStt << "\n";
		}

		if(!pk)
			pk = new std::pair<std::string, Field*>(std::string("ROWID"), NULL);
			
		createStt << ");";

		temp = createStt.str();

		err = sqlite3_exec(m->db, temp.c_str(), NULL, NULL, zErrMsg);

		if(DLCPP_VERBOSE_LEVEL)
		{
			std::fstream log_file;
			log_file.open(DLCPP_LOGFILE, std::fstream::out | std::fstream::app);


			if(err != SQLITE_OK)
			{
				if(DLCPP_VERBOSE_LEVEL == 2)
					log_file << "[models::Model::CREATE()][" << getime() << "] ERROR WHILE CREATING TABLE. SQLITE ERROR CODE " << sqlite3_errcode(m->db) << *zErrMsg << ".\n";
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

		rc = sqlite3_exec(m->db, temp.c_str(), NULL, NULL, zErrMsg);

		if(DLCPP_VERBOSE_LEVEL)
		{
			std::fstream log_file;
			log_file.open(DLCPP_LOGFILE, std::fstream::out | std::fstream::app);

			if(rc != SQLITE_OK)
			{
				/*if(DLCPP_VERBOSE_LEVEL == 2)
					log_file << "[ models::Model::DROP() ][" + getime() << "] ERROR WHILE DROPPING TABLE. SQLITE ERROR CODE " << sqlite3_errcode(m->db) << ": " << *zErrMsg <<"\n";
				else
					log_file << "DROP CLAUSE FAILEDn\n";

				log_file << "[ models::Model::DROP() ][" + getime() << "] SQL:";*/

				sqlite3_free(zErrMsg);
			}

			log_file << "\n========\n" << temp << "\n========\n" <<std::endl;

			log_file.close();
		}

		delete m;
	}

	template<class Derived>
	QuerySet<Derived>* Model<Derived>::insert()
	{
		return new QuerySet<Derived>(pk->first, "INSERT INTO () VALUES ();" );
	}


	template<class Derived>
	std::pair<std::string, Field*>* Model<Derived>::pk = NULL;

	template<class Derived>
	void QuerySet<Derived>::set(std::string column, int content)
	{
		DLCPP_MAP(Field) dm;
		std::string sqlaux, tname;
		size_t insert_pos;
		bool first;

		__table->retrieve(dm, tname);

		sqlaux = SQLquery.str();

		first = !(sqlaux.substr(sqlaux.find("("), 3).find(")") == std::string::npos);

		if(sqlaux.substr(0, 6) == "INSERT") // INSERT INTO
		{
			insert_pos = sqlaux.find(")");
			sqlaux.insert(insert_pos, std::string(first?" ":", ") + column);
			SQLquery.seekp(0);
			SQLquery << sqlaux.substr(0, sqlaux.size()-(first?1:2));
			SQLquery << (first?" ":", ") << content << " );";
		} else  if(sqlaux.substr(0,6) == "UPDATE") { // UPDATE SET
			insert_pos = sqlaux.rfind(" WHERE")+6;
			sqlaux.insert(insert_pos, std::string(first?" ":" AND") + dm[column].sql);

			insert_pos = sqlaux.find(" WHERE");
			sqlaux.insert(insert_pos, std::string(first?" ":", ") + column + std::string("="));
			insert_pos = sqlaux.find(" WHERE");

			SQLquery.clear();
			SQLquery << sqlaux;
			SQLquery.seekp(insert_pos);
			SQLquery << content << sqlaux.substr(insert_pos, sqlaux.size());
		}
	}

	template<class Derived>
	void QuerySet<Derived>::save()
	{
		int rc;
		char** zErrMsg;
		std::string temp = SQLquery.str();
		
		rc = SQLITE_OK;//sqlite3_exec(__table->db, temp.c_str(), NULL, NULL, zErrMsg);

		if(DLCPP_VERBOSE_LEVEL)
		{
			std::fstream log_file;
			log_file.open(DLCPP_LOGFILE, std::fstream::out | std::fstream::app);

			if(rc != SQLITE_OK)
			{
				if(DLCPP_VERBOSE_LEVEL == 2)
					log_file << "[models::QuerySet<>.save][" << getime() << "] ERROR WHILE MODIFYING TABLE. SQLITE ERROR CODE " << sqlite3_errcode(__table->db) << ": " << *zErrMsg <<"\n";
				else
					log_file << "ERROR WHILE MODIFYING TABLE.\n";

				log_file << "[models::QuerySet<>.save][" << getime() << "] SQL:";

				sqlite3_free(zErrMsg);
			}

			log_file << "\n========\n" << temp << "\n========\n" <<std::endl;

			log_file.close();
		}

		delete this;
	}
};
#endif