/*Brooke Weborg
EECS 5980
Programming Assignment #1
2-1-2019

This program will take console input in the form
DES <–action> <key> <mode> <infile> <outfile>

And perform the DES encryption/decryption on a given file.
*/

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

using namespace std;

/*Globals*/
unsigned long long roundKeys[17] = { 0 };		//index 0 will not be used. 1-16 will be used for easier reference to keys
unsigned long long sbox1[64] = { 14, 4 , 13, 01, 02, 15, 11, 8, 3, 10, 6, 12, 5, 9 , 0, 7,
								0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
								4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
								15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 };
unsigned long long sbox2[64] = { 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
								3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
								0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
								13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9};
unsigned long long sbox3[64] = { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8, 
								13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
								13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
								1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12};
unsigned long long sbox4[64] = { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
								13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
								10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
								3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14};
unsigned long long sbox5[64] = { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
								14, 11, 2, 12, 4,7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
								4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
								11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3};
unsigned long long sbox6[64] = { 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
								10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
								9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
								4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13};
unsigned long long sbox7[64] = { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
								13, 0 ,11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
								1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
								6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12};
unsigned long long sbox8[64] = { 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
								1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2, 
								7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
								2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11};

/*Precondition: Takes a character representing a hex value
Postcondition: creates a true integer representation but as a character */
unsigned char charHexValue(unsigned char c)
{
	if (c >= 0x30 && c <= 0x39)						//0 - 9 subtract 30 hex
		c = c - 0x30;
	else if ((c >= 65 && c <= 70) || (c >= 97 && c <= 102)) //A-F subtract 37 hex
	{
		c = toupper(c);
		c = c - 0x37;
	}
	else {
		cerr << "Error: Invalid hex character.";
		exit(1);
	}
	return c;
}

/*Precondition: Takes a string that is to represent the key for DES
Postcondition: The key is now represented as an unsigned long long*/
unsigned long long prepUserKey(string key) 
{
	//check the key for the right length first
	int length = key.length();
	if (length != 16 && length != 10) {
		cerr << "Error: Bad Input. Specify a 16 digit hex value or a 8 character string surrounded by single quotes for the key.";
		exit(1);
	}

	//If the string is enclosed in single quotes, it is to be interpretted as 8 characters
	//Else it's interpretted as 16 hex digits.
	unsigned long long keyToReturn = 0;
	if (key[0] == '\'' & key[length-1] == '\'')	//interpret as chars
	{
		//starting at the leftmost char, shift it into the key to be returned
		keyToReturn = unsigned char (key[1]) << 8;
		keyToReturn = (keyToReturn | unsigned char(key[2])) << 8;
		keyToReturn = (keyToReturn | unsigned char(key[3])) << 8;
		keyToReturn = (keyToReturn | unsigned char(key[4])) << 8;
		keyToReturn = (keyToReturn | unsigned char(key[5])) << 8;
		keyToReturn = (keyToReturn | unsigned char(key[6])) << 8;
		keyToReturn = (keyToReturn | unsigned char(key[7])) << 8;
		keyToReturn = (keyToReturn | unsigned char(key[8]));
	}
	else    //interpret as a 16 char hex key
	{
		//starting at the leftmost char, shift it into the key to be returned
		keyToReturn = charHexValue(unsigned char(key[0])) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[1]))) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[2]))) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[3]))) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[4]))) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[5]))) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[6]))) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[7]))) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[8]))) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[9]))) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[10]))) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[11]))) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[12]))) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[13]))) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[14]))) << 4;
		keyToReturn = (keyToReturn | charHexValue(unsigned char(key[15])));
	}
	return keyToReturn;
}

/*Precondition: Given a 64 bit key
PostCondition: Returns K0*/
unsigned long long makeK0(unsigned long long key) {
	//Every 8th bit is discarded
	unsigned long long k0 = 0;
	k0 |= (key >> 7) & 0x0100000000000000;			//Bit 1 aka leftmost bit is moved to position 8
	k0 |= (key >> 14) & 0x0001000000000000;			//Bit at 2 to 16
	k0 |= (key >> 21) & 0x0000010000000000;			//Bit at 3 to 24
	k0 |= (key >> 52) & 0x0000000000000100;			//Bit at 4 to 56
	k0 |= (key >> 47) & 0x0000000000001000;			//Bit at 5 to 52
	k0 |= (key >> 38) & 0x0000000000100000;			//Bit at 6 to 44
	k0 |= (key >> 29) & 0x0000000010000000;			//Bit at 7 to 36
	k0 |= (key << 2) & 0x0200000000000000;			//Bit at 9 to 7
	k0 |= (key >> 5) & 0x0002000000000000;			//Bit at 10 to 15
	k0 |= (key >> 12) & 0x0000020000000000;			//Bit at 11 to 23
	k0 |= (key >> 43) & 0x0000000000000200;			//Bit at 12 to 55
	k0 |= (key >> 38) & 0x0000000000002000;			//Bit at 13 to 51
	k0 |= (key >> 29) & 0x0000000000200000;			//Bit at 14 to 43
	k0 |= (key >> 20) & 0x0000000020000000;			//Bit at 15 to 35
	k0 |= (key << 11) & 0x0400000000000000;			//Bit at 17 to 6
	k0 |= (key << 4) & 0x0004000000000000;			//Bit at 18 to 14
	k0 |= (key >> 3) & 0x0000040000000000;			//Bit at 19 to 22
	k0 |= (key >> 34) & 0x0000000000000400;			//Bit at 20 to 54
	k0 |= (key >> 29) & 0x0000000000004000;			//Bit at 21 to 50
	k0 |= (key >> 20) & 0x0000000000400000;			//Bit at 22 to 42
	k0 |= (key >> 11) & 0x0000000040000000;			//Bit at 23 to 34
	k0 |= (key << 20) & 0x0800000000000000;			//Bit at 25 to 5
	k0 |= (key << 13) & 0x0008000000000000;			//Bit at 26 to 13
	k0 |= (key << 6) & 0x0000080000000000;			//Bit at 27 to 21
	k0 |= (key >> 25) & 0x0000000000000800;			//Bit at 28 to 53
	k0 |= (key >> 20) & 0x0000000000008000;			//Bit at 29 to 49
	k0 |= (key >> 11) & 0x0000000000800000;			//Bit at 30 to 41
	k0 |= (key >> 2) & 0x0000000080000000;			//Bit at 31 to 33
	k0 |= (key << 29) & 0x1000000000000000;			//Bit at 33 to 4
	k0 |= (key << 22) & 0x0010000000000000;			//Bit at 34 to 12
	k0 |= (key << 15) & 0x0000100000000000;			//Bit at 35 to 20
	k0 |= (key << 8) & 0x0000001000000000;			//Bit at 36 to 28
	k0 |= (key >> 11) & 0x0000000000010000;			//Bit at 37 to 48
	k0 |= (key >> 2) & 0x0000000001000000;			//Bit at 38 to 40
	k0 |= (key << 7) & 0x0000000100000000;			//Bit at 39 to 32
	k0 |= (key << 38) & 0x2000000000000000;			//Bit at 41 to 3
	k0 |= (key << 31) & 0x0020000000000000;			//Bit at 42 to 11
	k0 |= (key << 24) & 0x0000200000000000;			//Bit at 43 to 19
	k0 |= (key << 17) & 0x0000002000000000;			//Bit at 44 to 27
	k0 |= (key >> 2) & 0x0000000000020000;			//Bit at 45 to 47
	k0 |= (key << 7) & 0x0000000002000000;			//Bit at 46 to 39
	k0 |= (key << 16) & 0x0000000200000000;			//Bit at 47 to 31
	k0 |= (key << 47) & 0x4000000000000000;			//Bit at 49 to 2
	k0 |= (key << 40) & 0x0040000000000000;			//Bit at 50 to 10
	k0 |= (key << 33) & 0x0000400000000000;			//Bit at 51 to 18
	k0 |= (key << 26) & 0x0000004000000000;			//Bit at 52 to 26
	k0 |= (key << 7) & 0x0000000000040000;			//Bit at 53 to 46
	k0 |= (key << 16) & 0x0000000004000000;			//Bit at 54 to 38
	k0 |= (key << 25) & 0x0000000400000000;			//Bit at 55 to 30
	k0 |= (key << 56) & 0x8000000000000000;			//Bit at 57 to 1
	k0 |= (key << 49) & 0x0080000000000000;			//Bit at 58 to 9
	k0 |= (key << 42) & 0x0000800000000000;			//Bit at 59 to 17
	k0 |= (key << 35) & 0x0000008000000000;			//Bit at 60 to 25
	k0 |= (key << 16) & 0x0000000000080000;			//Bit at 61 to 45
	k0 |= (key << 25) & 0x0000000008000000;			//Bit at 62 to 37
	k0 |= (key << 34) & 0x0000000800000000;			//Bit at 63 to 29
	k0 = k0 >> 8;
	return k0;
}

