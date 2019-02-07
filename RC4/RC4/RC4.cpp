/*Brooke Weborg
EECS 5760
RC4 Algorithm
Encrypts and Decrpyts stuff done with RC4 

Valid inputs:
<key> <input> <output>
key can be taken as a string in the following format: 'MY STRING'
									or as a hex input: 4A2BE
input can be a valid file path or directly inputted to the console using the '-' character
output can be a valid file path or directly sent to the console using the '-' character*/

//Import Statements - Yeet
#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/*Precondition: Give it a 256 byte state array that needs permuted with an i and j to do the permutation
Postcondition: Returns an updated state array - yeet*/
unsigned char requestByte(unsigned char state[], int i, int j)
{
	unsigned char si = state[i];
	state[i] = state[j];
	state[j] = si;
	return state[(state[i] + state[j]) % 256];
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

/*Precondition: Takes a character representing a hex value
Postcondition: creates a true integer representation but as a character */
unsigned char charHexValue(unsigned char c)
{
	if (c >= 0x30 && c <= 0x39)						//0 - 9 subtract 30 hex
		c = c - 0x30;
	else if ((c >=65 && c <=70) || (c>=97 && c<=102)) //A-F subtract 37 hex
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

/*Takes input from command line to use in RC4 Encryption/Decrypt
and outputs result*/
int main(int argc, char * argv[])
{
	/*Command line inputs
	RC4	<key> <infile> <outfile>*/
	int length = argc;
	if (length < 4) //if length is less than 4 there are not enough inputs
	{
		cerr << "Error: Not enough arguements.\n <key> <infile> <outfile>";
		exit(0);
	}

	string key_in = argv[1];			//get and store key
	int lengthOfKey = key_in.length();	//key length if is interpretted as normal string
	int hexLength = lengthOfKey / 2;	//key length if is interpretted as hex
	unsigned char key[256] = "";

	//Are we interepretting this as a ASCII string?
	if (key_in[0] == '\'' && key_in[lengthOfKey - 1] == '\'')
	{
		key_in = key_in.substr(1, lengthOfKey - 2);
		lengthOfKey -= 2;

		//Make sure the length of key is less than 256
		if (lengthOfKey > 256)
		{
			cerr << "Error: Key length too long.";
			exit(0);
		}

		for (int i = 0; i < lengthOfKey; i++)
			key[i] = key[i];
	}
	else
	//Intrepret each character as a hex value
	{
		if(hexLength > 256)
		{
			cerr << "Error: Key length too long.";
			exit(1);
		}
		else if (lengthOfKey % 2 != 0)
		{
			cerr << "Error: Key length must be even.";
			exit(1);
		}

		unsigned char buffer = 0;				//put two nybbles in to make a byte before storing in key array
		int hexKeyCount = 0;					//location to store at in hex key arrau
		
		//Go through each char in inputted key. 
		//Interpret as hex value and store as binary representation
		//where to hex digits make one byte -> one character
		for(int i = 0; i < lengthOfKey; i++)
		{
			unsigned char c = key_in[i];
			c = charHexValue(c);

			if (buffer == 0)		//store upper half of byte
				buffer = buffer | (c << 4);
			else                   //store lower half of byte and then store as char in hex key array
			{
				buffer = buffer | c;
				key[hexKeyCount++] = buffer;
				buffer = 0;
			}
			lengthOfKey = hexLength;
		}
	}
	
	string infile = argv[2];		//contains input information
	string outfile = argv[3];		//contains output information

	//initialize RC4 state array
	unsigned char state[256];
	int stateLength = 256;
	for (int i = 0; i < stateLength; i++)
		state[i] = i;

	//Permute state array
	int j = 0;
	int i = 0;
	unsigned char si = ' ';
	for (i; i < stateLength; i++)
	{
		j = (j + state[i] + key[i % lengthOfKey]) % 256;
		si = state[i];
		state[i] = state[j];
		state[j] = si;
	}
	i = j = 0;

	if (infile[0] == '-')	//process as standard input
	{
		int inLength = infile.length();
		if (!((inLength - 1) % 2 == 0))
		{
			cerr << "Error: Input needs to be an even amount of hex digits.";
			exit(1);
		}
		unsigned char character = ' ';
		if (!(outfile[0] == '-')) 	//check if we're outputting to the console or not
		{
			try {
				//output stream setup
				ofstream outputText(outfile, ofstream::binary);
				outputText.exceptions(ofstream::failbit | ofstream::badbit);

				//go through and process input chars a char at a time to encrypt
				//input is assumed to be in hex and therefore is changed to 
				//it's character representation before encryption
				for (int k = 1; k < inLength; k++) {
					character = charHexValue(infile[k++]) << 4;
					character = character | (unsigned char)charHexValue(infile[k]);
					i = (i + 1) % 256;
					j = (j + state[i]) % 256;
					unsigned char rc4 = requestByte(state, i, j);
					rc4 = rc4 ^ character;			//XOR - actual encryption/decryption part
					outputText.put(rc4);			//print it to file
				}
			}
			catch (exception e) {
				cerr << e.what() << endl;
				exit(1);
			}
		}
		else {
			outfile = outfile.substr(1, outfile.length() - 1);	//get rid of the dash

			//go through and process input chars a char at a time to encrypt
			//input is assumed to be in hex and therefore is changed to 
			//it's character representation before encryption
			for (int k = 1; k < inLength; k++) {
				character = charHexValue(infile[k++]) << 4;
				character = character | (unsigned char)charHexValue(infile[k]);
				i = (i + 1) % 256;
				j = (j + state[i]) % 256;
				unsigned char rc4 = requestByte(state, i, j);
				rc4 = rc4 ^ character;			//XOR - actual encryption/decryption part
				cout << charToHex(rc4);			//print to screen
			}
		}
	}

	else//process as a file
	{
		try 
		{
			ifstream inputText(infile, ifstream::binary);
			if (!inputText.is_open())	//see if it's open
			{
				cerr << "Error: File cannot be read.";
				exit(1);
			}

			if (!(outfile[0] == '-')) 	//check if we're outputting to the console or not
			{
				//output stream setup
				ofstream outputText(outfile, ofstream::binary);
				outputText.exceptions(ofstream::failbit | ofstream::badbit);

				char temp = 0;						//holds input because can't put it immediately in unsigned char
				unsigned char character = temp;		//put the input in here so nothing explodes when we process 
				//Do until we reach the end of the file
				while (inputText.get(temp))
				{
					character = temp;
					i = (i + 1) % 256;				//alter i
					j = (j + state[i]) % 256;		//alter j
					unsigned char rc4 = requestByte(state, i, j);	//swap and return element to encrypt/decrypt with
					rc4 = rc4 ^ character;			//xor - actual encryption/decryption
					outputText.put(rc4);			//output to file
				}
				//close streams
				inputText.close();			
				outputText.close();
			}
			else //output to console
			{

				outfile = outfile.substr(1, outfile.length() - 1);
				char temp = 0;						//input function doesn't like unsigned temps so put it here first
				unsigned char character = temp;		//store temp here and actually use for processing
				//Do until we reach the end of the file
				while (inputText.get(temp))
				{
					character = temp;
					i = (i + 1) % 256;			//alter i
					j = (j + state[i]) % 256;	//alter j
					unsigned char rc4 = requestByte(state, i, j);		//swap and return element to encrypt/decrypt with
					rc4 = rc4 ^ character;			//xor - actual encryption/decryption
					cout << charToHex(rc4);			//output to console
				}
				inputText.close();				//close input stream
			}
		}
		catch (exception e) 
		{
			cerr << e.what() << endl;
			exit(1);
		}
	}
}
