#include "pch.h"
#include "BigInt.h"

BigInt::BigInt(string myInt)
{
	integer = myInt;
}

BigInt::BigInt()
{
	integer = "0";
}

BigInt::~BigInt()
{
}

/*Precondition: Given two BigInts 
Postcondition: Sum of the BigInts*/
const BigInt operator+(BigInt A, BigInt B)
{
	return BigInt(BigInt::add(A.integer, B.integer));
}

/*Precondition: Given A BigInts and a string representing a number
Postcondition: Sum of the BigInts*/
const BigInt operator+(BigInt A, string B)
{
	return BigInt(BigInt::add(A.integer, B));
}

/*Precondition: Given two strings representing numbers
Postcondition: Sum of the numbers as a string*/
string BigInt::add(string A, string B)
{
	unsigned char digitA = 0, digitB = 0, carry = 0, sum = 0;
	string finalAnswer = "";
	int locA = A.length() - 1, locB = B.length() - 1;
	for (locA, locB; (locA >= 0 && locB >= 0); locA--, locB--) {
		digitA = A[locA] & 0x0F;				//get digits to add from A and B
		digitB = B[locB] & 0x0F;

		sum = (digitA + digitB + carry);				//Add digits and generate carry value
		carry = 0;
		if (sum > 9) { sum -= 10; carry += 1; }
		finalAnswer = to_string(sum).append(finalAnswer);		//append current sum to answer
	}

	if (locA >= 0) {												//any digits leftover?
		while (carry != 0 && locA >= 0) {
			sum = ((A[locA] & 0x0F) + carry);
			carry = 0;
			if (sum > 9) { sum -= 10; carry += 1; }
			finalAnswer = to_string(sum).append(finalAnswer);
			locA--;
		}
		if (locA >= 0) finalAnswer = A.substr(0, locA + 1).append(finalAnswer);		//when there is no more carry, just append remaining digits
		else if (carry != 0)	 finalAnswer = to_string(carry).append(finalAnswer);
	}
	else if (locB >= 0) {											//any digits leftover?
		while (carry != 0 && locB >= 0) {
			sum = ((B[locB] & 0x0F) + carry);
			carry = 0;
			if (sum > 9) { sum -= 10; carry += 1; }
			finalAnswer = to_string(sum).append(finalAnswer);
			locB--;
		}
		if (locB >= 0) finalAnswer = B.substr(0, locB + 1).append(finalAnswer);	//when there is no more carry, just append remaining digits
		else if (carry != 0)	 finalAnswer = to_string(carry).append(finalAnswer);
	}
	else if (carry != 0) { finalAnswer = to_string(carry).append(finalAnswer); }
	return (finalAnswer);
}

/*Precondition: Given two BigInts
Postcondition: Difference of the BigInts*/
const BigInt operator-(BigInt A, BigInt B)
{
	//return error if A is less than B
	if (A < B) { cerr << "Error. A is less than B. Cannot compute."; exit(1); }
	else if (A == B) { return BigInt("0"); }										//We already know the answer so return it.
	return BigInt(BigInt::sub(A.integer, B.integer));
}

/*Precondition: Given two BigInts
Postcondition: Difference of the BigInts*/
const BigInt operator-(BigInt A, string B)
{
	//return error if A is less than B
	if (A < B) { cerr << "Error. A is less than B. Cannot compute."; exit(1); }
	else if (A == B) { return BigInt("0"); }										//We already know the answer so return it.
	return BigInt(BigInt::sub(A.integer, B));
}

/*Precondition: Given a BigInt and a string
Postcondition: Difference of the numbers as a string*/
string BigInt::sub(string A, string B)
{
	unsigned char digitA = 0, digitB = 0, carry = 0, sum = 0, tempA = 0, tempB = 0;
	string finalAnswer = "";
	int locA = A.length() - 1, locB = B.length() - 1, locATemp = 0, locBTemp = 0;

	for (locA, locB; (locA >= 0 && locB >= 0); locA--, locB--) {
		digitA = A[locA] & 0x0F;				//get digits to add from A and B
		digitB = B[locB] & 0x0F;

		if ((digitA >= digitB) && carry == 0) { sum = digitA - digitB; }			//Is digit A larger than digit B? If so subtract
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
				if (locATemp >= 0) { tempA = A[locATemp--] & 0x0F; }
				if (locBTemp >= 0) { tempB = B[locBTemp--] & 0x0F; }
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
		digitA = A[locA] & 0x0F;
		digitA = (digitA + 10) - 1;
		finalAnswer = to_string(digitA) + finalAnswer;
		locA--;
	}
	if (carry == 1) {
		carry--;
		digitA = A[locA] & 0x0F;
		digitA--;
		finalAnswer = to_string(digitA) + finalAnswer;
		locA--;
	}
	if (locA >= 0)	finalAnswer = A.substr(0, locA + 1) + finalAnswer;		//when there is no more carry, just append remaining digits

	if(NE(finalAnswer, "0"))	while (finalAnswer[0] == '0') { finalAnswer = finalAnswer.substr(1); }
	return (finalAnswer);
}

