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
	//create a big ints
	BigInt A("0");
	BigInt B("2");
	BigInt C("4");
	BigInt D("6");
	BigInt E("8");
	BigInt F("16");
	BigInt G("32");
	BigInt H("64");
	BigInt I("3000000000");
	BigInt J("2");
	BigInt K("4");
	BigInt L("6");
	BigInt M("8");
	BigInt N("16");
	BigInt O("32");
	BigInt P("64");
	BigInt Q("128");
	BigInt R("256");
	BigInt S("512");
	BigInt T("1024");
	BigInt U("2048");
	BigInt V("4096");
	BigInt W("8192");
	BigInt X("16384");
	BigInt Y("32768");
	BigInt Z("65536");
	BigInt AA("0");
	BigInt AB("3000000000");
	BigInt AC("15");


	//test the logic operators
	cout << "A < B: True : " << (A < B) << endl;
	cout << "A > B: False : " << (A > B) << endl;
	cout << "A <= C: True : " << (A <= C) << endl;
	cout << "A >= C: True : " << (A >= C) << endl;
	cout << "A == B: False : " << (A == B) << endl;
	cout << "A != C: False : " << (A != C) << endl;

	//test addition
	AA = A + Z;
	cout << "A + Z: 65536 : " << AA.integer << endl;
	AA = B + Y;
	cout << "B + Y: 32770 : " << AA.integer << endl;
	AA = C + X;
	cout << "C + X: 16388 : " << AA.integer << endl;
	AA = D + W;
	cout << "D + W: 8198 : " << AA.integer << endl;
	AA = E + V;
	cout << "E + V: 4104 : " << AA.integer << endl;
	AA = F + U;
	cout << "F + U: 2064 : " << AA.integer << endl;
	AA = G + T;
	cout << "G + T: 1056 : " << AA.integer << endl;
	AA = H + S;
	cout << "H + S: 576 : " << AA.integer << endl;
	AA = I + R;
	cout << "I + R: 256 : " << AA.integer << endl;
	AA = J + Q;
	cout << "J + Q: 130 : " << AA.integer << endl;
	AA = K + P;
	cout << "K + P: 68 : " << AA.integer << endl;
	AA = L + O;
	cout << "L + O: 38 : " << AA.integer << endl;
	AA = M + N;
	cout << "M + N: 24 : " << AA.integer << endl;

	//test subtraction
	AA = Z - A;
	cout << "Z - A: 65536 : " << AA.integer << endl;
	AA = Y - B;
	cout << "y - B: 32766 : " << AA.integer << endl;
	AA = X - C;
	cout << "X - C: 16380 : " << AA.integer << endl;
	AA = W - D;
	cout << "W - D: 8186 : " << AA.integer << endl;
	AA = V - E;
	cout << "V - E: 4088 : " << AA.integer << endl;
	AA = U - F;
	cout << "U - F: 2032 : " << AA.integer << endl;
	AA = T - G;
	cout << "T - G: 992 : " << AA.integer << endl;
	AA = S - H;
	cout << "S - H: 448 : " << AA.integer << endl;

	//test multiplication
	AA = AB * B;
	cout << "AB * A: 6000000000 : " << AA.integer << endl;
	AA = Z * A;
	cout << "Z * A: 0 : " << AA.integer << endl;
	AA = Y * B;
	cout << "y * B: 65536 : " << AA.integer << endl;
	AA = X * C;
	cout << "X * C: 65536 : " << AA.integer << endl;
	AA = W * D;
	cout << "W * D: 49125 : " << AA.integer << endl;
	AA = V * E;
	cout << "V * E: 32768 : " << AA.integer << endl;
	AA = U * F;
	cout << "U * F: 32768 : " << AA.integer << endl;
	AA = T * G;
	cout << "T * G: 32768 : " << AA.integer << endl;
	AA = S * H;
	cout << "S * H: 32768 : " << AA.integer << endl;
	//to-do Take care of giant * small num multiplication problem

	//test division
	AA = Z / A;
	cout << "Z / A: 0 : " << AA.integer << endl;
	AA = Y / B;
	cout << "y / B: 16384 : " << AA.integer << endl;
	AA = X / C;
	cout << "X / C: 4096 : " << AA.integer << endl;
	AA = W / D;
	cout << "W / D: 1365 : " << AA.integer << endl;
	AA = V / E;
	cout << "V / E: 512 : " << AA.integer << endl;
	AA = U / F;
	cout << "U / F: 128 : " << AA.integer << endl;

	//test pow
	AA = B.pow(B, C, "100");
	cout << "B / C mod 100: 0 : " << AA.integer << endl;
}
