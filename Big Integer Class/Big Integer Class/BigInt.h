#pragma once
#include <string>
#include <iostream>
#include "pch.h"
#include <math.h>

using namespace std;

class BigInt
{
public:
	BigInt(string myInt);
	BigInt();
	~BigInt();
	friend const BigInt operator+( BigInt A, BigInt B);
	friend const BigInt operator+(BigInt A, string B);
	friend const BigInt operator-( BigInt A, BigInt B);
	friend const BigInt operator-(BigInt A, string B);
	friend const BigInt operator*(BigInt A, BigInt B);
	friend const BigInt operator*(BigInt A, string B);
	friend const BigInt operator/(BigInt A, BigInt divisor);
	friend const BigInt operator/(BigInt A, string divisor);
	friend const BigInt operator%(BigInt A, BigInt divisor);
	friend const BigInt operator%(BigInt A, string B);
	friend bool operator <( BigInt A, BigInt B);
	friend bool operator <(BigInt A, string B);
	friend bool operator >( BigInt A, BigInt B);
	friend bool operator >(BigInt A, string B);
	friend bool operator <=( BigInt A, BigInt B);
	friend bool operator <=(BigInt A, string B);
	friend bool operator >=( BigInt A, BigInt B);
	friend bool operator >=(BigInt A, string B);
	friend bool operator !=( BigInt A, BigInt B);
	friend bool operator !=( BigInt A, string B);
	friend bool operator ==( BigInt A, BigInt B);
	friend bool operator ==( BigInt A, string B);
	static BigInt pow(BigInt A, BigInt B, BigInt C);
	static BigInt pow(BigInt A, BigInt B, string C);
	string toString();
private:
	static unsigned int toDecimal(string num);
	static string add(string A, string B);
	static string sub(string A, string B);
	static string mul(string A, string B);
	static string div(string A, string divisor);
	static string mod(string A, string B);
	static bool LT(string A, string B);
	static bool LE(string A, string B);
	static bool GT(string A, string B);
	static bool GE(string A, string B);
	static bool NE(string A, string B);
	static bool EQ(string A, string B);
	static string pow(string A, string B, string C);
	string integer = "";
};