/*Precondition: Given a set of round keys
Postcondition: Compress keys to 48 bits*/
void compressRoundKeys(unsigned long long roundKeys[]) {
	//ex:if (x & 0x80000000) y |= 0x00400000;
	//	 if (x & 0x40000000) y |= 0x00010000;

	unsigned long long key = roundKeys[1];
	unsigned long long newKey = 0;
	unsigned long long bit = 1;

	//compress round 1 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48-5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit<< (56 - 5)))  newKey |= (bit<< (48 - 6));		//bit at 5 to position 6
	if (key & (bit<< (56 - 6)))  newKey |= (bit<< (48 - 10));		//bit at 6 to position 10
	if (key & (bit<< (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 -  8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[1] = newKey;

	key = roundKeys[2];
	newKey = 0;

	//compress round 2 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[2] = newKey;

	key = roundKeys[3];
	newKey = 0;

	//compress round 3 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[3] = newKey;

	key = roundKeys[4];
	newKey = 0;

	//compress round 4 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[4] = newKey;

	key = roundKeys[5];
	newKey = 0;

	//compress round 5 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[5] = newKey;

	key = roundKeys[6];
	newKey = 0;

	//compress round 6 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[6] = newKey;

	key = roundKeys[7];
	newKey = 0;

	//compress round 7 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[7] = newKey;

	key = roundKeys[8];
	newKey = 0;

	//compress round 8 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[8] = newKey;

	key = roundKeys[9];
	newKey = 0;

	//compress round 9 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[9] = newKey;

	key = roundKeys[10];
	newKey = 0;

	//compress round 10 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[10] = newKey;

	key = roundKeys[11];
	newKey = 0;

	//compress round 11 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[11] = newKey;

	key = roundKeys[12];
	newKey = 0;

	//compress round 12 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[12] = newKey;

	key = roundKeys[13];
	newKey = 0;

	//compress round 13 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[13] = newKey;

	key = roundKeys[14];
	newKey = 0;

	//compress round 14 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[14] = newKey;

	key = roundKeys[15];
	newKey = 0;

	//compress round 15 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[15] = newKey;

	key = roundKeys[16];
	newKey = 0;

	//compress round 16 key
	if (key & (bit << (56 - 1)))  newKey |= (bit << (48 - 5));		//bit at 1 to position 5
	if (key & (bit << (56 - 2)))  newKey |= (bit << (48 - 24));		//bit at 2 to position 24
	if (key & (bit << (56 - 3)))  newKey |= (bit << (48 - 7));		//bit at 3 to position 7
	if (key & (bit << (56 - 4)))  newKey |= (bit << (48 - 16));		//bit at 4 to position 16
	if (key & (bit << (56 - 5)))  newKey |= (bit << (48 - 6));		//bit at 5 to position 6
	if (key & (bit << (56 - 6)))  newKey |= (bit << (48 - 10));		//bit at 6 to position 10
	if (key & (bit << (56 - 7)))  newKey |= (bit << (48 - 20));		//bit at 7 to position 20
	if (key & (bit << (56 - 8)))  newKey |= (bit << (48 - 18));		//bit at 8 to position 18
	if (key & (bit << (56 - 10)))  newKey |= (bit << (48 - 12));	//bit at 10 to position 12
	if (key & (bit << (56 - 11)))  newKey |= (bit << (48 - 3));		//bit at 11 to position 3
	if (key & (bit << (56 - 12)))  newKey |= (bit << (48 - 15));	//bit at 12 to position 15
	if (key & (bit << (56 - 13)))  newKey |= (bit << (48 - 23));	//bit at 13 to position 23
	if (key & (bit << (56 - 14)))  newKey |= (bit << (48 - 1));		//bit at 14 to position 1
	if (key & (bit << (56 - 15)))  newKey |= (bit << (48 - 9));		//bit at 15 to position 9
	if (key & (bit << (56 - 16)))  newKey |= (bit << (48 - 19));	//bit at 16 to position 19
	if (key & (bit << (56 - 17)))  newKey |= (bit << (48 - 2));		//bit at 17 to position 2
	if (key & (bit << (56 - 19)))  newKey |= (bit << (48 - 14));	//bit at 19 to position 14
	if (key & (bit << (56 - 20)))  newKey |= (bit << (48 - 22));	//bit at 20 to position 22
	if (key & (bit << (56 - 21)))  newKey |= (bit << (48 - 11));	//bit at 21 to position 11
	if (key & (bit << (56 - 23)))  newKey |= (bit << (48 - 13));	//bit at 23 to position 13
	if (key & (bit << (56 - 24)))  newKey |= (bit << (48 - 4));		//bit at 24 to position 4
	if (key & (bit << (56 - 26)))  newKey |= (bit << (48 - 17));	//bit at 26 to position 17
	if (key & (bit << (56 - 27)))  newKey |= (bit << (48 - 21));	//bit at 27 to position 21
	if (key & (bit << (56 - 28)))  newKey |= (bit << (48 - 8));	//bit at 28 to position 8
	if (key & (bit << (56 - 29)))  newKey |= (bit << (48 - 47));	//bit at 29 to position 47
	if (key & (bit << (56 - 30)))  newKey |= (bit << (48 - 31));	//bit at 30 to position 31
	if (key & (bit << (56 - 31)))  newKey |= (bit << (48 - 27));	//bit at 31 to position 27
	if (key & (bit << (56 - 32)))  newKey |= (bit << (48 - 48));	//bit at 32 to position 48
	if (key & (bit << (56 - 33)))  newKey |= (bit << (48 - 35));	//bit at 33 to position 35
	if (key & (bit << (56 - 34)))  newKey |= (bit << (48 - 41));	//bit at 34 to position 41
	if (key & (bit << (56 - 36)))  newKey |= (bit << (48 - 46));	//bit at 36 to position 46
	if (key & (bit << (56 - 37)))  newKey |= (bit << (48 - 28));	//bit at 37 to position 28
	if (key & (bit << (56 - 39)))  newKey |= (bit << (48 - 39));	//bit at 39 to position 39
	if (key & (bit << (56 - 40)))  newKey |= (bit << (48 - 32));	//bit at 40 to position 32
	if (key & (bit << (56 - 41)))  newKey |= (bit << (48 - 25));	//bit at 41 to position 25
	if (key & (bit << (56 - 42)))  newKey |= (bit << (48 - 44));	//bit at 42 to position 44
	if (key & (bit << (56 - 44)))  newKey |= (bit << (48 - 37));	//bit at 44 to position 37
	if (key & (bit << (56 - 45)))  newKey |= (bit << (48 - 34));	//bit at 45 to position 34
	if (key & (bit << (56 - 46)))  newKey |= (bit << (48 - 43));	//bit at 46 to position 43
	if (key & (bit << (56 - 47)))  newKey |= (bit << (48 - 29));	//bit at 47 to position 29
	if (key & (bit << (56 - 48)))  newKey |= (bit << (48 - 36));	//bit at 48 to position 36
	if (key & (bit << (56 - 49)))  newKey |= (bit << (48 - 38));	//bit at 49 to position 38
	if (key & (bit << (56 - 50)))  newKey |= (bit << (48 - 45));	//bit at 50 to position 45
	if (key & (bit << (56 - 51)))  newKey |= (bit << (48 - 33));	//bit at 51 to position 33
	if (key & (bit << (56 - 52)))  newKey |= (bit << (48 - 26));	//bit at 52 to position 26
	if (key & (bit << (56 - 53)))  newKey |= (bit << (48 - 42));	//bit at 53 to position 42
	if (key & (bit << (56 - 55)))  newKey |= (bit << (48 - 30));	//bit at 55 to position 30
	if (key & (bit << (56 - 56)))  newKey |= (bit << (48 - 40));	//bit at 56 to position 40
	roundKeys[16] = newKey;
}

/*Precondition: Given k0
Postcondition: Stores all 16 rounds keys into */
void makeRoundKeys(unsigned long long k0, unsigned long long roundKeys[])
{
	/*The round number determines the number of bits to rotate
	Rounds 1, 2, 9, and 16 - Rotate 1 bit
	All other rounds: Rotate 2 bits
	Circular shift the left and rights halves individually
	Left circular shift for both halves*/
	unsigned long long k0Left = k0 >> 28;
	unsigned long long k0Right = k0 & 0x0FFFFFFF;
	unsigned int bitToKeep = 0;

	//Round 1
	bitToKeep = (k0Left >> 27) & 0x00000001;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 1) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 27) & 0x00000001;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 1) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[1] = (k0Left << 28)  | k0Right;

	//Round 2
	bitToKeep = (k0Left >> 27) & 0x00000001;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 1) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 27) & 0x00000001;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 1) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[2] = (k0Left << 28) | k0Right;

	//Round 3
	bitToKeep = (k0Left >> 26) & 0x00000003;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 2) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 26) & 0x00000003;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 2) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[3] = (k0Left << 28) | k0Right;

	//Round 4
	bitToKeep = (k0Left >> 26) & 0x00000003;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 2) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 26) & 0x00000003;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 2) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[4] = (k0Left << 28) | k0Right;

	//Round 5
	bitToKeep = (k0Left >> 26) & 0x00000003;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 2) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 26) & 0x00000003;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 2) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[5] = (k0Left << 28) | k0Right;

	//Round 6
	bitToKeep = (k0Left >> 26) & 0x00000003;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 2) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 26) & 0x00000003;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 2) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[6] = (k0Left << 28) | k0Right;

	//Round 7
	bitToKeep = (k0Left >> 26) & 0x00000003;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 2) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 26) & 0x00000003;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 2) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[7] = (k0Left << 28) | k0Right;

	//Round 8
	bitToKeep = (k0Left >> 26) & 0x00000003;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 2) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 26) & 0x00000003;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 2) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[8] = (k0Left << 28) | k0Right;

	//Round 9
	bitToKeep = (k0Left >> 27) & 0x00000001;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 1) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 27) & 0x00000001;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 1) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[9] = (k0Left << 28) | k0Right;

	//Round 10
	bitToKeep = (k0Left >> 26) & 0x00000003;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 2) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 26) & 0x00000003;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 2) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[10] = (k0Left << 28) | k0Right;

	//Round 11
	bitToKeep = (k0Left >> 26) & 0x00000003;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 2) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 26) & 0x00000003;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 2) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[11] = (k0Left << 28) | k0Right;

	//Round 12
	bitToKeep = (k0Left >> 26) & 0x00000003;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 2) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 26) & 0x00000003;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 2) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[12] = (k0Left << 28) | k0Right;

	//Round 13
	bitToKeep = (k0Left >> 26) & 0x00000003;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 2) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 26) & 0x00000003;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 2) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[13] = (k0Left << 28) | k0Right;

	//Round 14
	bitToKeep = (k0Left >> 26) & 0x00000003;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 2) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 26) & 0x00000003;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 2) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[14] = (k0Left << 28) | k0Right;

	//Round 15
	bitToKeep = (k0Left >> 26) & 0x00000003;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 2) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 26) & 0x00000003;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 2) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[15] = (k0Left << 28) | k0Right;

	//Round 16
	bitToKeep = (k0Left >> 27) & 0x00000001;	//Left Circular Shift k0 Left by 1 bit
	k0Left = (((k0Left << 1) | bitToKeep) & 0x0FFFFFFF);
	bitToKeep = (k0Right >> 27) & 0x00000001;   //Left Circular Shift k0 Right by 1 bit
	k0Right = ((k0Right << 1) | bitToKeep) & 0x0FFFFFFF;
	roundKeys[16] = (k0Left << 28) | k0Right;

	compressRoundKeys(roundKeys);
	return;
}