/*Precondition: takes a string representing a number (must fit into an unsigned int)
Postcondition: returns unsigned int of that number*/
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

/*Precondition: Given two BigInts
Postcondition: Give the product as a bigint*/
const BigInt operator*(BigInt A, BigInt B)
{	
	return BigInt(BigInt::mul(A.integer, B.integer));
}

/*Precondition: Given a BigInt and a string
Postcondition: Give the product as a bigint*/
const BigInt operator*(BigInt A, string B)
{
	return BigInt(BigInt::mul(A.integer, B));
}

/*Precondition: Given two strings representing numbers
Postcondition: Give the product as a string*/
string BigInt::mul(string A, string B)
{
	if (A.length() == 1 && B.length() == 1) {
		int aNum = BigInt::toDecimal(A);
		int bNum = BigInt::toDecimal(B);
		int ans = aNum * bNum;
		return to_string(ans);
	}
	else if (EQ(A, "0") || EQ(B, "0")) { return "0"; }

	//split the digit sequences in the middle
	string high1 = "0";
	string low1 = "0";
	string high2 = "0";
	string low2 = "0";
	//calculates the size of the numbers
	unsigned int m = 0;
	if (A.length() > B.length()) { m = B.length(); }
	else { m = A.length(); }
	if (m % 2 != 0) { m = m / 2 + 1; }
	else { m = m / 2; }
	high1 = A.substr(0, A.length() - m);
	low1 = A.substr(A.length() - m);
	high2 = B.substr(0, B.length() - m);
	low2 = B.substr(B.length() - m);
	if (high2 == "") high2 = "0";
	if (high1 == "") high1 = "0";

	//do calculation things
	string z2 = mul(high1, high2);
	string z0 = mul(low1, low2);
	string z1 = add(mul(high1, low2), mul(low1,high2));
	if (z2 != "0") for (int i = 0; i < 2 * m; i++)	z2 = z2 + "0";	//finish z2 calc
	if (z1 != "0") for (int i = 0; i < m; i++)	z1 = z1 + "0";	//finish z1 calc

	return (add(add(z2, z1), z0));
}

/*Precondition: Given two BigInts
Postcondition: Give the quotient as a BigInt*/
const BigInt operator/(BigInt A, BigInt divisor)
{
	if (A < divisor) { return BigInt("0"); }
	if (divisor == "0") { cerr << "Can't divide by zero."; return BigInt("-1"); }
	return BigInt(BigInt::div(A.integer, divisor.integer));
}

/*Precondition: Given a BigInt and a string represening a number
Postcondition: Give the quotient as a BigInt*/
const BigInt operator/(BigInt A, string divisor)
{
	if (A < divisor) { return BigInt("0"); }
	if (divisor == "0") { cerr << "Can't divide by zero."; return BigInt("-1"); }
	return BigInt(BigInt::div(A.integer, divisor));
}

/*Precondition: Given two strings representing numbers
Postcondition: Give the quotient as a string*/
string BigInt::div(string A, string divisor)
{
	unsigned int numODigitsA = A.length(), nextDigit = 1;
	string dividend = ((A).substr(0, nextDigit));

	//make multiplication table
	string mulTable[11] = {"0", mul(divisor,"1"), mul(divisor,"2"), mul(divisor,"3"), mul(divisor,"4"), mul(divisor,"5"), mul(divisor,"6"), mul(divisor,"7"), mul(divisor,"8"), mul(divisor,"9"), mul(divisor,"10") };
	string multiple = "0";
	int i = 0;
	string quotient = "";

	bool keepDividing = true;

	while (nextDigit < numODigitsA && LT(dividend, divisor)) { dividend = dividend + A[nextDigit++]; }
	while (keepDividing) {
		if (GE(dividend, divisor))	//is dividend greater than the divisor
		{	//which multiple gets us just below B?
			for (i = 0; i < 11; i++) {
				if (LE(mulTable[i], dividend)) { multiple = (mulTable[i]); }
				else { break; }
			}

			//Subtract multiple from dividend
			dividend = BigInt::sub(dividend, multiple);

			//save quotient
			quotient += to_string(i - 1);

			//grab next digit
			if (nextDigit < numODigitsA && NE(dividend, "0")) dividend += A[nextDigit++];
			else if (nextDigit < numODigitsA) dividend = A[nextDigit++];
		}
		else {	//divisor is greater, need to grab another digit
			if (nextDigit < numODigitsA && NE(dividend, "0")) { dividend = dividend + A[nextDigit++]; quotient += "0"; }
			else if (nextDigit < numODigitsA) { dividend = A[nextDigit++]; quotient += "0"; }
			else keepDividing = false;
		}
	}
	return (quotient);
}

