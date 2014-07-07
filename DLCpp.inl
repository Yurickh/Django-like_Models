//////////////////////// DEFINITIONS /////////////////////////////

  /////////////////////
 // Model           //
/////////////////////

    template<class Derived>
    void Model<Derived>::retrieve(DLCPP_FMAP &a, std::string &c)
    {
        a = this->column;
        c = this->ref;
    }

    template<class Derived>
    void Model<Derived>::CREATE()
    {
        DLCPP_FMAP_ITER it;
        std::stringstream createStt;
        std::string temp;
        char** zErrMsg;
        int err;

        std::cout << "Model<>.CREATE();" << std::endl;

        createStt << "CREATE TABLE IF NOT EXISTS " << ref << "(\n";

        this->pk = NULL;

        for(it = column.begin(); it != column.end();)
        {
            createStt << it->first << " " << it->second.sql;
            if(it->second.__primary_key)
                this->pk = new std::pair<std::string, Field>(it->first, it->second);
            ++it;
            if(it != column.end())
                createStt << ",";
            createStt << "\n";
        }

        if(!(this->pk))
            this->pk = new std::pair<std::string, Field>("ROWID", Field());

        std::cout << "FIRSTPK:" << this->pk->first << std::endl;
			
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
        DLCPP_FMAP f;
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
        return select<SingleSet<Derived>, int>(requestr, value);
    }


    template<class Derived>
    SingleSet<Derived>* Model<Derived>::get(std::string requestr, bool value)
    {
        return select<SingleSet<Derived>, bool>(requestr, value);
    }


    template<class Derived>
    SingleSet<Derived>* Model<Derived>::get(std::string requestr, float value)
    {
        return select<SingleSet<Derived>, float>(requestr, value);
    }


    template<class Derived>
    SingleSet<Derived>* Model<Derived>::get(std::string requestr, std::string value)
    {
        return select<SingleSet<Derived>, std::string>(requestr, value);
    }

    template<class Derived>
    SingleSet<Derived>* Model<Derived>::get(std::string requestr, const char* value)
    {
        return select<SingleSet<Derived>, std::string>(requestr, std::string(value));
    }

    template<class Derived>
    MultipleSet<Derived>* Model<Derived>::filter(std::string requestr, int value)
    {
        return select<MultipleSet<Derived>, int>(requestr, value);
    }

    template<class Derived>
    MultipleSet<Derived>* Model<Derived>::filter(std::string requestr, bool value)
    {
        return select<MultipleSet<Derived>, bool>(requestr, value);
    }

    template<class Derived>
    MultipleSet<Derived>* Model<Derived>::filter(std::string requestr, float value)
    {
        return select<MultipleSet<Derived>, float>(requestr, value);
    }

    template<class Derived>
    MultipleSet<Derived>* Model<Derived>::filter(std::string requestr, std::string value)
    {
        return select<MultipleSet<Derived>, std::string>(requestr, value);
    }

    template<class Derived>
    template<class retClass, typename argClass>
    retClass* Model<Derived>::select(std::string requestr, argClass value)
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
        temp.insert(pos, "'" + token + "'");

        pos = temp.find(" FROM");
        temp.insert(pos, " * ");

        pos = temp.find(" WHERE");

        for(it=rqlist.begin(); it!=rqlist.end(); ++it)
        {
            temp.insert(pos, " NATURAL JOIN "); pos+=14;
            temp.insert(pos, *it); pos+=it->size();
        }

        std::cout << "UPK:" << this->pk->first << std::endl;

        return new retClass(this, this->pk->first, temp);
    } 

  /////////////////////////
 // QUERYSET            //
