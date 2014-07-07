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

#define DLCPP_LIST 	std::list<std::map<std::string, std::string> >
#define DLCPP_MAP	std::map<std::string, std::string>
#define DLCPP_FMAP  std::map<std::string, models::Field>

#define DLCPP_LIST_ITER std::list<std::map<std::string, std::string> >::iterator
#define DLCPP_MAP_ITER  std::map<std::string, std::string>::iterator
#define DLCPP_FMAP_ITER std::map<std::string, Field>::iterator

//For correctely CRTPing Models.
#define DLCPP_NEW_MODEL(Type) class Type : public models::Model<Type>

namespace models
{
	std::string tostring(int value);
	std::string tostring(float value);
	std::string tostring(bool value);
    std::string tostring(std::string value);

    int cb_single(void* p_data, int num_fields, char**p_fields, char**p_col_names);
    int cb_multiple(void* p_data, int num_fields, char**p_fields, char **p_col_names);

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
        BaseException(std::string why="")
        {
            CAUSE = why;
        }

        std::string what()
        {
            return CAUSE;
        }
    };

    class MultipleObjectsReturned : public BaseException 
    {
    public: 
        MultipleObjectsReturned(std::string why="") : BaseException(why)
        {}
    };

	class ObjectDoesNotExist : public BaseException
    {
    public: 
        ObjectDoesNotExist(std::string why="") : BaseException(why)
        {}
    };

    std::string getime(void);


///////////////////QUERYSET//////////////////////////////

    template<class Derived>
    class QuerySet
    {
    protected:
        std::stringstream SQLquery;
        Model<Derived>* __table;
        std::string __pk;

        template<typename intype>
        void set_g(std::string column, intype content);

    public:
        QuerySet(Model<Derived>* d, std::string pk, std::string sql)
        {
            __table = d;
            SQLquery << sql.substr(0, sql.size()+1 - sql.substr(sql.find(";"), sql.size()).size());

            std::cout << "ULTRAPK:" << pk << std::endl;

            this->__pk = pk;
        }
		
        std::string pk(void);

        void set(std::string column, bool           content);
        void set(std::string column, std::string    content);
        void set(std::string column, float          content);
        void set(std::string column, int            content);
        void set(std::string column, const char*    content);
        //void set(std::string column, QuerySet&      content);

        void remove(void);

        void save(void);
	};

    template<class Derived>
    class SingleSet : public QuerySet<Derived>
    {
    private:
        typename DLCPP_MAP value;


    public:
        SingleSet(Model<Derived>* __table, std::string pk, std::string sql) throw (MultipleObjectsReturned, ObjectDoesNotExist) : QuerySet<Derived>(__table, pk, sql)
        {
            std::string temp = this->SQLquery.str();
            size_t pos;
            int rc;
            char* zErrMsg;

            rc = sqlite3_exec(this->__table->db, temp.c_str(), cb_single, &value, &zErrMsg);

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
                else if(value.empty())
                {
                    if(DLCPP_VERBOSE_LEVEL == 2)
                        log_file << "[models::SingleSet<>.get][" << getime() << "] ERROR WHILE RETRIEVING DATA FROM TABLE. NO DATA FOUND.\n";
                    else log_file << "SELECT CLAUSE FAILED.\n";

                    log_file << "[models::SingleSet<>get][" << getime() << "] SQL:";
                }

                log_file << "\n========\n" << temp << "\n========\n" <<std::endl;

                log_file.close();
            }

            if(value.empty())
                throw ObjectDoesNotExist();
            if(rc==SQLITE_ABORT)
                throw MultipleObjectsReturned();
        }

        DLCPP_MAP getv(void){ return value; }

        //OPERATORS
        std::string         operator[](const std::string& index){ return value[index]; }
        SingleSet<Derived>& operator= (const SingleSet<Derived>& x){ value = x.getv(); return *this;}
        SingleSet<Derived>& operator= (const typename DLCPP_MAP& x){ value = x; return *this;}

        //ITERATORS
        typename DLCPP_MAP_ITER begin(void)     { return value.begin(); }
        typename DLCPP_MAP_ITER end(void)       { return value.end();   }
        typename DLCPP_MAP_ITER rbegin(void)    { return value.rbegin();}
        typename DLCPP_MAP_ITER rend(void)      { return value.rend();  }

        //CAPACITY
        bool            empty(void)     const { return value.empty();   }
        unsigned int    size(void)      const { return value.size();    }
        unsigned int    max_size(void)  const { return value.max_size();}

        //INSERT
        std::pair<typename DLCPP_MAP_ITER, std::string> insert(const std::string& val){ return value.insert(val); }
        typename DLCPP_MAP_ITER                         insert(typename DLCPP_MAP_ITER pos, const std::string& val){ return value.insert(pos, val); }
        // range insert is not currently supported

        //ERASE
        void            erase(typename DLCPP_MAP_ITER pos){ return value.erase(pos); }
        unsigned int    erase(const std::string& k){ return value.erase(k); }
        void            erase(typename DLCPP_MAP_ITER first, typename DLCPP_MAP_ITER last){ return value.erase(first, last); }
        void            clear(void){ value.clear(); }

        // Observers not currently supported

        //OPERATIONS
        typename DLCPP_MAP_ITER find(const std::string& k){ return value.find(k); }
        unsigned int            count(const std::string& k) const{ return value.count(k); }
        typename DLCPP_MAP_ITER lower_bound(const std::string& k){ return value.lower_bound(k); }
        typename DLCPP_MAP_ITER upper_bound(const std::string& k){ return value.upper_bound(k); }
        // equal_range not currently supported

        // get_allocator not currently supported
    };

    template<class Derived>
    class MultipleSet : public QuerySet<Derived>
    {
    private:
        DLCPP_LIST value;

    public:
        MultipleSet(Model<Derived>* table, std::string pk, std::string sql) : QuerySet<Derived>(table, pk, sql) 
        {
            std::string temp = this->SQLquery.str();
            size_t pos;
            int rc;
            char* zErrMsg;

            rc = sqlite3_exec(this->__table->db, temp.c_str(), cb_multiple, &value, &zErrMsg);

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
                else if(value.empty())
                {
                    if(DLCPP_VERBOSE_LEVEL == 2)
                        log_file << "[models::SingleSet<>.get][" << getime() << "] ERROR WHILE RETRIEVING DATA FROM TABLE. NO DATA FOUND.\n";
                    else log_file << "SELECT CLAUSE FAILED.\n";

                    log_file << "[models::SingleSet<>get][" << getime() << "] SQL:";
                }

                log_file << "\n========\n" << temp << ";\n========\n" <<std::endl;

                log_file.close();
            }

            if(value.empty())
                throw ObjectDoesNotExist();
        }

        DLCPP_LIST getv(void) { return value; }

        //OPERATORS
        MultipleSet<Derived>&   operator=(const MultipleSet<Derived>& x){ value = x.getv(); }
        MultipleSet<Derived>&   operator=(const DLCPP_LIST& x){ value = x; }

        //ITERATORS
        DLCPP_LIST_ITER begin(void){    return value.begin();   }
        DLCPP_LIST_ITER end(void){      return value.end();     }
        DLCPP_LIST_ITER rbegin(void){   return value.rbegin();  }
        DLCPP_LIST_ITER rend(void){     return value.rend();    }

        //CAPACITY
        bool            empty(void)     const{ return value.empty();    }
        unsigned int    size(void)      const{ return value.size();     }
        unsigned int    max_size(void)  const{ return value.max_size(); }

        //ELEMENT ACCESS
        DLCPP_MAP& front(void){ return value.front(); }
        DLCPP_MAP& back(void) { return value.back();  }

        //MODIFIERS
        void push_front(const std::string& val) { value.push_front(val);}
        void pop_front(void)                    { value.pop_front();    }
        void push_back(const std::string& val)  { value.push_back(val); }
        void pop_back(void)                     { value.pop_back();     }

        //INSERT
        DLCPP_LIST_ITER insert(DLCPP_LIST_ITER position, const std::string& val){ return value.insert(position, val); }
        void            insert(DLCPP_LIST_ITER position, unsigned int n, const std::string& val){ value.insert(position, n, val); }
        //range insert not currently supported

        //ERASE
        DLCPP_LIST_ITER erase(DLCPP_LIST_ITER position){ return value.erase(position); }
        DLCPP_LIST_ITER erase(DLCPP_LIST_ITER first, DLCPP_LIST_ITER last){ return value.erase(first, last); }
        void            clear(void){ value.clear(); }

        //OPERATIONS
        void splice(DLCPP_LIST_ITER position, DLCPP_LIST& x){ value.splice(position, x); }
        void splice(DLCPP_LIST_ITER position, DLCPP_LIST& x, DLCPP_LIST_ITER i){ value.splice(position, x, i); }
        void splice(DLCPP_LIST_ITER position, DLCPP_LIST& x, DLCPP_LIST_ITER first, DLCPP_LIST_ITER last){ value.splice(position, x, first, last); }
        void remove(const std::string& val){ value.remove(val); }
        void merge(DLCPP_LIST& x){ value.merge(x); }
        void sort(void){ value.sort(); }
        void reverse(void){ value.reverse(); }
        // remove_if and unique not currently supported

        // get_allocator not currently supported

        void save(void);
    };

