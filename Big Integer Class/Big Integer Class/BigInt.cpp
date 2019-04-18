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
const BigInt operator+(BigInt A, BigInt B)
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
		if(locA >= 0) finalAnswer = A.integer.substr(0, locA+1) + finalAnswer;		//when there is no more carry, just append remaining digits
	}
	else if (locB >= 0) {											//any digits leftover?
		while (carry != 0) {
			sum = ((B.integer[locB] & 0x0F) + carry);
			carry = 0;
			if (sum > 9) { sum -= 10; carry += 1; }
			finalAnswer = to_string(sum) + finalAnswer;
			locB--;
		}
		if (locB >= 0) finalAnswer = B.integer.substr(0, locB+1) + finalAnswer;	//when there is no more carry, just append remaining digits
	}
	else if (carry != 0) { finalAnswer = to_string(carry) + finalAnswer; }
	return BigInt(finalAnswer);
}

const BigInt operator-(BigInt A, BigInt B)
{
	//return error if A is less than B
	if (A < B) { cerr << "Error. A is less than B. Cannot compute."; exit(1); }
	else if (A == B) { return BigInt("0"); }										//We already know the answer so return it.

	unsigned char digitA = 0, digitB = 0, carry = 0, sum = 0, tempA = 0, tempB = 0;
	string finalAnswer = "";
	int locA = A.integer.length() - 1, locB = B.integer.length() - 1, locATemp = 0, locBTemp = 0;

	for (locA, locB; (locA >= 0 && locB >= 0); locA--, locB--) {
		digitA = A.integer[locA] & 0x0F;				//get digits to add from A and B
		digitB = B.integer[locB] & 0x0F;

		if ((digitA >= digitB) & carry == 0) { sum = digitA - digitB; }			//Is digit A larger than digit B? If so subtract
		else if (carry > 1) {
			carry--;
			digitA = (digitA + 10) - 1;
			sum = digitA - digitB;
		}
		else if (carry == 1) {
			carry--;
			digitA--;
			sum = digitA - digitB;
		}
		else {									//If A isn't larger and we have no carry figure out how to carry
			locATemp = locA - 1;
			locBTemp = locB - 1;
			do {								//find the next pair of digits where A is at least one larger than B
				if (locATemp >= 0)  { tempA = A.integer[locATemp--] & 0x0F; }
				if (locBTemp >= 0)	{ tempB = B.integer[locBTemp--] & 0x0F; }
				else { tempB = 0; }
				carry++;
			} while (tempB >= tempA);
			digitA += 10;
			sum = digitA - digitB;
		}
		finalAnswer = to_string(sum) + finalAnswer;		//append current sum to answer
		}

	while (carry > 1) {
		carry--;
		digitA = A.integer[locA] & 0x0F;
		digitA = (digitA + 10) - 1;
		finalAnswer = to_string(digitA) + finalAnswer;
		locA--;
	}
	if (carry == 1) {
		carry--;
		digitA = A.integer[locA] & 0x0F;
		digitA--;
		finalAnswer = to_string(digitA) + finalAnswer;
		locA--;
	}
	if(locA >= 0)	finalAnswer = A.integer.substr(0, locA+1) + finalAnswer;		//when there is no more carry, just append remaining digits

	while (finalAnswer[0] == '0') { finalAnswer = finalAnswer.substr(1); }
	return BigInt(finalAnswer);
}

const BigInt operator-(BigInt A, string B)
{
	BigInt temp(B);
	return (A-temp);
}

unsigned int BigInt::toDecimal(string num)
{
	int length = num.length(), theNum = 0, currentNum = 0;
	int values[10] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};
	for (int i = 0, positionVal = length - 1; i < length && positionVal >= 0; i++, positionVal--) {
		currentNum = num[i] - 0x030;
		theNum += currentNum * values[positionVal];
	}
	return theNum;
}

const BigInt operator*(BigInt A, BigInt B)
{	
	BigInt base("10");
	if (A <= base && B <= base) {
		int aNum = A.toDecimal(A.integer);
		int bNum = A.toDecimal(B.integer);
		int ans = aNum * bNum;
		return BigInt(to_string(ans));
	}

	//split the digit sequences in the middle
	BigInt high1("0");
	BigInt low1("0");
	BigInt high2("0");
	BigInt low2("0");
	//calculates the size of the numbers
	unsigned int m = 0;
	if (A.integer.length() > B.integer.length()) { m = A.integer.length(); }
	else { m = B.integer.length(); }
	if (m % 2 != 0)	m = m / 2 + 1;
	high1.integer = A.integer.substr(0, A.integer.length() - m);
	low1.integer = A.integer.substr(A.integer.length() - m);
	high2.integer = B.integer.substr(0, B.integer.length() - m);
	low2.integer = B.integer.substr(B.integer.length() - m);
	
	//do calculation things
	BigInt z2(high1 * high2);
	BigInt z0(low1 * low2);
	BigInt z1(high1 * low2 + low1 * high2);
	for (int i = 0; i < 2*m; i++)	z2.integer = z2.integer + "0";	//finish z2 calc
	for (int i = 0; i < m; i++)	z1.integer = z1.integer + "0";	//finish z1 calc

	return BigInt(z2 + z1 + z0);
}

const BigInt operator*(BigInt A, string B)
{
	return BigInt(A * BigInt(B));
}

