#include <iostream>
#include "wiringPi.h"
#include "rotaryencoder.h"
using namespace std;

int main()
{
	int pin_a = 21;
	int pin_b = 18;
	struct encoder *e = setupencoder(pin_a, pin_b);

	for(;;)
	{
		cout << e->value << endl;
		delay(50);
	}

	return 0;
}
