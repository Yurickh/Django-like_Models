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
	std::string tostring(int value);
	std::string tostring(float value);
	std::string tostring(bool value);

	int cb_single(void* p_data, int num_fields, char**p_fields, char**p_col_names)
	{
		std::string* temp = static_cast<std::string*>(p_data);

		if(temp != NULL)
			return 1;
		
		*temp = *p_fields;

		return 0;
	}

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
    protected:
        std::stringstream SQLquery;
		Model<Derived>* __table;
		std::string __pk;

    public:
        QuerySet(Model<Derived>* d, std::string pk, std::string sql)
		{
			__table = d;
			SQLquery << sql.substr(0, sql.size() - sql.substr(sql.find(";"), sql.size()).size());
			this->__pk = pk;
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

	template<class Derived>
	class SingleSet : public QuerySet<Derived>
	{
	private:
		typename DLCPP_MAP(std::string) value;

	public:
		SingleSet(Model<Derived>* table, std::string pk, std::string sql) : QuerySet<Derived>(table, pk, sql){}

		//OPERATORS
		std::string	operator[](const std::string& index);

		//ERASE
		unsigned int 	erase(const std::string& k){ return value.erase(k); }
		void 			clear(void){ value.clear(); }

		void save(void);
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
		std::pair<std::string, Field*>* pk;

		~Model(void){ 
			int err;

			err = sqlite3_close(db); 
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

			pk = NULL;

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

		void CREATE(void);

        static void DROP(void);

		void retrieve(DLCPP_MAP(Field)&, std::string&);

        /*QuerySet* filter(std::stringstream requestr, bool);
        QuerySet* filter(std::stringstream requestr, std::string);
        QuerySet* filter(std::stringstream requestr, float);
        QuerySet* filter(std::stringstream requestr, int);

        QuerySet* get(std::string requestr, bool);
        QuerySet* get(std::string requestr, std::string);
        QuerySet* get(std::string requestr, float);*/
        SingleSet<Derived>* get(std::string requestr, int value);

        QuerySet<Derived>* insert(void);
    };

///////////////////////// DEFINITIONS /////////////////////////////

  /////////////////////
 // Model			//
/////////////////////

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
		std::stringstream createStt;
		std::string temp;
		char** zErrMsg;
		int err;

		std::cout << "Model<>.CREATE();" << std::endl;

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

		err = sqlite3_exec(this->db, temp.c_str(), NULL, NULL, zErrMsg);

		if(DLCPP_VERBOSE_LEVEL)
		{
			std::fstream log_file;
			log_file.open(DLCPP_LOGFILE, std::fstream::out | std::fstream::app);


			if(err != SQLITE_OK)
			{
				if(DLCPP_VERBOSE_LEVEL == 2)
					log_file << "[models::Model::CREATE()][" << getime() << "] ERROR WHILE CREATING TABLE. SQLITE ERROR CODE " << sqlite3_errcode(this->db) << *zErrMsg << ".\n";
				else
					log_file << "CREATE CLAUSE FAILED.\n";

				log_file << "[models::Model::CREATE()][" << getime() << "] SQL:";
			}
			
			log_file << "\n========\n" << temp << "\n========\n" <<std::endl;
			
			log_file.close();
		}

		std::cout << temp << std::endl;
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

		std::cout << "Model<>::DROP();" << std::endl;

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
				if(DLCPP_VERBOSE_LEVEL == 2)
					log_file << "[ models::Model::DROP() ][" + getime() << "] ERROR WHILE DROPPING TABLE. SQLITE ERROR CODE " << sqlite3_errcode(m->db) << ": " << *zErrMsg <<"\n";
				else
					log_file << "DROP CLAUSE FAILEDn\n";

				log_file << "[ models::Model::DROP() ][" + getime() << "] SQL:";
			}

			log_file << "\n========\n" << temp << "\n========\n" <<std::endl;

			log_file.close();
		}

		std::cout << temp << std::endl;

		delete m;
	}

	template<class Derived>
	QuerySet<Derived>* Model<Derived>::insert()
	{
		std::string sql = "INSERT INTO ";
		sql.append(ref);
		sql.append(" () VALUES ();");
		return new QuerySet<Derived>(this, pk->first, sql );
	}

	template<class Derived>
	SingleSet<Derived>* Model<Derived>::get(std::string requestr, int value)
	{
		std::list<std::string> rqlist;
		std::list<std::string>::iterator it;
		std::string token, temp;
		size_t pos = 0;

		std::stringstream SQLquery;

		//EXPLODE STRING (__) INTO rqlist
		while((pos = requestr.find("__")) != std::string::npos)
		{
			token = requestr.substr(0, pos);
			rqlist.push_front(token);
			requestr.erase(0, pos+2);
		}
		token = requestr.substr(0, requestr.size());
		rqlist.push_front(token);
		//END EXPLODE -> rqlist contains exploded string

		SQLquery << "SELECT FROM " << ref << " WHERE ;";

		token = rqlist.front(); // get type of access
		rqlist.pop_front();

		temp = SQLquery.str();
		pos = temp.find("WHERE")+6;

		temp.insert(pos, (token == "eq") ? "= " :
						 (token == "gt") ? "> " :
						 (token == "lt") ? "< " :
						 (token == "ge") ? ">=" :
						 (token == "le") ? "<=" :
						 (token == "ne") ? "!=" :
							"");

		temp.insert(pos, rqlist.front());
		pos += rqlist.front().size()+2;
		rqlist.pop_front();

		token = tostring(value);
		temp.insert(pos, token);

		pos = temp.find(" FROM");
		temp.insert(pos, " * ");

		pos = temp.find(" WHERE");

		for(it=rqlist.begin(); it!=rqlist.end(); ++it)
		{
			temp.insert(pos, " NATURAL JOIN "); pos+=14;
			temp.insert(pos, *it); pos+=it->size();
		}

		return new SingleSet<Derived>(this, pk->first, temp);
	}

  /////////////////////////
 // QUERYSET			//
