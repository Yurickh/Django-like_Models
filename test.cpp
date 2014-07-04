#include "DLCpp.h"
#include <stdexcept>
#include <iostream>

using namespace std;

DLCPP_NEW_MODEL(Abacate)
{
	public: Abacate()
	{
		column["1int"] = models::IntegerField();
		column["2int"] = models::IntegerField().size(32);
	}
};

int main()
{
	try
	{	Abacate::CREATE(); }
	catch(exception e)
	{
		printf("%s", e.what());
	}
	return 0;
}