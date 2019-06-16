#include "concretemodule.hpp"


using namespace std;


// Test application. Hopefully readable without many comments.
int main()
{
	// Init
	EventSource<int> intSrc;
	EventSource<float> floatSrc1, floatSrc2;
	EventSource<vector<string>> vecStrSrc;
	EventSource<string> strSrc;

	// PS: Creational pattern to be reviewd.
	std::shared_ptr<ConcreteModuleType1> module1 = ConcreteModuleType1::create(TYPE_1_IMPLEMENTATION_1);
	std::shared_ptr<ConcreteModuleType2> module2 = ConcreteModuleType2::create(TYPE_2_IMPLEMENTATION_1);

	// Connections
	module1->setInput(intSrc, floatSrc1, strSrc, floatSrc2);
	module2->setInput(*module1, floatSrc1, strSrc);

	// Run
	for (int i = 0; i < 10; i++)
	{
		intSrc.emit(i);
		floatSrc1.emit(i * .5f);
		strSrc.emit("text");

		// Emit the second float only every other frame.
		if (i % 2)
			floatSrc2.emit(i * 1.2f);
	}

	cout << "Press a key to terminate" << std::endl;
	getchar();
	return 0;
}