/*Precondition: Must be given a message that is 64 bits long
Postcondition: Returns permuted message.*/
unsigned long long initialP(unsigned long long message) {
	
	/*---- INITIAL PERMUTATION ----*/
	unsigned long long newMessage = 0;
	unsigned long long bit = 1;

	if (message & (bit << (64 - 1)))  newMessage |= (bit << (64 - 40));		//bit at 1 to position 40
	if (message & (bit << (64 - 2)))  newMessage |= (bit << (64 - 8));		//bit at 2 to position 8
	if (message & (bit << (64 - 3)))  newMessage |= (bit << (64 - 48));		//bit at 3 to position 48
	if (message & (bit << (64 - 4)))  newMessage |= (bit << (64 - 16));		//bit at 4 to position 16
	if (message & (bit << (64 - 5)))  newMessage |= (bit << (64 - 56));		//bit at 5 to position 56
	if (message & (bit << (64 - 6)))  newMessage |= (bit << (64 - 24));		//bit at 6 to position 24
	if (message & (bit << (64 - 7)))  newMessage |= (bit << (64 - 64));		//bit at 7 to position 64
	if (message & (bit << (64 - 8)))  newMessage |= (bit << (64 - 32));		//bit at 8 to position 32
	if (message & (bit << (64 - 9)))  newMessage |= (bit << (64 - 39));		//bit at 9 to position 39
	if (message & (bit << (64 - 10)))  newMessage |= (bit << (64 - 7));		//bit at 10 to position 7
	if (message & (bit << (64 - 11)))  newMessage |= (bit << (64 - 47));	//bit at 11 to position 47
	if (message & (bit << (64 - 12)))  newMessage |= (bit << (64 - 15));		//bit at 12 to position 15
	if (message & (bit << (64 - 13)))  newMessage |= (bit << (64 - 55));		//bit at 13 to position 55
	if (message & (bit << (64 - 14)))  newMessage |= (bit << (64 - 23));		//bit at 14 to position 23
	if (message & (bit << (64 - 15)))  newMessage |= (bit << (64 - 63));		//bit at 15 to position 63
	if (message & (bit << (64 - 16)))  newMessage |= (bit << (64 - 31));		//bit at 16 to position 31
	if (message & (bit << (64 - 17)))  newMessage |= (bit << (64 - 38));		//bit at 17 to position 38
	if (message & (bit << (64 - 18)))  newMessage |= (bit << (64 - 6));		//bit at 18 to position 6
	if (message & (bit << (64 - 19)))  newMessage |= (bit << (64 - 46));		//bit at 19 to position 46
	if (message & (bit << (64 - 20)))  newMessage |= (bit << (64 - 14));		//bit at 20 to position 14
	if (message & (bit << (64 - 21)))  newMessage |= (bit << (64 - 54));		//bit at 21 to position 54
	if (message & (bit << (64 - 22)))  newMessage |= (bit << (64 - 22));		//bit at 22 to position 22
	if (message & (bit << (64 - 23)))  newMessage |= (bit << (64 - 62));		//bit at 23 to position 62
	if (message & (bit << (64 - 24)))  newMessage |= (bit << (64 - 30));		//bit at 24 to position 30
	if (message & (bit << (64 - 25)))  newMessage |= (bit << (64 - 37));		//bit at 25 to position 37
	if (message & (bit << (64 - 26)))  newMessage |= (bit << (64 - 5));		//bit at 26 to position 5
	if (message & (bit << (64 - 27)))  newMessage |= (bit << (64 - 45));		//bit at 27 to position 45
	if (message & (bit << (64 - 28)))  newMessage |= (bit << (64 - 13));		//bit at 28 to position 13
	if (message & (bit << (64 - 29)))  newMessage |= (bit << (64 - 53));		//bit at 29 to position 53
	if (message & (bit << (64 - 30)))  newMessage |= (bit << (64 - 21));		//bit at 30 to position 21
	if (message & (bit << (64 - 31)))  newMessage |= (bit << (64 - 61));		//bit at 31 to position 61
	if (message & (bit << (64 - 32)))  newMessage |= (bit << (64 - 29));		//bit at 32 to position 29
	if (message & (bit << (64 - 33)))  newMessage |= (bit << (64 - 36));		//bit at 33 to position 36
	if (message & (bit << (64 - 34)))  newMessage |= (bit << (64 - 4));		//bit at 34 to position 4
	if (message & (bit << (64 - 35)))  newMessage |= (bit << (64 - 44));		//bit at 35 to position 44
	if (message & (bit << (64 - 36)))  newMessage |= (bit << (64 - 12));		//bit at 36 to position 12
	if (message & (bit << (64 - 37)))  newMessage |= (bit << (64 - 52));		//bit at 37 to position 52
	if (message & (bit << (64 - 38)))  newMessage |= (bit << (64 - 20));		//bit at 38 to position 20
	if (message & (bit << (64 - 39)))  newMessage |= (bit << (64 - 60));		//bit at 39 to position 60
	if (message & (bit << (64 - 40)))  newMessage |= (bit << (64 - 28));		//bit at 40 to position 28
	if (message & (bit << (64 - 41)))  newMessage |= (bit << (64 - 35));		//bit at 41 to position 35
	if (message & (bit << (64 - 42)))  newMessage |= (bit << (64 - 3));		//bit at 42 to position 3
	if (message & (bit << (64 - 43)))  newMessage |= (bit << (64 - 43));		//bit at 43 to position 43
	if (message & (bit << (64 - 44)))  newMessage |= (bit << (64 - 11));		//bit at 44 to position 11
	if (message & (bit << (64 - 45)))  newMessage |= (bit << (64 - 51));		//bit at 45 to position 51
	if (message & (bit << (64 - 46)))  newMessage |= (bit << (64 - 19));		//bit at 46 to position 19
	if (message & (bit << (64 - 47)))  newMessage |= (bit << (64 - 59));		//bit at 47 to position 59
	if (message & (bit << (64 - 48)))  newMessage |= (bit << (64 - 27));		//bit at 48 to position 27
	if (message & (bit << (64 - 49)))  newMessage |= (bit << (64 - 34));		//bit at 49 to position 34
	if (message & (bit << (64 - 50)))  newMessage |= (bit << (64 - 2));		//bit at 50 to position 2
	if (message & (bit << (64 - 51)))  newMessage |= (bit << (64 - 42));		//bit at 51 to position 42
	if (message & (bit << (64 - 52)))  newMessage |= (bit << (64 - 10));		//bit at 52 to position 10
	if (message & (bit << (64 - 53)))  newMessage |= (bit << (64 - 50));		//bit at 53 to position 50
	if (message & (bit << (64 - 54)))  newMessage |= (bit << (64 - 18));		//bit at 54 to position 18
	if (message & (bit << (64 - 55)))  newMessage |= (bit << (64 - 58));		//bit at 55 to position 58
	if (message & (bit << (64 - 56)))  newMessage |= (bit << (64 - 26));		//bit at 56 to position 26
	if (message & (bit << (64 - 57)))  newMessage |= (bit << (64 - 33));		//bit at 57 to position 33
	if (message & (bit << (64 - 58)))  newMessage |= (bit << (64 - 1));		//bit at 58 to position 1
	if (message & (bit << (64 - 59)))  newMessage |= (bit << (64 - 41));		//bit at 59 to position 41
	if (message & (bit << (64 - 60)))  newMessage |= (bit << (64 - 9));		//bit at 60 to position 9
	if (message & (bit << (64 - 61)))  newMessage |= (bit << (64 - 49));		//bit at 61 to position 49
	if (message & (bit << (64 - 62)))  newMessage |= (bit << (64 - 17));		//bit at 62 to position 17
	if (message & (bit << (64 - 63)))  newMessage |= (bit << (64 - 57));		//bit at 63 to position 57
	if (message & (bit << (64 - 64)))  newMessage |= (bit << (64 - 25));		//bit at 64 to position 25
	return newMessage;
}

