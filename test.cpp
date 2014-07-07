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
    models::MultipleSet<Abacate>* q;

    q = a->filter("coluna2__eq", 4);

    q->front()["coluna2"] = 5;

    q->save();

    delete a;
	return 0;
}