const BigInt operator/(BigInt& A, BigInt& divisor)
{
	unsigned int numODigitsA = A.integer.length(), nextDigit = 1;
	if (A < divisor) { return BigInt("0"); }
	if (divisor == "0") { cerr << "Can't divide by zero."; return BigInt("-1"); }
	BigInt dividend ((A.integer).substr(0, nextDigit));

	//make multiplication table
	BigInt mulTable[11] = {BigInt("0"), BigInt(divisor*"1"), BigInt(divisor*"2"), BigInt(divisor*"3"), BigInt(divisor*"4"), BigInt(divisor*"5"), BigInt(divisor*"6"), BigInt(divisor*"7"), BigInt(divisor*"8"), BigInt(divisor*"9"), BigInt(divisor*"10") };
	BigInt multiple("0");
	int i = 0;
	string quotient = "";

	bool keepDividing = true;

	while(nextDigit < numODigitsA && dividend < divisor ) { dividend.integer = dividend.integer + A.integer[nextDigit++]; }
	while (keepDividing) {
		if (dividend >= divisor)	//is dividend greater than the divisor
		{	//which multiple gets us just below B?
			for (i = 0; i < 11; i++) {
				if (mulTable[i] <= dividend) { multiple.integer = (mulTable[i]).integer; }
				else { break; } 
			}

			//Subtract multiple from dividend
			dividend = dividend - multiple;

			//save quotient
			quotient += to_string(i-1);

			//grab next digit
			if(nextDigit < numODigitsA && dividend != "0") dividend.integer += A.integer[nextDigit++];
			else if (nextDigit < numODigitsA) dividend.integer = A.integer[nextDigit++];
		}
		else {	//divisor is greater, need to grab another digit
			if (nextDigit < numODigitsA && dividend != "0") { dividend.integer = dividend.integer + A.integer[nextDigit++]; quotient += "0"; }
			else if (nextDigit < numODigitsA) { dividend.integer = A.integer[nextDigit++]; quotient += "0"; }
			else keepDividing = false;
		}
	}
	return BigInt(quotient);
}

const BigInt operator/(BigInt & A, string divisor)
{
	BigInt temp(divisor);
	return (A / temp);
}

const BigInt operator%(BigInt& A, BigInt& divisor)
{
	unsigned int numODigitsA = A.integer.length(), nextDigit = 1;
	if (A < divisor) { return A; }
	if (divisor == "0") { cerr << "Can't divide by zero."; return BigInt("-1"); }
	BigInt dividend((A.integer).substr(0, nextDigit));

	//make multiplication table
	BigInt mulTable[11] = { BigInt("0"), BigInt(divisor*"1"), BigInt(divisor*"2"), BigInt(divisor*"3"), BigInt(divisor*"4"), BigInt(divisor*"5"), BigInt(divisor*"6"), BigInt(divisor*"7"), BigInt(divisor*"8"), BigInt(divisor*"9"), BigInt(divisor*"10") };
	BigInt multiple("0");
	int i = 0;
	string quotient = "";

	bool keepDividing = true;

	while (nextDigit < numODigitsA && dividend < divisor) { dividend.integer = dividend.integer + A.integer[nextDigit++]; }
	while (keepDividing) {
		if (dividend >= divisor)	//is dividend greater than the divisor
		{	//which multiple gets us just below B?
			for (i = 0; i < 11; i++) {
				if (mulTable[i] <= dividend) { multiple.integer = (mulTable[i]).integer; }
				else { break; }
			}

			//Subtract multiple from dividend
			dividend = dividend - multiple;

			//save quotient
			quotient += to_string(i - 1);

			//grab next digit
			if (nextDigit < numODigitsA && dividend != "0") dividend.integer += A.integer[nextDigit++];
			else if (nextDigit < numODigitsA) dividend.integer = A.integer[nextDigit++];
		}
		else {	//divisor is greater, need to grab another digit
			if (nextDigit < numODigitsA && dividend != "0") { dividend.integer = dividend.integer + A.integer[nextDigit++]; quotient += "0"; }
			else if (nextDigit < numODigitsA) { dividend.integer = A.integer[nextDigit++]; quotient += "0"; }
			else keepDividing = false;
		}
	}
	return dividend;
}

const BigInt operator%(BigInt& A, string B)
{
	BigInt temp(B);
	return (A % temp);
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
Postcondition: Returns true if A and B are not equal*/
bool operator!=(const BigInt & A, string B)
{
	/*Look at the lengths first. If one is longer, they are not equal.*/
	if ((A.integer).length() != B.length())	return true;

	//start comparing digits
	for (int i = 0; i < (A.integer).length(); i++) {
		if ((A.integer)[i] == B[i])	continue;
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

/*Precondition: Compares A to B
Postcondition: Returns true if A == B*/
bool operator==(const BigInt & A, string B)
{
	/*Look at the lengths first. If one is longer, they are not equal.*/
	if ((A.integer).length() != (B).length())	return false;

	//start comparing digits
	for (int i = 0; i < (A.integer).length(); i++) {
		if ((A.integer)[i] == (B)[i])	continue;
		else								return false;
	}
	return true;
}

BigInt BigInt::pow(BigInt A, BigInt exponent, BigInt mod)
{
	BigInt a1(A);
	BigInt x("1");
	while (exponent != "0") {
		while ((exponent % "2") == "0") {
			a1 = (a1*a1);
			a1 = a1 % mod;
			exponent = exponent / "2";
		}
		x = (x*a1);
		x = x % mod;
		exponent = exponent - "1";
	}
	return x;
}

BigInt BigInt::pow(BigInt A, BigInt B, string C)
{
	return pow(A, B, BigInt (C));
}



