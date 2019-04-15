#include "pch.h"
#include "BigInt.h"

BigInt::BigInt(string myInt)
{
	integer = myInt;
}

BigInt::BigInt()
{
	integer = "";
}

BigInt::~BigInt()
{
}

/*Precondition: Given two BigInts 
Postcondition: Sum of the BigInts*/
const BigInt operator+(const BigInt& A, const BigInt& B)
{
	unsigned char digitA = 0, digitB = 0, carry = 0, sum = 0;
	string finalAnswer = "";
	int locA = A.integer.length() - 1, locB = B.integer.length() - 1;
	for (locA, locB; (locA >= 0 && locB >= 0); locA--, locB--) {
		digitA = A.integer[locA] & 0x0F;				//get digits to add from A and B
		digitB = B.integer[locB] & 0x0F;

		sum = (digitA + digitB + carry);				//Add digits and generate carry value
		carry = 0;
		if (sum > 9) { sum -= 10; carry += 1;}
		finalAnswer = to_string(sum) + finalAnswer;		//append current sum to answer
	}

	if (locA >= 0) {												//any digits leftover?
		while(carry != 0){
			sum = ((A.integer[locA] & 0x0F) + carry) ;
			carry = 0;
			if (sum > 9) { sum -= 10; carry += 1; }
			finalAnswer = to_string(sum) + finalAnswer;
			locA--;
		}
		finalAnswer = A.integer.substr(0, locA) + finalAnswer;		//when there is no more carry, just append remaining digits
	}
	else if (locB >= 0) {											//any digits leftover?
		while (carry != 0) {
			sum = ((B.integer[locB] & 0x0F) + carry);
			carry = 0;
			if (sum > 9) { sum -= 10; carry += 1; }
			finalAnswer = to_string(sum) + finalAnswer;
			locB--;
		}
		finalAnswer = B.integer.substr(0, locB+1) + finalAnswer;	//when there is no more carry, just append remaining digits
	}
	return BigInt(finalAnswer);
}

const BigInt BigInt::operator-(BigInt A) const
{
	return BigInt();
}

const BigInt BigInt::operator*(BigInt A) const
{
	return BigInt();
}

const BigInt BigInt::operator/(BigInt A) const
{
	return BigInt();
}

const BigInt BigInt::operator%(BigInt A) const
{
	return BigInt();
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is less than B*/
bool operator <(const BigInt& A, const BigInt& B)
{
	/*Look at the lengths first. If A is longer it is larger than B.*/
	if ((A.integer).length() > (B.integer).length())		return false;
	else if ((A.integer).length() < (B.integer).length()) return true;

	//start comparing digits
	for (int i = 0; i < (A.integer).length(); i++) {
		if ((A.integer)[i] > (B.integer)[i])			return false;		//if a digit in A is greater than the digit in B, A is larger. 
		else if ((A.integer)[i] == (B.integer)[i])	continue;			//if the digit in A is equal to the digit in B continue
		else											return true;		//the digit in A must be less than the one in B. A is smaller.
	}
	return false;
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is less than or equal to B*/
bool operator <=(const BigInt& A, const BigInt& B)
{
	/*Look at the lengths first. If A is longer it is larger than B.*/
	if ((A.integer).length() > (B.integer).length())	return false;
	else if ((A.integer).length() < (B.integer).length()) return true;

	//start comparing digits
	for (int i = 0; i < (A.integer).length(); i++) {
		if ((A.integer)[i] > (B.integer)[i])			return false;		//if a digit in A is greater than the digit in B, A is larger. 
		else if ((A.integer)[i] == (B.integer)[i])	continue;			//if the digit in A is equal to the digit in B continue
		else											return true;		//the digit in A must be less than the one in B. A is smaller.
	}
	return true;
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is greater than B*/
bool operator >(const BigInt& A, const BigInt& B)
{
	/*Look at the lengths first. If A is longer it is larger than B.*/
	if ((A.integer).length() > (B.integer).length())	return true;
	else if ((A.integer).length() < (B.integer).length()) return false;

	//start comparing digits
	for (int i = 0; i < (A.integer).length(); i++) {
		if ((A.integer)[i] > (B.integer)[i])			return true;		//if a digit in A is greater than the digit in B, A is larger. 
		else if ((A.integer)[i] == (B.integer)[i])	continue;			//if the digit in A is equal to the digit in B continue
		else											return false;		//the digit in A must be less than the one in B. A is smaller.
	}
	return false;
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is greater than or equal to B*/
bool operator >=(const BigInt& A, const BigInt& B)
{
	/*Look at the lengths first. If A is longer it is larger than B.*/
	if ((A.integer).length() > (B.integer).length())	return true;
	else if ((A.integer).length() < (B.integer).length()) return false;

	//start comparing digits
	for (int i = 0; i < (A.integer).length(); i++) {
		if ((A.integer)[i] > (B.integer)[i])			return true;		//if a digit in A is greater than the digit in B, A is larger. 
		else if ((A.integer)[i] == (B.integer)[i])	continue;			//if the digit in A is equal to the digit in B continue
		else											return false;		//the digit in A must be less than the one in B. A is smaller.
	}
	return true;
}

/*Precondition: Compares A to B
Postcondition: Returns true if A and B are not equal*/
bool operator !=(const BigInt& A, const BigInt& B)
{
	/*Look at the lengths first. If one is longer, they are not equal.*/
	if ((A.integer).length() != (B.integer).length())	return true;

	//start comparing digits
	for (int i = 0; i < (A.integer).length(); i++) {
		if ((A.integer)[i] == (B.integer)[i])	continue;
		else								return true;
	}
	return false;
}

/*Precondition: Compares A to B
Postcondition: Returns true if A == B*/
bool operator ==(const BigInt& A, const BigInt& B)
{
	/*Look at the lengths first. If one is longer, they are not equal.*/
	if ((A.integer).length() != (B.integer).length())	return false;

	//start comparing digits
	for (int i = 0; i < (A.integer).length(); i++) {
		if ((A.integer)[i] == (B.integer)[i])	continue;
		else								return false;
	}
	return true;
}

/*Given a string, set the big integer to the new value*/
void BigInt::setInt(string A)
{
	integer = A;
}


/*Returns the binary representation of an integer*/
//TO-DO
string BigInt::bin(BigInt a)
{	
	string binaryRep = "";
	return binaryRep;
	/*find the largest power of 2 that fits within the integer*/
}

/*Precondition: given a big int
Postcondition: returns bool representing whether that big int is even or not*/
bool BigInt::isEven(BigInt a)
{
	unsigned char temp = (a.integer)[0];
	if (temp == '0' || temp == '2' || temp == '4' || temp == '6' || temp == '8') return true;
	else																		 return false;
}