/*Precondition: Must be given a message that is 64 bits long
Postcondition: Returns permuted message.*/
unsigned long long finalP(unsigned long long message) {

	/*---- FINAL PERMUTATION ----*/
	unsigned long long newMessage = 0;
	unsigned long long bit = 1;

	if (message & (bit << (64 - 1)))  newMessage |= (bit << (64 - 58));			//bit at 1 to position 58
	if (message & (bit << (64 - 2)))  newMessage |= (bit << (64 - 50));			//bit at 2 to position 50
	if (message & (bit << (64 - 3)))  newMessage |= (bit << (64 - 42));			//bit at 3 to position 42
	if (message & (bit << (64 - 4)))  newMessage |= (bit << (64 - 34));			//bit at 4 to position 34
	if (message & (bit << (64 - 5)))  newMessage |= (bit << (64 - 26));			//bit at 5 to position 26
	if (message & (bit << (64 - 6)))  newMessage |= (bit << (64 - 18));			//bit at 6 to position 18
	if (message & (bit << (64 - 7)))  newMessage |= (bit << (64 - 10));			//bit at 7 to position 10
	if (message & (bit << (64 - 8)))  newMessage |= (bit << (64 - 2));			//bit at 8 to position 2
	if (message & (bit << (64 - 9)))  newMessage |= (bit << (64 - 60));			//bit at 9 to position 60
	if (message & (bit << (64 - 10)))  newMessage |= (bit << (64 - 52));		//bit at 10 to position 52
	if (message & (bit << (64 - 11)))  newMessage |= (bit << (64 - 44));		//bit at 11 to position 44
	if (message & (bit << (64 - 12)))  newMessage |= (bit << (64 - 36));		//bit at 12 to position 36
	if (message & (bit << (64 - 13)))  newMessage |= (bit << (64 - 28));		//bit at 13 to position 28
	if (message & (bit << (64 - 14)))  newMessage |= (bit << (64 - 20));		//bit at 14 to position 20
	if (message & (bit << (64 - 15)))  newMessage |= (bit << (64 - 12));		//bit at 15 to position 12
	if (message & (bit << (64 - 16)))  newMessage |= (bit << (64 - 4));			//bit at 16 to position 4
	if (message & (bit << (64 - 17)))  newMessage |= (bit << (64 - 62));		//bit at 17 to position 62
	if (message & (bit << (64 - 18)))  newMessage |= (bit << (64 - 54));		//bit at 18 to position 54
	if (message & (bit << (64 - 19)))  newMessage |= (bit << (64 - 46));		//bit at 19 to position 46
	if (message & (bit << (64 - 20)))  newMessage |= (bit << (64 - 38));		//bit at 20 to position 38
	if (message & (bit << (64 - 21)))  newMessage |= (bit << (64 - 30));		//bit at 21 to position 30
	if (message & (bit << (64 - 22)))  newMessage |= (bit << (64 - 22));		//bit at 22 to position 22
	if (message & (bit << (64 - 23)))  newMessage |= (bit << (64 - 14));		//bit at 23 to position 14
	if (message & (bit << (64 - 24)))  newMessage |= (bit << (64 - 6));			//bit at 24 to position 6
	if (message & (bit << (64 - 25)))  newMessage |= (bit << (64 - 64));		//bit at 25 to position 64
	if (message & (bit << (64 - 26)))  newMessage |= (bit << (64 - 56));		//bit at 26 to position 56
	if (message & (bit << (64 - 27)))  newMessage |= (bit << (64 - 48));		//bit at 27 to position 48
	if (message & (bit << (64 - 28)))  newMessage |= (bit << (64 - 40));		//bit at 28 to position 40
	if (message & (bit << (64 - 29)))  newMessage |= (bit << (64 - 32));		//bit at 29 to position 32
	if (message & (bit << (64 - 30)))  newMessage |= (bit << (64 - 24));		//bit at 30 to position 24
	if (message & (bit << (64 - 31)))  newMessage |= (bit << (64 - 16));		//bit at 31 to position 16
	if (message & (bit << (64 - 32)))  newMessage |= (bit << (64 - 8));			//bit at 32 to position 8
	if (message & (bit << (64 - 33)))  newMessage |= (bit << (64 - 57));		//bit at 33 to position 57
	if (message & (bit << (64 - 34)))  newMessage |= (bit << (64 - 49));		//bit at 34 to position 49
	if (message & (bit << (64 - 35)))  newMessage |= (bit << (64 - 41));		//bit at 35 to position 41
	if (message & (bit << (64 - 36)))  newMessage |= (bit << (64 - 33));		//bit at 36 to position 33
	if (message & (bit << (64 - 37)))  newMessage |= (bit << (64 - 25));		//bit at 37 to position 25
	if (message & (bit << (64 - 38)))  newMessage |= (bit << (64 - 17));		//bit at 38 to position 17
	if (message & (bit << (64 - 39)))  newMessage |= (bit << (64 - 9));			//bit at 39 to position 9
	if (message & (bit << (64 - 40)))  newMessage |= (bit << (64 - 1));			//bit at 40 to position 1
	if (message & (bit << (64 - 41)))  newMessage |= (bit << (64 - 59));		//bit at 41 to position 59
	if (message & (bit << (64 - 42)))  newMessage |= (bit << (64 - 51));		//bit at 42 to position 51
	if (message & (bit << (64 - 43)))  newMessage |= (bit << (64 - 43));		//bit at 43 to position 43
	if (message & (bit << (64 - 44)))  newMessage |= (bit << (64 - 35));		//bit at 44 to position 35
	if (message & (bit << (64 - 45)))  newMessage |= (bit << (64 - 27));		//bit at 45 to position 27
	if (message & (bit << (64 - 46)))  newMessage |= (bit << (64 - 19));		//bit at 46 to position 19
	if (message & (bit << (64 - 47)))  newMessage |= (bit << (64 - 11));		//bit at 47 to position 11
	if (message & (bit << (64 - 48)))  newMessage |= (bit << (64 - 3));			//bit at 48 to position 3
	if (message & (bit << (64 - 49)))  newMessage |= (bit << (64 - 61));		//bit at 49 to position 61
	if (message & (bit << (64 - 50)))  newMessage |= (bit << (64 - 53));		//bit at 50 to position 53
	if (message & (bit << (64 - 51)))  newMessage |= (bit << (64 - 45));		//bit at 51 to position 45
	if (message & (bit << (64 - 52)))  newMessage |= (bit << (64 - 37));		//bit at 52 to position 37
	if (message & (bit << (64 - 53)))  newMessage |= (bit << (64 - 29));		//bit at 53 to position 29
	if (message & (bit << (64 - 54)))  newMessage |= (bit << (64 - 21));		//bit at 54 to position 21
	if (message & (bit << (64 - 55)))  newMessage |= (bit << (64 - 13));		//bit at 55 to position 13
	if (message & (bit << (64 - 56)))  newMessage |= (bit << (64 - 5));			//bit at 56 to position 5
	if (message & (bit << (64 - 57)))  newMessage |= (bit << (64 - 63));		//bit at 57 to position 63
	if (message & (bit << (64 - 58)))  newMessage |= (bit << (64 - 55));		//bit at 58 to position 55
	if (message & (bit << (64 - 59)))  newMessage |= (bit << (64 - 47));		//bit at 59 to position 47
	if (message & (bit << (64 - 60)))  newMessage |= (bit << (64 - 39));		//bit at 60 to position 39
	if (message & (bit << (64 - 61)))  newMessage |= (bit << (64 - 31));		//bit at 61 to position 31
	if (message & (bit << (64 - 62)))  newMessage |= (bit << (64 - 23));		//bit at 62 to position 23
	if (message & (bit << (64 - 63)))  newMessage |= (bit << (64 - 15));		//bit at 63 to position 15
	if (message & (bit << (64 - 64)))  newMessage |= (bit << (64 - 7));			//bit at 64 to position 7
	return newMessage;
}

