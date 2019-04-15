#pragma once
#include <string>
#include <iostream>
#include "pch.h"

using namespace std;

class BigInt
{
public:
	BigInt(string myInt);
	BigInt();
	~BigInt();
	friend const BigInt operator+(const BigInt& A, const BigInt& B);
	const BigInt operator -(BigInt A) const;
	const BigInt operator *(BigInt A) const;
	const BigInt operator /(BigInt A) const;
	const BigInt operator %(BigInt A) const;
	string integer = "";
	void setInt(string A);
	friend bool operator <(const BigInt& A, const BigInt& B);
	friend bool operator >(const BigInt& A, const BigInt& B);
	friend bool operator <=(const BigInt& A, const BigInt& B);
	friend bool operator >=(const BigInt& A, const BigInt& B);
	friend bool operator !=(const BigInt& A, const BigInt& B);
	friend bool operator ==(const BigInt& A, const BigInt& B);
private:
	string bin(BigInt a);
	bool isEven(BigInt a);
};

