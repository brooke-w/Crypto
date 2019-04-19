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
	BigInt AB("3000");
	BigInt AC("15");


	//test the logic operators
	cout << "A < B: 1 : " << (A < B) << endl;
	cout << "A > B: 0 : " << (A > B) << endl;
	cout << "A <= C: 1 : " << (A <= C) << endl;
	cout << "A >= C: 0 : " << (A >= C) << endl;
	cout << "A == B: 0 : " << (A == B) << endl;
	cout << "A != C: 1 : " << (A != C) << endl;

	//test addition
	AA = A + A;
	cout << "A + A: 0 : " << AA.toString() << endl;
	AA = A + Z;
	cout << "A + Z: 65536 : " << AA.toString() << endl;
	AA = B + Y;
	cout << "B + Y: 32770 : " << AA.toString() << endl;
	AA = C + X;
	cout << "C + X: 16388 : " << AA.toString() << endl;
	AA = D + W;
	cout << "D + W: 8198 : " << AA.toString() << endl;
	AA = E + V;
	cout << "E + V: 4104 : " << AA.toString() << endl;
	AA = F + U;
	cout << "F + U: 2064 : " << AA.toString() << endl;
	AA = G + T;
	cout << "G + T: 1056 : " << AA.toString() << endl;
	AA = H + S;
	cout << "H + S: 576 : " << AA.toString() << endl;
	AA = I + R;
	cout << "I + R: 3000000256 : " << AA.toString() << endl;
	AA = J + Q;
	cout << "J + Q: 130 : " << AA.toString() << endl;
	AA = K + P;
	cout << "K + P: 68 : " << AA.toString() << endl;
	AA = L + O;
	cout << "L + O: 38 : " << AA.toString() << endl;
	AA = M + N;
	cout << "M + N: 24 : " << AA.toString() << endl;

	//test subtraction
	AA = A - A;
	cout << "A -A: 0 : " << AA.toString() << endl;
	AA = Z - A;
	cout << "Z - A: 65536 : " << AA.toString() << endl;
	AA = Y - B;
	cout << "y - B: 32766 : " << AA.toString() << endl;
	AA = X - C;
	cout << "X - C: 16380 : " << AA.toString() << endl;
	AA = W - D;
	cout << "W - D: 8186 : " << AA.toString() << endl;
	AA = V - E;
	cout << "V - E: 4088 : " << AA.toString() << endl;
	AA = U - F;
	cout << "U - F: 2032 : " << AA.toString() << endl;
	AA = T - G;
	cout << "T - G: 992 : " << AA.toString() << endl;
	AA = S - H;
	cout << "S - H: 448 : " << AA.toString() << endl;

	//test multiplication
	AA = A * A;
	cout << "A * A: 0 : " << AA.toString() << endl;
	AA = AB * B;
	cout << "AB * A: 6000 : " << AA.toString() << endl;
	AA = Z * A;
	cout << "Z * A: 0 : " << AA.toString() << endl;
	AA = Y * B;
	cout << "y * B: 65536 : " << AA.toString() << endl;
	AA = X * C;
	cout << "X * C: 65536 : " << AA.toString() << endl;
	AA = W * D;
	cout << "W * D: 49125 : " << AA.toString() << endl;
	AA = V * E;
	cout << "V * E: 32768 : " << AA.toString() << endl;
	AA = U * F;
	cout << "U * F: 32768 : " << AA.toString() << endl;
	AA = T * G;
	cout << "T * G: 32768 : " << AA.toString() << endl;
	AA = S * H;
	cout << "S * H: 32768 : " << AA.toString() << endl;
	//to-do Take care of giant * small num multiplication problem

	//test division
	AA = A / "2";
	cout << "A / 2: 0 : " << AA.toString() << endl;
	AA = Z / A;
	cout << "Z / A: 0 : " << AA.toString() << endl;
	AA = Y / B;
	cout << "y / B: 16384 : " << AA.toString() << endl;
	AA = X / C;
	cout << "X / C: 4096 : " << AA.toString() << endl;
	AA = W / D;
	cout << "W / D: 1365 : " << AA.toString() << endl;
	AA = V / E;
	cout << "V / E: 512 : " << AA.toString() << endl;
	AA = U / F;
	cout << "U / F: 128 : " << AA.toString() << endl;

	//test MOD
	AA = A % "2";
	cout << "A % 2: 0 : " << AA.toString() << endl;
	AA = Z % AC;
	cout << "Z % AC: 1 : " << AA.toString() << endl;
	AA = Y % AC;
	cout << "y % AC: 8 : " << AA.toString() << endl;
	AA = X % AC;
	cout << "X % AC: 4 : " << AA.toString() << endl;
	AA = W % AC;
	cout << "W % AC: 2 : " << AA.toString() << endl;
	AA = V % AC;
	cout << "V % AC: 1 : " << AA.toString() << endl;
	AA = U % AC;
	cout << "U % AC: 8 : " << AA.toString() << endl;

	//test pow
	AA = BigInt::pow(BigInt("8"), BigInt("5"), "10");
	cout << "8^5 mod 10: 8 : " << AA.toString() << endl;
	AA = BigInt::pow(BigInt("32"), BigInt("5"), "13");
	cout << "32^5 mod 13: 2 : " << AA.toString() << endl;
	AA = BigInt::pow(BigInt("320"), BigInt("5"), "13");
	cout << "320^5 mod 13: 8 : " << AA.toString() << endl;
	AA = BigInt::pow(BigInt("3200"), BigInt("5"), "13");
	cout << "3200^5 mod 13: ? : " << AA.toString() << endl;
	AA = BigInt::pow(BigInt("123456789"), BigInt("5"), "24");
	cout << "123456789^5 mod 24: ? : " << AA.toString() << endl;

	AA = BigInt::pow(BigInt("8"), BigInt("288"), "10");
	cout << "8^288 mod 10: ? : " << AA.toString() << endl;
	AA = BigInt::pow(BigInt("32"), BigInt("288"), "13");
	cout << "32^288 mod 13: ? : " << AA.toString() << endl;
	AA = BigInt::pow(BigInt("320"), BigInt("288"), "13");
	cout << "320^288 mod 13: ? : " << AA.toString() << endl;
	AA = BigInt::pow(BigInt("3200"), BigInt("288"), "13");
	cout << "3200^288 mod 13: ? : " << AA.toString() << endl;
	AA = BigInt::pow(BigInt("123456789"), BigInt("288"), "24");
	cout << "123456789^288 mod 24: ? : " << AA.toString() << endl;

	//Dr. T tests
	cout << "inverse test" << endl;
	AA = BigInt::pow(BigInt("3"), BigInt("3"), "5");
	cout << "3^(5-2) mod 5: ? : " << AA.toString() << endl;
	AA = BigInt("3") * AA;
	AA = AA % "5";
	cout << "a*x mod 5: ? : " << AA.toString() << endl;

	BigInt a = BigInt("123456789012345678901234567890");
	BigInt b = BigInt("6864797660130609714981900799081393217269435300143305409394463459185543183397656052122559640661454554977296311391480858037121987999716643812574028291115057151");
	clock_t start = clock();	//start a timer
	BigInt c = BigInt::pow(a, (b - "2"), b);
	BigInt d = a * c;
	AA = d % b;
	cout << a.toString() << endl << c.toString() << endl << AA.toString() << endl;
	//print out how long it took to run
	clock_t runtime = clock() - start;
	cout << runtime / 1000 << "." << runtime % 1000 << " s" << endl;

}
