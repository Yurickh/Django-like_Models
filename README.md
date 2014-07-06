# Django-Like models in C++

## Functionality

The aim of this project is to develop a solid c++ framework that emulates the way that Django handles Models.

To further reading about Django's way of dealing with database's tables, follow:
https://docs.djangoproject.com/en/dev/topics/db/models/

The main point of using this framework is to make SQL queries transparent to the programmer.

## Authors

For now, the only programmer responsible for the project is Yurick Hauschild, graduate student at UnB (University of Brasilia).

## Usage

### CREATE TABLE

In order to create a table, you must:

1. Create a subclass of models::Model, as follows:

    `DLCPP_NEW_MODEL(ExampleModel);`

2. Create the columns as models::Field:


    ```c++

    DLCPP_NEW_MODEL(ExampleModel)
    {
        public: ExampleModel()
        {
            column["id"] = models::IntegerField().primary_key();
            column["income"] = models::FloatField().standard(12.4).null(True);

            CREATE();
        }
    };
    ```

### DROP TABLE

Use the DROP() static method:

```c++
ExampleModel::DROP();
```

### Retrieve data (SELECT)

To retrieve a set of data, use the .filter() method. e.g.:

```c++
models::Model* m;
models::QuerySet* ret;

m = new ExampleModel;
ret = m->filter("income__gt", 10); // selects entries that have income > 10

// in this case, returns a STL list 
// with a STL map<string, int> defined by 
// column_name => column_value.

// Shall print the ID of all entries returned.
for(DLCPP_LIST::iterator i = ret->begin(); i != ret->end(); ++i)
    cout << *i["id"];
```

To retrieve a single instance of data, it's better to use the .get() method. e.g.:

```c++
models::Model* m;
models::QuerySet* ret;

m = new ExampleModel;
ret = m->get("id__eq", 5);

// in this case, returns a single
// STL map<string, int> 
// defined by column_name => column_value.

ret["id"] // shall return 5
```

### INSERT data

To insert new data in a table, do as follows:

```c++
models::Model* m;
models::QuerySet* n;

m = new ExampleModel;
n = m->insert();

n.set("income", 10000);
n.set("id", 7);

n.save();
```

### UPDATE data

To update data, you must retrieve it first:

```c++
models::Model* m;
models::QuerySet* n;

m = new ExampleModel;
n = m->get("id__eq", 7);

n.set("income", 20);

n.save();
```

### DELETE data

Like an update, you must retrieve the data first:

```c++
models::Model* m;
models::QuerySet* n;

m = new ExampleModel;
n = m->get("id__eq", 7);

n->remove();

n.save();
```