unsigned long long desRound(unsigned long long message, unsigned long long roundKey)
{
	unsigned long long messageLeft = message >> 32;
	unsigned long long messageRight = message & 0x0FFFFFFFF;
	unsigned long long scrambler = messageRight;
	unsigned long long newScrambler = 0;	//used for temporarily storing scrambler changes
	unsigned long long bit = 1;
	
	/*EXPANSION P-BOX*/
	if (scrambler & (bit << (32 - 1))) { newScrambler |= (bit << (48 - 2)); newScrambler |= (bit << (48 - 48));}	//bit at 1 to position 2, 48
	if (scrambler & (bit << (32 - 2)))  newScrambler |= (bit << (48 - 3));											//bit at 2 to position 3
	if (scrambler & (bit << (32 - 3)))  newScrambler |= (bit << (48 - 4));											//bit at 3 to position 4
	if (scrambler & (bit << (32 - 4))) { newScrambler |= (bit << (48 - 5)); newScrambler |= (bit << (48 - 7)); }	//bit at 4 to position 5, 7
	if (scrambler & (bit << (32 - 5))) { newScrambler |= (bit << (48 - 6)); newScrambler |= (bit << (48 - 8)); }	//bit at 5 to position 6,8
	if (scrambler & (bit << (32 - 6)))  newScrambler |= (bit << (48 - 9));											//bit at 6 to position 9
	if (scrambler & (bit << (32 - 7)))  newScrambler |= (bit << (48 - 10));											//bit at 7 to position 10
	if (scrambler & (bit << (32 - 8))) { newScrambler |= (bit << (48 - 11)); newScrambler |= (bit << (48 - 13)); };	//bit at 8 to position 11, 13
	if (scrambler & (bit << (32 - 9))) { newScrambler |= (bit << (48 - 12)); newScrambler |= (bit << (48 - 14)); }	//bit at 9 to position 12, 14
	if (scrambler & (bit << (32 - 10)))  newScrambler |= (bit << (48 - 15));										//bit at 10 to position 15
	if (scrambler & (bit << (32 - 11)))  newScrambler |= (bit << (48 - 16));										//bit at 11 to position 16
	if (scrambler & (bit << (32 - 12))) { newScrambler |= (bit << (48 - 17)); newScrambler |= (bit << (48 - 19)); }	//bit at 12 to position 17, 19
	if (scrambler & (bit << (32 - 13))) { newScrambler |= (bit << (48 - 18)); newScrambler |= (bit << (48 - 20)); }	//bit at 13 to position 18, 20
	if (scrambler & (bit << (32 - 14)))  newScrambler |= (bit << (48 - 21));										//bit at 14 to position 21
	if (scrambler & (bit << (32 - 15)))  newScrambler |= (bit << (48 - 22));										//bit at 15 to position 22
	if (scrambler & (bit << (32 - 16))) { newScrambler |= (bit << (48 - 23)); newScrambler |= (bit << (48 - 25)); }	//bit at 16 to position 23, 25
	if (scrambler & (bit << (32 - 17))) { newScrambler |= (bit << (48 - 24)); newScrambler |= (bit << (48 - 26)); }	//bit at 17 to position 24, 26
	if (scrambler & (bit << (32 - 18)))  newScrambler |= (bit << (48 - 27));										//bit at 18 to position 27
	if (scrambler & (bit << (32 - 19)))  newScrambler |= (bit << (48 - 28));										//bit at 19 to position 28
	if (scrambler & (bit << (32 - 20))) { newScrambler |= (bit << (48 - 29)); newScrambler |= (bit << (48 - 31)); };//bit at 20 to position 29, 31
	if (scrambler & (bit << (32 - 21))) { newScrambler |= (bit << (48 - 30)); newScrambler |= (bit << (48 - 32)); }	//bit at 21 to position 30, 32
	if (scrambler & (bit << (32 - 22)))  newScrambler |= (bit << (48 - 33));										//bit at 22 to position 33
	if (scrambler & (bit << (32 - 23)))  newScrambler |= (bit << (48 - 34));										//bit at 23 to position 34
	if (scrambler & (bit << (32 - 24))) { newScrambler |= (bit << (48 - 35)); newScrambler |= (bit << (48 - 37)); }	//bit at 24 to position 35, 37
	if (scrambler & (bit << (32 - 25))) { newScrambler |= (bit << (48 - 36)); newScrambler |= (bit << (48 - 38)); }	//bit at 25 to position 36, 38
	if (scrambler & (bit << (32 - 26)))  newScrambler |= (bit << (48 - 39));										//bit at 26 to position 39
	if (scrambler & (bit << (32 - 27)))  newScrambler |= (bit << (48 - 40));										//bit at 27 to position 40
	if (scrambler & (bit << (32 - 28))) { newScrambler |= (bit << (48 - 41)); newScrambler |= (bit << (48 - 43)); }	//bit at 28 to position 41, 43
	if (scrambler & (bit << (32 - 29))) { newScrambler |= (bit << (48 - 42)); newScrambler |= (bit << (48 - 44)); }	//bit at 29 to position 42, 44
	if (scrambler & (bit << (32 - 30)))  newScrambler |= (bit << (48 - 45));										//bit at 30 to position 45
	if (scrambler & (bit << (32 - 31)))  newScrambler |= (bit << (48 - 46));										//bit at 31 to position 46
	if (scrambler & (bit << (32 - 32))) { newScrambler |= (bit << (48 - 47)); newScrambler |= (bit << (48 - 1)); }	//bit at 32 to position 47, 1

	/*XOR WITH KEY*/
	scrambler = newScrambler ^ roundKey;
	newScrambler = 0;
	
	/*S-BOX 1*/
	//grabs bits 1-6
	//make the array selector for the substitution
	unsigned long long group = (scrambler >> 42) & 0x00000000003F;
	unsigned long long row = (group & 0x20) >> 4 | (group & 0x01);
	unsigned long long col = (group & 0x10) >> 1 | (group & 0x08) >> 1 | (group & 0x04) >> 1 | (group & 0x02) >> 1;
	unsigned long long selector = row * 16 + col;
	newScrambler |= sbox1[selector] << (32 - 4);

	/*S-BOX 2*/
	//grabs bits 7-12
	//make the array selector for the substitution
	group = (scrambler >> 36) & 0x00000000003F;
	row = (group & 0x20) >> 4 | (group & 0x01);
	col = (group & 0x10) >> 1 | (group & 0x08) >> 1 | (group & 0x04) >> 1 | (group & 0x02) >> 1;
	selector = row * 16 + col;
	newScrambler |= sbox2[selector] << (32 - 8);

	/*S-BOX 3*/
	//grabs bits 13-18
	//make the array selector for the substitution
	group = (scrambler >> 30) & 0x00000000003F;
	row = (group & 0x20) >> 4 | (group & 0x01);
	col = (group & 0x10) >> 1 | (group & 0x08) >> 1 | (group & 0x04) >> 1 | (group & 0x02) >> 1;
	selector = row * 16 + col;
	newScrambler |= sbox3[selector] << (32 - 12);


	/*S-BOX 4*/
	//grabs bits 19-24
	//make the array selector for the substitution
	group = (scrambler >> 24) & 0x00000000003F;
	row = (group & 0x20) >> 4 | (group & 0x01);				//bit 1 & bit 6
	col = (group & 0x10) >> 1 | (group & 0x08) >> 1 | (group & 0x04) >> 1 | (group & 0x02) >> 1;
	selector = row * 16 + col;
	newScrambler |= sbox4[selector] << (32 - 16);


	/*S-BOX 5*/
	//grabs bits 25-30
	//make the array selector for the substitution
	group = (scrambler >> 18) & 0x00000000003F;
	row = (group & 0x20) >> 4 | (group & 0x01);
	col = (group & 0x10) >> 1 | (group & 0x08) >> 1 | (group & 0x04) >> 1 | (group & 0x02) >> 1;
	selector = row * 16 + col;
	newScrambler |= sbox5[selector] << (32 - 20);


	/*S-BOX 6*/
	//grabs bits 31-36
	//make the array selector for the substitution
	group = (scrambler >> 12) & 0x00000000003F;
	row = (group & 0x20) >> 4 | (group & 0x01);
	col = (group & 0x10) >> 1 | (group & 0x08) >> 1 | (group & 0x04) >> 1 | (group & 0x02) >> 1;
	selector = row * 16 + col;
	newScrambler |= sbox6[selector] << (32 - 24);


	/*S-BOX 7*/
	//grabs bits 37-42
	//make the array selector for the substitution
	group = (scrambler >> 6) & 0x00000000003F;
	row = (group & 0x20) >> 4 | (group & 0x01);
	col = (group & 0x10) >> 1 | (group & 0x08) >> 1 | (group & 0x04) >> 1 | (group & 0x02) >> 1;
	selector = row * 16 + col;
	newScrambler |= sbox7[selector] << (32 - 28);

	/*S-BOX 8*/
	//grabs bits 43-48
	//make the array selector for the substitution
	group = (scrambler >> 0) & 0x00000000003F;
	row = (group & 0x20) >> 4 | (group & 0x01);
	col = (group & 0x10) >> 1 | (group & 0x08) >> 1 | (group & 0x04) >> 1 | (group & 0x02) >> 1;
	selector = row * 16 + col;
	newScrambler |= sbox8[selector] << (32 - 32);

	scrambler = newScrambler;
	newScrambler = 0;

	/*Post S-Box Right-Side Permutation*/
	if (scrambler & (bit << (32 - 1)))  newScrambler |= (bit << (32 - 9));		//bit at 1 to position 9
	if (scrambler & (bit << (32 - 2)))  newScrambler |= (bit << (32 - 17));		//bit at 2 to position 17
	if (scrambler & (bit << (32 - 3)))  newScrambler |= (bit << (32 - 23));		//bit at 3 to position 23
	if (scrambler & (bit << (32 - 4)))  newScrambler |= (bit << (32 - 31));		//bit at 4 to position 31
	if (scrambler & (bit << (32 - 5)))  newScrambler |= (bit << (32 - 13));		//bit at 5 to position 13
	if (scrambler & (bit << (32 - 6)))  newScrambler |= (bit << (32 - 28));		//bit at 6 to position 28
	if (scrambler & (bit << (32 - 7)))  newScrambler |= (bit << (32 - 2));		//bit at 7 to position 2
	if (scrambler & (bit << (32 - 8)))  newScrambler |= (bit << (32 - 18));		//bit at 8 to position 18
	if (scrambler & (bit << (32 - 9)))  newScrambler |= (bit << (32 - 24));		//bit at 9 to position 24
	if (scrambler & (bit << (32 - 10)))  newScrambler |= (bit << (32 - 16));		//bit at 10 to position 16
	if (scrambler & (bit << (32 - 11)))  newScrambler |= (bit << (32 - 30));		//bit at 11 to position 30
	if (scrambler & (bit << (32 - 12)))  newScrambler |= (bit << (32 - 6));		//bit at 12 to position 6
	if (scrambler & (bit << (32 - 13)))  newScrambler |= (bit << (32 - 26));		//bit at 13 to position 26
	if (scrambler & (bit << (32 - 14)))  newScrambler |= (bit << (32 - 20));		//bit at 14 to position 20
	if (scrambler & (bit << (32 - 15)))  newScrambler |= (bit << (32 - 10));		//bit at 15 to position 10
	if (scrambler & (bit << (32 - 16)))  newScrambler |= (bit << (32 - 1));		//bit at 16 to position 1
	if (scrambler & (bit << (32 - 17)))  newScrambler |= (bit << (32 - 8));		//bit at 17 to position 8
	if (scrambler & (bit << (32 - 18)))  newScrambler |= (bit << (32 - 14));		//bit at 18 to position 14
	if (scrambler & (bit << (32 - 19)))  newScrambler |= (bit << (32 - 25));		//bit at 19 to position 25
	if (scrambler & (bit << (32 - 20)))  newScrambler |= (bit << (32 - 3));		//bit at 20 to position 3
	if (scrambler & (bit << (32 - 21)))  newScrambler |= (bit << (32 - 4));		//bit at 21 to position 4
	if (scrambler & (bit << (32 - 22)))  newScrambler |= (bit << (32 - 29));		//bit at 22 to position 29
	if (scrambler & (bit << (32 - 23)))  newScrambler |= (bit << (32 - 11));		//bit at 23 to position 11
	if (scrambler & (bit << (32 - 24)))  newScrambler |= (bit << (32 - 19));		//bit at 24 to position 19
	if (scrambler & (bit << (32 - 25)))  newScrambler |= (bit << (32 - 32));		//bit at 25 to position 32
	if (scrambler & (bit << (32 - 26)))  newScrambler |= (bit << (32 - 12));		//bit at 26 to position 12
	if (scrambler & (bit << (32 - 27)))  newScrambler |= (bit << (32 - 22));		//bit at 27 to position 22
	if (scrambler & (bit << (32 - 28)))  newScrambler |= (bit << (32 - 7));		//bit at 28 to position 7
	if (scrambler & (bit << (32 - 29)))  newScrambler |= (bit << (32 - 5));		//bit at 29 to position 5
	if (scrambler & (bit << (32 - 30)))  newScrambler |= (bit << (32 - 27));		//bit at 30 to position 27
	if (scrambler & (bit << (32 - 31)))  newScrambler |= (bit << (32 - 15));		//bit at 31 to position 15
	if (scrambler & (bit << (32 - 32)))  newScrambler |= (bit << (32 - 21));		//bit at 32 to position 21

	scrambler = messageRight;	//we're done using scrambler - I'm just using it here to temporarily hold the right half to swap
	/*CREATE NEW RIGHT HALF*/
	messageRight = messageLeft ^ newScrambler;
	messageLeft = scrambler;

	/*OUTPUT IS RIGHT AND LEFT HALVES PUT BACK TOGETHER*/
	return (messageLeft << 32 | messageRight);
}

