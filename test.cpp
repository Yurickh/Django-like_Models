#include "DLCpp.h"
#include <stdexcept>
#include <iostream>

using namespace std;
 
DLCPP_NEW_MODEL(Abacate)
{
	public: Abacate()
	{
		column["coluna1"] = models::CharField().max_length(32).standard("HELOU");
		column["coluna2"] = models::IntegerField().size(32);

		CREATE();
	}
};

int main()
{
	Abacate* a = new Abacate();
    models::QuerySet<Abacate>* q;

    q = a->insert();

    q->set("coluna1", "Yurick");
    q->set("coluna2", 4);

    q->save();

	return 0;
}