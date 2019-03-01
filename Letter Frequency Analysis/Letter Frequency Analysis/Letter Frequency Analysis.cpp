/*Brooke Weborg
EECS 5980
Programming Assignment #1
2-1-2019

Brooke Weborg's Analyzer Assistant
aka BWAA-haha

Looks at a files 
>single bit frequencies
>64 bit block frequencies (octet)
>how often all 0 and all 1 bit 64 bit blocks occur
>single character frequencies
>di-graph frequencies
>tri-graph frequencies

And puts that information in files.

Everything is hard coded file names because I'm lazy and didn't feel like doing console I/O
*/

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//Important variables that for the most part don't need to be globals
//But I'm lazy and this is just for analyzing code so meh
unsigned int bit = 1;
unsigned int zeroBitFreq = 0;
unsigned int oneBitFreq = 0;
unsigned int blockCompositionFreq[65] = { 0 }; //0 index represents all zeros, 1 index represents 63 zeros and one 1, and 64 represents all 1s
unsigned int charFreq[256] = { 0 };
unsigned int digraphFreq[65536] = { 0 };
unsigned int octetFreq[640000] = { 0 };
unsigned long long octet[640000] = { 0 };
unsigned int trigraphFreq[16777216] = { 0 };

int main()
{
	string filename = //"C:\\Users\\Brooke Weborg\\Downloads\\Shakespeare DES\\Shakespeare.txt";
		//"C:\\Users\\Brooke Weborg\\Downloads\\Shakespeare DES\\Shakespeare.des";
		"C:\\Users\\Brooke Weborg\\Desktop\\cbc_encrypt.txt";
    //Open an ifstream
	ifstream inputText(filename, ifstream::binary);
	if (!inputText.is_open())
	{
		cerr << "Error: File cannot be read.";
		exit(1);
	}

	//get the first 3 chars first & process accordingly
	//this will definitely break if the file is less than 3 chars....
	char temp = 0;
	inputText.get(temp);
	unsigned char threeCharsAgo = temp;	//in a galaxy far, far away....
	charFreq[threeCharsAgo]++;			//store single byte in character frequency array
	//process char for single bits
	(  temp >> 7 == bit) ? (oneBitFreq++) : (zeroBitFreq++);
	(((temp >> 6) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
	(((temp >> 5) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
	(((temp >> 4) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
	(((temp >> 3) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
	(((temp >> 2) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
	(((temp >> 1) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
	(( temp		  & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);


	inputText.get(temp);
	unsigned char previous = temp;
	charFreq[previous]++;
	unsigned int comboChars = 0;
	//process char for single bits
	(temp >> 7 == bit) ? (oneBitFreq++) : (zeroBitFreq++);
	(((temp >> 6) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
	(((temp >> 5) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
	(((temp >> 4) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
	(((temp >> 3) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
	(((temp >> 2) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
	(((temp >> 1) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
	((temp & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);

	inputText.get(temp);
	unsigned char current = temp;

	do 
	{
		current = temp;
		//process single bits of character
		(  current >> 7 == bit) ? (oneBitFreq++) : (zeroBitFreq++);
		(((current >> 6) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
		(((current >> 5) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
		(((current >> 4) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
		(((current >> 3) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
		(((current >> 2) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
		(((current >> 1) & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);
		(( current & 0x01) == bit) ? (oneBitFreq++) : (zeroBitFreq++);

		//increment letter frequency by one
		charFreq[current]++;

		//increment digraph frequency by one (based on previous letter + current)
		comboChars = (previous << 8) | current;
		digraphFreq[comboChars]++;

		//increment trigraph frequency by two (based on previous TWO letters + current)
		comboChars = ((threeCharsAgo << 16) | (previous << 8)) | current;
		trigraphFreq[comboChars]++;

		//save two previous characters
		//two previous plus current equals 3 total characters
		threeCharsAgo = previous;
		previous = current;

	} while (inputText.get(temp));

	inputText.close();
	
	ifstream inputText1(filename, ifstream::binary);
	if (!inputText1.is_open())
	{
		cerr << "Error: File cannot be read.";
		exit(1);
	}
	
	//Process input text as 64 bit blocks to analyze octet and deduce if any blocks were all 0 or all 1.
	unsigned char buffer[8];
	unsigned long long block = 0;
	int blockCompCount = 0;
	while (inputText1.read(reinterpret_cast<char*>(&buffer), 8)) {
		//Process 64 bits of input into usable form
		block = (unsigned long long)(buffer[0]) << (64 - 8);
		block |= (unsigned long long)(buffer[1]) << (64 - 16);
		block |= (unsigned long long)(buffer[2]) << (64 - 24);
		block |= (unsigned long long)(buffer[3]) << (64 - 32);
		block |= (unsigned long long)(buffer[4]) << (64 - 40);
		block |= (unsigned long long)(buffer[5]) << (64 - 48);
		block |= (unsigned long long)(buffer[6]) << (64 - 56);
		block |= (unsigned long long)(buffer[7]);

		//store block composition information
		for(int i = 0; i < 64; i++)
			blockCompCount += (block >> i) & 0x01;
		blockCompositionFreq[blockCompCount]++;
		blockCompCount = 0;

		//store frequency info for octet block
		for (int i = 0; i < 640000; i++) {
			//if nothing is stored in that array block, the octet
			//hasn't been recorded yet so it must be written to the octet array
			if (octet[i] == 0 || octet[i] == block) {
				octet[i] = block;
				octetFreq[i]++;
				break;
			}
		}
	}
		inputText1.close();

		try
		{
			ofstream output1("C:\\Users\\Brooke Weborg\\Documents\\Shakespeare Analysis Files\\cbc_encrypted_output_single_bit.csv", ofstream::binary);
			output1.exceptions(ofstream::failbit | ofstream::badbit);
			output1 << "Bit,Frequency\n" << 0 << ',' << zeroBitFreq << endl << 1 << ',' << oneBitFreq << endl;
			output1.close();

			ofstream outputText("C:\\Users\\Brooke Weborg\\Documents\\Shakespeare Analysis Files\\cbc_encrypted_output_single.csv", ofstream::binary);
			outputText.exceptions(ofstream::failbit | ofstream::badbit);
			outputText << "Character,Frequency\n";

			for (int i = 0; i < 256; i++) {
				current = (unsigned char)i;
				outputText << to_string(i);
				outputText.put(',');
				outputText << to_string(charFreq[i]);
				outputText.put('\n');
			}
			outputText.close();

			ofstream output2("C:\\Users\\Brooke Weborg\\Documents\\Shakespeare Analysis Files\\cbc_encrypted_output_digraph.csv", ofstream::binary);
			output2.exceptions(ofstream::failbit | ofstream::badbit);
			output2 << "Digraph,Frequency\n";

			for (int i = 0; i < 256 * 256; i++) {
				comboChars = i;
				current = comboChars >> 8;
				if (digraphFreq[i] == 0)
					continue;
				output2 << to_string((unsigned int)current) + " ";

				current = comboChars & 0x00FF;
				output2 << to_string((unsigned int)current);

				output2.put(',');
				output2 << to_string(digraphFreq[i]);
				output2.put('\n');
			}
			output2.close();

			ofstream output3("C:\\Users\\Brooke Weborg\\Documents\\Shakespeare Analysis Files\\cbc_encrypted_output_trigraph.csv", ofstream::binary);
			output3.exceptions(ofstream::failbit | ofstream::badbit);
			output3 << "Trigraph,Frequency\n";
			int count = 0;
			unsigned int k = 0;
			for (k; k < 16777216; k++) {
				comboChars = k;
				current = comboChars >> 16;
				if (trigraphFreq[k] == 0)
					continue;
				count++;
				output3 << to_string((unsigned int)current) + " ";

				current = (comboChars >> 8) & 0x0000FF;
				output3 << to_string((unsigned int)current) + " ";

				current = comboChars & 0x00000FF;
				output3 << to_string((unsigned int)current);

				output3.put(',');
				output3 << to_string(trigraphFreq[k]);
				output3.put('\n');
				if (count >= 500000)
					break;
			}
			count = 0;
			for (int i = 40000; i > 0; i--) {
				output3.put('\n');
			}
			if (!(k >= 16777215))
				for (unsigned int i = 16777215; i > 0; i--) {
					comboChars = i;
					current = comboChars >> 16;
					if (trigraphFreq[i] == 0)
						continue;
					count++;
					output3 << to_string((unsigned int)current) + " ";

					current = (comboChars >> 8) & 0x0000FF;
					output3 << to_string((unsigned int)current) + " ";

					current = comboChars & 0x00000FF;
					output3 << to_string((unsigned int)current);

					output3.put(',');
					output3 << to_string(trigraphFreq[i]);
					output3.put('\n');
					if (count >= 500000)
						break;
				}
			output3.close();

			ofstream output4("C:\\Users\\Brooke Weborg\\Documents\\Shakespeare Analysis Files\\cbc_encrypted_output_bit_block.csv", ofstream::binary);
			output4.exceptions(ofstream::failbit | ofstream::badbit);
			output4 << "Block,Frequency\n";

			for (int i = 0; i < 65; i++) {
				output4 << 64-i << " zeros " << i << " ones, " << blockCompositionFreq[i] << endl;
			}
			output4.close();

			ofstream output5("C:\\Users\\Brooke Weborg\\Documents\\Shakespeare Analysis Files\\cbc_encrypted_output_octet.csv", ofstream::binary);
			output5.exceptions(ofstream::failbit | ofstream::badbit);
			output5 << "Octet,Frequency\n";

			for (int i = 0; i < 640000; i++) {
				output5 << octet[i] << ',' << octetFreq[i] << endl;
			}
			output5.close();
		}
		catch (exception e)
		{
			cerr << e.what() << endl;
			exit(1);
		}
	}