/*Precondition: A message that needs encrypted with DES
Postcondition: A encrypted message*/
unsigned long long desEncrypt(unsigned long long message) 
{
	unsigned long long left = 0;
	/*Initial Permutation*/
	message = initialP(message);
	/*DES Round*/
	message = desRound(message, roundKeys[1]);
	message = desRound(message, roundKeys[2]);
	message = desRound(message, roundKeys[3]);
	message = desRound(message, roundKeys[4]);
	message = desRound(message, roundKeys[5]);
	message = desRound(message, roundKeys[6]);
	message = desRound(message, roundKeys[7]);
	message = desRound(message, roundKeys[8]);
	message = desRound(message, roundKeys[9]);
	message = desRound(message, roundKeys[10]);
	message = desRound(message, roundKeys[11]);
	message = desRound(message, roundKeys[12]);
	message = desRound(message, roundKeys[13]);
	message = desRound(message, roundKeys[14]);
	message = desRound(message, roundKeys[15]);
	message = desRound(message, roundKeys[16]);
	//swap left & right halves
	left = message >> 32;
	message <<= 32;
	message |= left;
	/*Final Permutation*/
	message = finalP(message);
	return message;	
}

/*Precondition: A message that needs decrypted with DES
Postcondition: A decrypted message*/
unsigned long long desDecrypt(unsigned long long message)
{
	unsigned long long left = 0;
	/*Initial Permutation*/
	message = initialP(message);
	//Rounds
	message = desRound(message, roundKeys[16]);
	message = desRound(message, roundKeys[15]);
	message = desRound(message, roundKeys[14]);
	message = desRound(message, roundKeys[13]);
	message = desRound(message, roundKeys[12]);
	message = desRound(message, roundKeys[11]);
	message = desRound(message, roundKeys[10]);
	message = desRound(message, roundKeys[9]);
	message = desRound(message, roundKeys[8]);
	message = desRound(message, roundKeys[7]);
	message = desRound(message, roundKeys[6]);
	message = desRound(message, roundKeys[5]);
	message = desRound(message, roundKeys[4]);
	message = desRound(message, roundKeys[3]);
	message = desRound(message, roundKeys[2]);
	message = desRound(message, roundKeys[1]);
	//swap left & right halves
	left = message >> 32;
	message <<= 32;
	message |= left;
	/*Final Permutation*/
	message = finalP(message);
	return message;
}

