#pragma once
#include <string>
#include "pch.h"

using namespace std;

class BigInt
{
public:
	BigInt();
	~BigInt();
	const BigInt operator +(BigInt A) const;
	const BigInt operator -(BigInt A) const;
	const BigInt operator *(BigInt A) const;
	const BigInt operator /(BigInt A) const;
	const BigInt operator %(BigInt A) const;
	const BigInt operator <(BigInt A) const;
	const BigInt operator <=(BigInt A) const;
	const BigInt operator >(BigInt A) const;
	const BigInt operator >=(BigInt A) const;
	const BigInt operator ==(BigInt A) const;
	const BigInt operator !=(BigInt A) const;
	string getInt(BigInt A);
	string setInt(BigInt A);
private:
	string bin(BigInt a);
	bool isEven(BigInt a);
	string integer = "";
};