/*Precondition: Given two BigInts
Postcondition: Give the remainder as a BigInt*/
const BigInt operator%(BigInt A, BigInt divisor)
{
	if (A < divisor) { return BigInt("0"); }
	if (divisor == "0") { cerr << "Can't divide by zero."; return BigInt("-1"); }
	return BigInt(BigInt::mod(A.integer, divisor.integer));
}

/*Precondition: Given a BigInt and a string represening a number
Postcondition: Give the remainder as a BigInt*/
const BigInt operator%(BigInt A, string B)
{
	if (A < B) { return BigInt("0"); }
	if (B == "0") { cerr << "Can't divide by zero."; return BigInt("-1"); }
	return BigInt(BigInt::mod(A.integer, B));
}

/*Precondition: Given two strings representing numbers
Postcondition: Give the remainder as a string*/
string BigInt::mod(string A, string divisor)
{
	unsigned int numODigitsA = A.length(), nextDigit = 1;
	string dividend = ((A).substr(0, nextDigit));

	//make multiplication table
	string mulTable[11] = { "0", mul(divisor,"1"), mul(divisor,"2"), mul(divisor,"3"), mul(divisor,"4"), mul(divisor,"5"), mul(divisor,"6"), mul(divisor,"7"), mul(divisor,"8"), mul(divisor,"9"), mul(divisor,"10") };
	string multiple = "0";
	int i = 0;
	string quotient = "";

	bool keepDividing = true;

	while (nextDigit < numODigitsA && LT(dividend, divisor)) { dividend = dividend + A[nextDigit++]; }
	while (keepDividing) {
		if (GE(dividend, divisor))	//is dividend greater than the divisor
		{	//which multiple gets us just below B?
			for (i = 0; i < 11; i++) {
				if (LE(mulTable[i], dividend)) { multiple = (mulTable[i]); }
				else { break; }
			}

			//Subtract multiple from dividend
			dividend = BigInt::sub(dividend, multiple);

			//save quotient
			quotient += to_string(i - 1);

			//grab next digit
			if (nextDigit < numODigitsA && NE(dividend, "0")) dividend += A[nextDigit++];
			else if (nextDigit < numODigitsA) dividend = A[nextDigit++];
		}
		else {	//divisor is greater, need to grab another digit
			if (nextDigit < numODigitsA && NE(dividend, "0")) { dividend = dividend + A[nextDigit++]; quotient += "0"; }
			else if (nextDigit < numODigitsA) { dividend = A[nextDigit++]; quotient += "0"; }
			else keepDividing = false;
		}
	}
	return (dividend);
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is less than B*/
bool operator <(BigInt A, BigInt B)
{
	return BigInt::LT(A.integer, B.integer);
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is less than B*/
bool operator<(BigInt A, string B)
{
	return BigInt::LT(A.integer, B);
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is less than B*/
bool BigInt::LT(string A, string B)
{
	/*Look at the lengths first. If A is longer it is larger than B.*/
	if ((A).length() > (B).length())		return false;
	else if ((A).length() < (B).length()) return true;

	//start comparing digits
	for (int i = 0; i < (A).length(); i++) {
		if ((A)[i] > (B)[i])			return false;		//if a digit in A is greater than the digit in B, A is larger. 
		else if ((A)[i] == (B)[i])	continue;			//if the digit in A is equal to the digit in B continue
		else											return true;		//the digit in A must be less than the one in B. A is smaller.
	}
	return false;
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is less than or equal to B*/
bool operator <=(BigInt A,BigInt B)
{
	return BigInt::LE(A.integer, B.integer);
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is less than or equal to B*/
bool operator <=(BigInt A, string B)
{
	return BigInt::LE(A.integer, B);
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is less than or equal to B*/
bool BigInt::LE(string A, string B)
{
	/*Look at the lengths first. If A is longer it is larger than B.*/
	if ((A).length() > (B).length())	return false;
	else if ((A).length() < (B).length()) return true;

	//start comparing digits
	for (int i = 0; i < (A).length(); i++) {
		if ((A)[i] > (B)[i])			return false;		//if a digit in A is greater than the digit in B, A is larger. 
		else if ((A)[i] == (B)[i])	continue;			//if the digit in A is equal to the digit in B continue
		else											return true;		//the digit in A must be less than the one in B. A is smaller.
	}
	return true;
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is greater than B*/
bool operator >( BigInt A, BigInt B)
{
	return BigInt::GE(A.integer, B.integer);
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is greater than B*/
bool operator>(BigInt A, string B)
{
	return BigInt::GE(A.integer, B);
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is greater than B*/
bool BigInt::GT(string A, string B)
{
	/*Look at the lengths first. If A is longer it is larger than B.*/
	if ((A).length() > (B).length())	return true;
	else if ((A).length() < (B).length()) return false;

	//start comparing digits
	for (int i = 0; i < (A).length(); i++) {
		if ((A)[i] > (B)[i])			return true;		//if a digit in A is greater than the digit in B, A is larger. 
		else if ((A)[i] == (B)[i])	continue;			//if the digit in A is equal to the digit in B continue
		else											return false;		//the digit in A must be less than the one in B. A is smaller.
	}
	return false;
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is greater than or equal to B*/
bool operator >=(BigInt A, const BigInt B)
{
	return BigInt::GE(A.integer, B.integer);
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is greater than or equal to B*/
bool operator>=(BigInt A, string B)
{
	return BigInt::GE(A.integer, B);
}

/*Precondition: Compares A to B
Postcondition: Returns true if A is greater than or equal to B*/
bool BigInt::GE(string A, string B)
{
	/*Look at the lengths first. If A is longer it is larger than B.*/
	if ((A).length() > (B).length())	return true;
	else if ((A).length() < (B).length()) return false;

	//start comparing digits
	for (int i = 0; i < (A).length(); i++) {
		if ((A)[i] > (B)[i])			return true;		//if a digit in A is greater than the digit in B, A is larger. 
		else if ((A)[i] == (B)[i])	continue;			//if the digit in A is equal to the digit in B continue
		else											return false;		//the digit in A must be less than the one in B. A is smaller.
	}
	return true;
}

/*Precondition: Compares A to B
Postcondition: Returns true if A and B are not equal*/
bool operator !=(BigInt A, BigInt B)
{
	return BigInt::NE(A.integer, B.integer);
}

/*Precondition: Compares A to B
Postcondition: Returns true if A and B are not equal*/
bool operator!=( BigInt A, string B)
{
	return BigInt::NE(A.integer, B);
}

bool BigInt::NE(string A, string B)
{
	/*Look at the lengths first. If one is longer, they are not equal.*/
	if ((A).length() != (B).length())	return true;

	//start comparing digits
	for (int i = 0; i < (A).length(); i++) {
		if ((A)[i] == (B)[i])	continue;
		else								return true;
	}
	return false;
}

/*Precondition: Compares A to B
Postcondition: Returns true if A == B*/
bool operator ==( BigInt A, BigInt B)
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
bool operator==(BigInt A, string B)
{
	BigInt temp(B);
	return A == temp;
}

/*Precondition: Compares A to B
Postcondition: Returns true if A == B*/
bool BigInt::EQ(string A, string B)
{
	/*Look at the lengths first. If one is longer, they are not equal.*/
	if ((A).length() != (B).length())	return false;

	//start comparing digits
	for (int i = 0; i < (A).length(); i++) {
		if ((A)[i] == (B)[i])	continue;
		else								return false;
	}
	return true;
}

/*Precondition: A, B, and C are BigInts
Postcondition: Returns A raised to B mod C*/
BigInt BigInt::pow(BigInt A, BigInt B, BigInt C)
{
	return BigInt(pow(A.integer, B.integer, C.integer));
}

/*Precondition: A, B, are BigInts and C is a string representing number
Postcondition: Returns A raised to B mod C*/
BigInt BigInt::pow(BigInt A, BigInt B, string C)
{
	return BigInt(pow(A.integer, B.integer, C));
}

/*Precondition: A is a BigInts, B and C are strings representing numbers
Postcondition: Returns A raised to B mod C*/
string BigInt::pow(string A, string exponent, string C)
{
	string a1 = A;
	string x = "1";
	while (NE(exponent, "0")) {
		while (EQ(mod(exponent, "2"), "0")) {
			a1 = mul(a1,a1);
			a1 = mod(a1, C);
			exponent = div(exponent, "2");
		}
		x = mul(x,a1);
		x = mod(x, C);
		exponent = sub(exponent, "1");
	}
	return x;
}

string BigInt::toString()
{
	return integer;
}