/////////////////////////

	template<class Derived>
	void QuerySet<Derived>::set(std::string column, int content)
	{
		DLCPP_MAP(Field) dm;
		std::string sqlaux, tname;
		size_t insert_pos;
		bool first;

		std::cout << "QuerySet<>.set();" << std::endl;

		__table->retrieve(dm, tname);

		sqlaux = SQLquery.str();

		first = !(sqlaux.substr(sqlaux.find("("), 3).find(")") == std::string::npos);

		if(sqlaux.substr(0, 6) == "INSERT") // INSERT INTO
		{
			insert_pos = sqlaux.find(")");
			sqlaux.insert(insert_pos, std::string(first?" ":", ") + column);
			SQLquery.seekp(0);
			SQLquery << sqlaux.substr(0, sqlaux.size()-(first?1:2));
			SQLquery << (first?" '":", '") << content << "' );";
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
		char* zErrMsg;
		std::string temp = SQLquery.str();

		std::cout << "QuerySet<>.save();" << std::endl;
		std::cout << temp << std::endl;
		
		rc = sqlite3_exec(__table->db, temp.c_str(), NULL, NULL, &zErrMsg);

		if(DLCPP_VERBOSE_LEVEL)
		{
			std::fstream log_file;
			log_file.open(DLCPP_LOGFILE, std::fstream::out | std::fstream::app);

			if(rc != SQLITE_OK)
			{
				if(DLCPP_VERBOSE_LEVEL == 2)
					log_file << "[models::QuerySet<>.save][" << getime() << "] ERROR WHILE MODIFYING TABLE. SQLITE ERROR CODE " << sqlite3_errcode(__table->db) << ": " << zErrMsg <<"\n";
				else
					log_file << "ERROR WHILE MODIFYING TABLE.\n";

				log_file << "[models::QuerySet<>.save][" << getime() << "] SQL:";
			}

			log_file << "\n========\n" << temp << "\n========\n" <<std::endl;

			log_file.close();
		}

		delete this;
	}

	template<class Derived>
	std::string	SingleSet<Derived>::operator[](const std::string& index) throw (MultipleObjectsReturned, ObjectDoesNotExist)
	{
		std::string temp = this->SQLquery.str();
		size_t pos;
		int rc;
		char* zErrMsg;
		std::string retvalue;
	
		pos = temp.find("*");
		temp.replace(pos, 1, index);

		rc = sqlite3_exec(this->__table->db, temp.c_str(), cb_single, &retvalue, &zErrMsg);

		if(DLCPP_VERBOSE_LEVEL)
		{
			std::fstream log_file;
			log_file.open(DLCPP_LOGFILE, std::fstream::out | std::fstream::app);

			if(rc != SQLITE_OK)
			{
				if(DLCPP_VERBOSE_LEVEL == 2)
					log_file << "[models::SingleSet<>.get][" << getime() << "] ERROR WHILE RETRIEVING DATA FROM TABLE. SQLITE ERROR CODE " << sqlite3_errcode(this->__table->db) << ": " << zErrMsg <<"\n";
				else
					log_file << "SELECT CLAUSE FAILED.\n";

				log_file << "[models::SingleSet<>.get][" << getime() << "] SQL:";
			}

			log_file << "\n========\n" << temp << "\n========\n" <<std::endl;

			log_file.close();
		}

		

		return retvalue;
	}

	
};
#endif