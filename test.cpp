#include "DLCpp.h"
#include <stdexcept>
#include <iostream>

using namespace std;

DLCPP_NEW_MODEL(Abacate)
{
	public: Abacate()
	{
		column["coluna1"] = models::IntegerField();
		column["coluna2"] = models::IntegerField().size(32);

		CREATE();
	}
};

int main()
{
	Abacate* a = new Abacate;
	try
	{
		models::SingleSet<Abacate>* q;

		q = a->get("coluna1__gt", 0);

		cout << "RET:" << (*q)["coluna2"] << endl;
	}
	catch(std::runtime_error e)
	{
		cout << e.what();
	}
	catch(models::ObjectDoesNotExist e)
	{
		cout << "NOOP";
	}
	catch(models::MultipleObjectsReturned e)
	{
		cout << "MUL";
	}

	delete a;
	return 0;
}