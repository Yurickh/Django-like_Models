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

1. Create a subclass of models.Model, as follows:

	class ExampleModel :: public models.Model;

2. Create the columns as models.Field:

    class ExampleModel :: public models.Model
    {
        models.Field* id;
        models.Field* income;
    };

3. Initialize each models.Field with its respective constraints:

    class ExampleModel : public models.Model
    {
        models.Field* id;
        models.Field* income;

        ExampleModel()
        {
            id = new models.IntegerField().primary_key();
            income = new models.FloatField().default(12.4).null(True);
        }
    };

### DROP TABLE

To drop a table, you have two options:

1. Runtime drop

Use the DROP() static method:

    ExampleModel::DROP();

2. Normal drop

Just delete de model declaration.

### Retrieve data (SELECT)

To retrieve a set of data, use the .filter() method. e.g.:

    ```c++
    models.Model* m;
    models.QuerySet* n;

    m = new ExampleModel;
    n = m->filter("income",'>', 10);

    n.objects(); // in this case, returns a STL list with a
                // STL map<string, int> with key => value.
    ```

`ret` is now a list of 

To retrieve a single instance of data, it's better to use the .get() method. e.g.:

    ```c++
    models.Model* m;
    models.QuerySet* n;

    m = new ExampleModel;
    n = m->get("id", '=', 5);

    n.objects(); // in this case, returns a single 
                //  STL map<string, int> with key => value
    ```

### INSERT data

To insert new data in a table, do as follows:

    ```c++
    models.Model* m;

    m = new ExampleModel;

    m.set("income", 10000);
    m.set("id", 7);

    m.save();
    ```

### UPDATE data

To update a data, you must retrieve it first:

    ```c++
    models.Model* m;
    models.QuerySet* n;

    m = new ExampleModel;
    n = m->get("id", '=', 7);

    n.set("income", 20);

    n.save();
    ```

### DELETE data

Like an update, you must retrieve the data first:

    ```c++
    models.Model* m;
    models.QuerySet* n;

    m = new ExampleModel;
    n = m->get("id", '=', 7);

    n.delete();

    n.save();
    ```