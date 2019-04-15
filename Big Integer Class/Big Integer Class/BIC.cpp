/*Brooke Weborg
EECS 5980
Programming Assignment #4
4-2#-2019
*/

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include "BigInt.h"

using namespace std;

int main()
{
	//create a two big ints
	BigInt A("55");
	BigInt B;
	B.setInt("550");
	BigInt C("45");

	//test the logic operators
	cout << "A < B: True : "  << (A < B)  << endl;
	cout << "A > B: False : " << (A > B)  << endl;
	cout << "A <= C: True : " << (A <= C) << endl;
	cout << "A >= C: True : " << (A >= C) << endl;
	cout << "A == B: False : " << (A == B) << endl;
	cout << "A != C: False : " << (A != C) << endl;

	//test adition
	BigInt D (A + B);
	cout << "A + B: 95 : " << D.integer << endl;
	BigInt E(A + C);
	cout << "A + C: 90 : " << E.integer << endl;
}
