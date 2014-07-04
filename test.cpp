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
	try
	{	Abacate::DROP(); }
	catch(exception e)
	{
		printf("%s", e.what());
	}
	return 0;
}