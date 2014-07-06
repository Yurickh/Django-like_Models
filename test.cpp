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
	}
};

int main()
{
	Abacate::CREATE();

	try
	{
		Abacate* a = new Abacate;
		models::QuerySet<Abacate>* q;

		/*q = a->insert();
		
		q->set("coluna1", 2);
		q->set("coluna2", 3);

		q->save();*/

		delete a;
	}
	catch(std::runtime_error e)
	{
		printf("%s", e.what());
	}
	return 0;
}