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
	friend const BigInt operator-( BigInt A, BigInt B);
	friend const BigInt operator-( BigInt A, string B);
	friend const BigInt operator*(BigInt A, BigInt B);
	friend const BigInt operator*(BigInt A, string B);
	friend const BigInt operator/(BigInt& A, BigInt& divisor);
	friend const BigInt operator/(BigInt& A, string divisor);
	friend const BigInt operator%(BigInt& A, BigInt& divisor);
	friend const BigInt operator%(BigInt& A, string B);
	string integer = "";
	friend bool operator <(const BigInt& A, const BigInt& B);
	friend bool operator >(const BigInt& A, const BigInt& B);
	friend bool operator <=(const BigInt& A, const BigInt& B);
	friend bool operator >=(const BigInt& A, const BigInt& B);
	friend bool operator !=(const BigInt& A, const BigInt& B);
	friend bool operator !=(const BigInt& A, string B);
	friend bool operator ==(const BigInt& A, const BigInt& B);
	friend bool operator ==(const BigInt& A, string B);
	BigInt pow(BigInt A, BigInt B, BigInt C);
	BigInt pow(BigInt A, BigInt B, string C);
private:
	unsigned int toDecimal(string num);
};

