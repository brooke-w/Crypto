/*Brooke Weborg
EECS 5980
Programming Assignment #1
2-21-2019

Does multiplication within the GF
*/

#include "pch.h"
#include <iostream>
#include <string>

using namespace std;

/*Precondition: Two 8 bit representations of the polynomials to be multiplied
Postcondition: The GF mutltiplication result of the two polynomials as an 8 bit number*/
unsigned char gfMult(int a, int b, int c) {
	int result = b;
	int product = 0;

	for (int i = 0; i < 8; i++) {
		if ((result >> 8) & 0x01){	//if there is a 1 bit in the 2^8 bit position
			result ^= c;
			if (a & 0x01)	product ^= result;
			result <<= 1;
		}
		else {						//if there is a 0 bit in the 2^8 bit position
			if (a & 0x01)	product ^= result;
			result <<= 1;
		}
		a >>= 1;
	}
	return (unsigned char)product;
}

/*Precondition: Give it a character
Postcondition: Returns a string of hex digits representing the character inputted*/
string charToHex(unsigned char character)
{
	string hexDigits = "";
	unsigned char firstDigit = character >> 4;	//first hex digit is most significant 4 bits
	unsigned char secondDigit = character & 0x0F;	//second hex digit is least significant 4 bits

	if (firstDigit >= 0 && firstDigit <= 9)		//if it's 0-9 add 30 to get character representation
		hexDigits += firstDigit + 0x30;
	else
		hexDigits += firstDigit + 0x37;			//otherwise it's a letter so add 37 to get character rep		

	if (secondDigit >= 0 && secondDigit <= 9)	//same logic as above but for second digit
		hexDigits += secondDigit + 0x30;
	else
		hexDigits += secondDigit + 0x37;

	return hexDigits;
}

int main()
{
	int c = 0x11B; //x8 + x4 + x3 + x + 1
	unsigned char product = 0;
	for (int i = 0; i < 16; i++) {
		cout << charToHex(i) << ":" << endl;
		for (int j = 0; j < 256; j++) {
			if ((j + 1) % 32 != 0)	cout << charToHex(gfMult(i, j, c)) << " ";
			else					cout << charToHex(gfMult(i, j, c)) << endl;
		}
		cout << endl;
	}
}