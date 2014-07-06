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
	try
	{
		Abacate* a = new Abacate;
		models::SingleSet<Abacate>* q;

		q = a->get("coluna1__eq", 2);

		cout << "RETURNED VALUE:" << (*q)["coluna2"] << endl;

		delete a;
	}
	catch(std::runtime_error e)
	{
		printf("%s", e.what());
	}
	return 0;
}