/////////////////////////

    template<class Derived>
    template<typename intype>
    void QuerySet<Derived>::set_g(std::string column, intype content)
    {
        DLCPP_FMAP dm;
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
            insert_pos = sqlaux.rfind(")");
            sqlaux.insert(insert_pos, std::string(first?" '":", '"));
            insert_pos = sqlaux.rfind(")");
            sqlaux.insert(insert_pos, tostring(content) + "'");
            SQLquery.seekp(0);
            SQLquery << sqlaux << (first?";":"");
        }
    }

    template<class Derived>
    void QuerySet<Derived>::set(std::string column, int content)
    {
        this->set_g<int>(column, content);
    }

    template<class Derived>
    void QuerySet<Derived>::set(std::string column, float content)
    {
        this->set_g<float>(column, content);
    }

    template<class Derived>
    void QuerySet<Derived>::set(std::string column, std::string content)
    {
        this->set_g<std::string>(column, content);
    }

    template<class Derived>
    void QuerySet<Derived>::set(std::string column, bool content)
    {
        this->set_g<bool>(column, content);
    }

    template<class Derived>
    void QuerySet<Derived>::set(std::string column, const char* content)
    {
        this->set_g<std::string>(column, std::string(content));
    }

    template<class Derived>
    void QuerySet<Derived>::save()
    {
        int rc;
        char* zErrMsg;
        std::string temp = SQLquery.str();

        std::cout << "QuerySet<>.save();" << std::endl;
		
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
    void QuerySet<Derived>::remove()
    {
        int rc;
        char* zErrMsg;
        std::string temp = SQLquery.str();

        temp.replace(temp.find("SELECT *"), std::string("SELECT *").size(), "DELETE");


        std::cout << "QuerySet<>.delete();" << std::endl;

        rc = sqlite3_exec(__table->db, temp.c_str(), NULL, NULL, &zErrMsg);

        if(DLCPP_VERBOSE_LEVEL)
        {
            std::fstream log_file;
            log_file.open(DLCPP_LOGFILE, std::fstream::out | std::fstream::app);

            if(rc != SQLITE_OK)
            {
                if(DLCPP_VERBOSE_LEVEL == 2)
                    log_file << "[models::QuerySet<>.del ][" << getime() << "] ERROR WHILE DELETING DATA. SQLITE ERROR CODE " << sqlite3_errcode(__table->db) << ": " << zErrMsg <<"\n";
                else
                    log_file << "ERROR WHILE DELETING DATA.\n";

                log_file << "[models::QuerySet<>.del ][" << getime() << "] SQL:";
            }

            log_file << "\n========\n" << temp << "\n========\n" <<std::endl;

            log_file.close();
        }

        delete this;
    }

    template<class Derived>
    void MultipleSet<Derived>::save()
    {
        DLCPP_LIST_ITER it;
        DLCPP_MAP_ITER jt;

        std::list<std::string> lt;

        std::cout << "MultipleSet<Derived>::save();" << std::endl;

        for(it=value.begin(); it!=value.end(); ++it) // it is now line of resultset
        {
            std::string temp = this->SQLquery.str();
            size_t posi, posf, posk;

            std::cout << "CHECK1" << std::endl;
            std::cout << temp << std::endl;
            std::cout << "pk:" << this->__pk << ";" << (*it)[this->__pk] << std::endl;

            temp = temp.substr(0, temp.find(";"));
            posi = temp.find("SELECT *  FROM");
            temp.replace(posi, 14, "UPDATE"); posi+=6;
            
            posf = temp.find("WHERE");
            temp.insert(posf, "SET "); posf+=4;

            posk = temp.size()-1;
            temp.insert(posk, " AND " + this->__pk + "=" + (*it)[this->__pk]);

            std::cout << "CHECK2" << std::endl;
            std::cout << temp << std::endl;

            for(jt=it->begin(); jt!=it->end(); ++jt) // jt is now column of it
            {
                if(jt->first == this->__pk) continue;
    
                bool first = (temp.substr(0, temp.find("WHERE")).find("=") == std::string::npos);
                temp.insert(posf, (first?" ":", ") + jt->first + "=" + jt->second);
            }

            std::cout << "CHECK3" << std::endl;
            std::cout << temp << std::endl;

            lt.push_back(temp);
        }

        for(std::list<std::string>::iterator it=lt.begin(); it!=lt.end(); ++it)
            std::cout << (*it) << std::endl;
    }