int main(int argc, char * argv[])
{
	clock_t start = clock();	//start a timer
	//Go through command line parameters and make sure it's all hunky-dory
	//Accepted command line input
	//<–action> <key> <mode> <infile> <outfile>
	int length = argc;
	if (length != 6) {			//did we receive the correct amount of arguments
		cerr << "Error: Bad input. This program accepts the following arguments: <action> <key> <mode> <infile> <outfile>\n";
		exit(1);				//exit 1 is bad input
	}

	//Are we encrypting or decrypting?
	string arg1 = argv[1];
	bool isEncrypt = true;
	if (arg1 == "-d" || arg1 == "-D")
		isEncrypt = false;
	else if (arg1 != "-e" && arg1 != "-E") 
	{
		cerr << "Error: Bad input: Action must be specified as -e or -d. <action> <key> <mode> <infile> <outfile>\n";
		exit(1);
	}

	//get the key
	unsigned long long key = prepUserKey(argv[2]);
	//Create K0
	key = makeK0(key);
	//Create Round Keys
	makeRoundKeys(key, roundKeys);

	//get the mode
	string temp = argv[3];
	string mode = "";
	bool isECB = true;
	for (short i = 0; i < temp.length(); i++)	//make everything lowercase
		mode += tolower(temp[i]);

	if (mode != "ecb")
	{
		cerr << "Error: Bad Input. ECB mode is only accepted. <action> <key> <mode> <infile> <outfile>\n";
		exit(1);
	}

	//open input and output streams
	string infile = argv[4];
	string outfile = argv[5];
	try
	{
		unsigned long long temp1 = 0;	//for whatever purposes I deem worthy
		unsigned long long fileLength = 0;	//contains length of file - wow

		ifstream inputText(infile, ifstream::binary);
		unsigned char buffer[8];
		if (!inputText.is_open())	//see if it's open
		{
			cerr << "Error: File cannot be read.";
			exit(1);
		}

		//output stream setup
		ofstream outputText(outfile, ofstream::binary);
		outputText.exceptions(ofstream::failbit | ofstream::badbit);

		//Need to do stuff with file length dependent on whether we are encrypting or decrypting
		if(isEncrypt)
		{
			//get length of file, pad on left, encrypt, and write to file
			inputText.seekg(0, inputText.end);
			fileLength = inputText.tellg();
			inputText.seekg(0, inputText.beg);

			srand(start);
			temp1 = ((unsigned long long)rand() % 256 << 32 | (unsigned long long)rand() % 256 << 24 | (unsigned long long)rand() % 256 << 16 
					| (unsigned long long)rand() % 256 << 8 | (unsigned long long)rand() % 256) << 32;
			unsigned long long fileLengthNGarbage = (temp1 | fileLength);
			fileLengthNGarbage = desEncrypt(fileLengthNGarbage);

			//Prep new message for output
			buffer[0] = (fileLengthNGarbage >> (64 - 8)) & 0x00000000000000FF;
			buffer[1] = (fileLengthNGarbage >> (64 - 16)) & 0x00000000000000FF;
			buffer[2] = (fileLengthNGarbage >> (64 - 24)) & 0x00000000000000FF;
			buffer[3] = (fileLengthNGarbage >> (64 - 32)) & 0x00000000000000FF;
			buffer[4] = (fileLengthNGarbage >> (64 - 40)) & 0x00000000000000FF;
			buffer[5] = (fileLengthNGarbage >> (64 - 48)) & 0x00000000000000FF;
			buffer[6] = (fileLengthNGarbage >> (64 - 56)) & 0x00000000000000FF;
			buffer[7] = (fileLengthNGarbage >> (64 - 64)) & 0x00000000000000FF;
			outputText.write(reinterpret_cast<char*>(&buffer), 8);

			//Do DES Stuff here
			while (inputText.read(reinterpret_cast<char*>(&buffer), 8)) {
				//Process 64 bits of input into usable form for DES
				temp1 = (unsigned long long)(buffer[0]) << (64 - 8);
				temp1 |= (unsigned long long)(buffer[1]) << (64 - 16);
				temp1 |= (unsigned long long)(buffer[2]) << (64 - 24);
				temp1 |= (unsigned long long)(buffer[3]) << (64 - 32);
				temp1 |= (unsigned long long)(buffer[4]) << (64 - 40);
				temp1 |= (unsigned long long)(buffer[5]) << (64 - 48);
				temp1 |= (unsigned long long)(buffer[6]) << (64 - 56);
				temp1 |= (unsigned long long)(buffer[7]);
				temp1 = desEncrypt(temp1);							//encrypt/decrypt with DES

				//Prep new message for output
				buffer[0] = (temp1 >> (64 - 8)) & 0x00000000000000FF;
				buffer[1] = (temp1 >> (64 - 16)) & 0x00000000000000FF;
				buffer[2] = (temp1 >> (64 - 24)) & 0x00000000000000FF;
				buffer[3] = (temp1 >> (64 - 32)) & 0x00000000000000FF;
				buffer[4] = (temp1 >> (64 - 40)) & 0x00000000000000FF;
				buffer[5] = (temp1 >> (64 - 48)) & 0x00000000000000FF;
				buffer[6] = (temp1 >> (64 - 56)) & 0x00000000000000FF;
				buffer[7] = (temp1 >> (64 - 64)) & 0x00000000000000FF;
				outputText.write(reinterpret_cast<char*>(&buffer), 8);
			}
			//check last read to see how many bytes came
			//pad and encrypt
			int bytesLeft = inputText.gcount();
			if (bytesLeft > 0)
			{
				temp1 = 0;
				//Process 64 bits using DES
				switch (bytesLeft) {
				case 7: temp1 |= (unsigned long long)(buffer[6]) << (64 - 56);
				case 6: temp1 |= (unsigned long long)(buffer[5]) << (64 - 48);
				case 5: temp1 |= (unsigned long long)(buffer[4]) << (64 - 40);
				case 4: temp1 |= (unsigned long long)(buffer[3]) << (64 - 32);
				case 3: temp1 |= (unsigned long long)(buffer[2]) << (64 - 24);
				case 2: temp1 |= (unsigned long long)(buffer[1]) << (64 - 16);
				case 1: temp1 |= (unsigned long long)(buffer[0]) << (64 - 8);
				}

				switch (bytesLeft) {
				case 1: temp1 |= (unsigned long long)(rand() % 256) << (64 - 16);
				case 2: temp1 |= (unsigned long long)(rand() % 256) << (64 - 24);
				case 3: temp1 |= (unsigned long long)(rand() % 256) << (64 - 32);
				case 4: temp1 |= (unsigned long long)(rand() % 256) << (64 - 40);
				case 5: temp1 |= (unsigned long long)(rand() % 256) << (64 - 48);
				case 6: temp1 |= (unsigned long long)(rand() % 256) << (64 - 56);
				default: temp1 |= (unsigned long long)(rand() % 256) << (64 - 64);
				}

				temp1 = desEncrypt(temp1);

				//Prep new message for output
				buffer[0] = (temp1 >> (64 - 8)) & 0x00000000000000FF;
				buffer[1] = (temp1 >> (64 - 16)) & 0x00000000000000FF;
				buffer[2] = (temp1 >> (64 - 24)) & 0x00000000000000FF;
				buffer[3] = (temp1 >> (64 - 32)) & 0x00000000000000FF;
				buffer[4] = (temp1 >> (64 - 40)) & 0x00000000000000FF;
				buffer[5] = (temp1 >> (64 - 48)) & 0x00000000000000FF;
				buffer[6] = (temp1 >> (64 - 56)) & 0x00000000000000FF;
				buffer[7] = (temp1 >> (64 - 64)) & 0x00000000000000FF;
				outputText.write(reinterpret_cast<char*>(&buffer), 8);
			}
		}
		else //decrypt
		{
			//read in first 8 bytes, decrypt, throw away left half, get file size from right
			inputText.read(reinterpret_cast<char*>(&buffer), 8);
			temp1 = (unsigned long long)(buffer[0]) << (64 - 8);
			temp1 |= (unsigned long long)(buffer[1]) << (64 - 16);
			temp1 |= (unsigned long long)(buffer[2]) << (64 - 24);
			temp1 |= (unsigned long long)(buffer[3]) << (64 - 32);
			temp1 |= (unsigned long long)(buffer[4]) << (64 - 40);
			temp1 |= (unsigned long long)(buffer[5]) << (64 - 48);
			temp1 |= (unsigned long long)(buffer[6]) << (64 - 56);
			temp1 |= (unsigned long long)(buffer[7]);

			temp1 = desDecrypt(temp1);
			fileLength = (unsigned int) temp1 & 0x00000000FFFFFFFF;

			//Do DES Stuff here
			bool isNotLastBlock = true;
			int count = 0;
			while (inputText.read(reinterpret_cast<char*>(&buffer), 8)) {
				//Process 64 bits of input into usable form for DES
				temp1 = (unsigned long long)(buffer[0]) << (64 - 8);
				temp1 |= (unsigned long long)(buffer[1]) << (64 - 16);
				temp1 |= (unsigned long long)(buffer[2]) << (64 - 24);
				temp1 |= (unsigned long long)(buffer[3]) << (64 - 32);
				temp1 |= (unsigned long long)(buffer[4]) << (64 - 40);
				temp1 |= (unsigned long long)(buffer[5]) << (64 - 48);
				temp1 |= (unsigned long long)(buffer[6]) << (64 - 56);
				temp1 |= (unsigned long long)(buffer[7]);
				temp1 = desDecrypt(temp1);							//encrypt/decrypt with DES
				

				//Prep new message for output
				buffer[0] = (temp1 >> (64 - 8)) & 0x00000000000000FF;
				buffer[1] = (temp1 >> (64 - 16)) & 0x00000000000000FF;
				buffer[2] = (temp1 >> (64 - 24)) & 0x00000000000000FF;
				buffer[3] = (temp1 >> (64 - 32)) & 0x00000000000000FF;
				buffer[4] = (temp1 >> (64 - 40)) & 0x00000000000000FF;
				buffer[5] = (temp1 >> (64 - 48)) & 0x00000000000000FF;
				buffer[6] = (temp1 >> (64 - 56)) & 0x00000000000000FF;
				buffer[7] = (temp1 >> (64 - 64)) & 0x00000000000000FF;

				count += 8;
				isNotLastBlock = count <= fileLength;
				if (isNotLastBlock)
					outputText.write(reinterpret_cast<char*>(&buffer), 8);
				else
					outputText.write(reinterpret_cast<char*>(&buffer), fileLength % 8);
			}
		}

		//close streams
		inputText.close();
		outputText.close();
	}
	catch (exception e)
	{
		cerr << e.what() << endl;
		exit(1);
	}

	clock_t runtime = clock() - start;
	cout << runtime / 1000 << "." << runtime % 1000 << " s"<< endl;
}//close main