////////////////////FIELDS////////////////////////////////

	/** Classe responsável por modelar colunas da tabela.
	*/
    class Field
    {
    public:
        bool __notNull;
        std::string __db_column;
        bool __db_index;
        bool __primary_key;
        bool __unique;

        Field(){}

        std::string sql;

        Field& null(bool null);
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
        BooleanField(void);
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
        CharField(void);
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
        FloatField(void);
        FloatField& size_d(int, int);
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
        ForeignKey& to_field(std::string);
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
        DLCPP_FMAP column; // Columns
        std::string ref; // TableName

        template<class retClass, typename argClass>
        retClass* select(std::string, argClass input);

    public:
        sqlite3* db;
        std::pair<std::string, Field>* pk;

        ~Model(void)
        { 
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

            delete pk;
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

        void CREATE(void);

        static void DROP(void);

        void retrieve(DLCPP_FMAP&, std::string&);

        MultipleSet<Derived>* filter(std::string, bool);
        MultipleSet<Derived>* filter(std::string, std::string);
        MultipleSet<Derived>* filter(std::string, float);
        MultipleSet<Derived>* filter(std::string, int);
        MultipleSet<Derived>* filter(std::string, const char*);

        SingleSet<Derived>* get(std::string, bool);
        SingleSet<Derived>* get(std::string, std::string);
        SingleSet<Derived>* get(std::string, float);
        SingleSet<Derived>* get(std::string, int);
        SingleSet<Derived>* get(std::string, const char*);
        

        QuerySet<Derived>* insert(void);
    };

    #include "DLCpp.inl" // For inline definitions
	
};
#endif