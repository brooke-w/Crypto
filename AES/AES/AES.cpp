/*Brooke Weborg
EECS 5980
Programming Assignment #3
3-11-2019

This program will take console input in the form
AES <–action> <key> <mode> <infile> <outfile>

And perform the AES encryption/decryption on a given file.
*/

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

using namespace std;

//Globals
unsigned int roundKeys[56];	//40 for 10 rounds, 48 for 12 rounds, 56 for 14 rounds
unsigned char state[4][4] = { 0 };	//holds the current state of the message to be encrypted/decrypted
//all the substitutions for subBytes function
unsigned char subBytesSubs[16][16] = { {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76},
										{0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0},
										{0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15},
										{0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75},
										{0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84},
										{0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf},
										{0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8},
										{0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2},
										{0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73},
										{0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb},
										{0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79},
										{0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08},
										{0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a},
										{0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e},
										{0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf},
										{0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16} };	
// all the substitutions for invSubBytes function
unsigned char invSubBytesSubs[16][16] = { {0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb},
									{0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb},
									{0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e},
									{0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25},
									{0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92},
									{0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84},
									{0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06},
									{0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b},
									{0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73},
									{0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e},
									{0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b},
									{0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4},
									{0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f},
									{0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef},
									{0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61},
									{0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d} };
//mult table for gf x2
const unsigned char gfMult2[256] = { 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2A, 0x2C, 0x2E, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E,
								0x40, 0x42, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5A, 0x5C, 0x5E, 0x60, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C, 0x6E, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7A, 0x7C, 0x7E,
								0x80, 0x82, 0x84, 0x86, 0x88, 0x8A, 0x8C, 0x8E, 0x90, 0x92, 0x94, 0x96, 0x98, 0x9A, 0x9C, 0x9E, 0xA0, 0xA2, 0xA4, 0xA6, 0xA8, 0xAA, 0xAC, 0xAE, 0xB0, 0xB2, 0xB4, 0xB6, 0xB8, 0xBA, 0xBC, 0xBE,
								0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE, 0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE, 0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE, 0xF0, 0xF2, 0xF4, 0xF6, 0xF8, 0xFA, 0xFC, 0xFE,
								0x1B, 0x19, 0x1F, 0x1D, 0x13, 0x11, 0x17, 0x15, 0x0B, 0x09, 0x0F, 0x0D, 0x03, 0x01, 0x07, 0x05, 0x3B, 0x39, 0x3F, 0x3D, 0x33, 0x31, 0x37, 0x35, 0x2B, 0x29, 0x2F, 0x2D, 0x23, 0x21, 0x27, 0x25,
								0x5B, 0x59, 0x5F, 0x5D, 0x53, 0x51, 0x57, 0x55, 0x4B, 0x49, 0x4F, 0x4D, 0x43, 0x41, 0x47, 0x45, 0x7B, 0x79, 0x7F, 0x7D, 0x73, 0x71, 0x77, 0x75, 0x6B, 0x69, 0x6F, 0x6D, 0x63, 0x61, 0x67, 0x65,
								0x9B, 0x99, 0x9F, 0x9D, 0x93, 0x91, 0x97, 0x95, 0x8B, 0x89, 0x8F, 0x8D, 0x83, 0x81, 0x87, 0x85, 0xBB, 0xB9, 0xBF, 0xBD, 0xB3, 0xB1, 0xB7, 0xB5, 0xAB, 0xA9, 0xAF, 0xAD, 0xA3, 0xA1, 0xA7, 0xA5,
								0xDB, 0xD9, 0xDF, 0xDD, 0xD3, 0xD1, 0xD7, 0xD5, 0xCB, 0xC9, 0xCF, 0xCD, 0xC3, 0xC1, 0xC7, 0xC5, 0xFB, 0xF9, 0xFF, 0xFD, 0xF3, 0xF1, 0xF7, 0xF5, 0xEB, 0xE9, 0xEF, 0xED, 0xE3, 0xE1, 0xE7, 0xE5 };
//mult table for gf x3
const unsigned char gfMult3[256] = { 0x00, 0x03, 0x06, 0x05, 0x0C, 0x0F, 0x0A, 0x09, 0x18, 0x1B, 0x1E, 0x1D, 0x14, 0x17, 0x12, 0x11, 0x30, 0x33, 0x36, 0x35, 0x3C, 0x3F, 0x3A, 0x39, 0x28, 0x2B, 0x2E, 0x2D, 0x24, 0x27, 0x22, 0x21,
								0x60, 0x63, 0x66, 0x65, 0x6C, 0x6F, 0x6A, 0x69, 0x78, 0x7B, 0x7E, 0x7D, 0x74, 0x77, 0x72, 0x71, 0x50, 0x53, 0x56, 0x55, 0x5C, 0x5F, 0x5A, 0x59, 0x48, 0x4B, 0x4E, 0x4D, 0x44, 0x47, 0x42, 0x41,
								0xC0, 0xC3, 0xC6, 0xC5, 0xCC, 0xCF, 0xCA, 0xC9, 0xD8, 0xDB, 0xDE, 0xDD, 0xD4, 0xD7, 0xD2, 0xD1, 0xF0, 0xF3, 0xF6, 0xF5, 0xFC, 0xFF, 0xFA, 0xF9, 0xE8, 0xEB, 0xEE, 0xED, 0xE4, 0xE7, 0xE2, 0xE1,
								0xA0, 0xA3, 0xA6, 0xA5, 0xAC, 0xAF, 0xAA, 0xA9, 0xB8, 0xBB, 0xBE, 0xBD, 0xB4, 0xB7, 0xB2, 0xB1, 0x90, 0x93, 0x96, 0x95, 0x9C, 0x9F, 0x9A, 0x99, 0x88, 0x8B, 0x8E, 0x8D, 0x84, 0x87, 0x82, 0x81,
								0x9B, 0x98, 0x9D, 0x9E, 0x97, 0x94, 0x91, 0x92, 0x83, 0x80, 0x85, 0x86, 0x8F, 0x8C, 0x89, 0x8A, 0xAB, 0xA8, 0xAD, 0xAE, 0xA7, 0xA4, 0xA1, 0xA2, 0xB3, 0xB0, 0xB5, 0xB6, 0xBF, 0xBC, 0xB9, 0xBA,
								0xFB, 0xF8, 0xFD, 0xFE, 0xF7, 0xF4, 0xF1, 0xF2, 0xE3, 0xE0, 0xE5, 0xE6, 0xEF, 0xEC, 0xE9, 0xEA, 0xCB, 0xC8, 0xCD, 0xCE, 0xC7, 0xC4, 0xC1, 0xC2, 0xD3, 0xD0, 0xD5, 0xD6, 0xDF, 0xDC, 0xD9, 0xDA,
								0x5B, 0x58, 0x5D, 0x5E, 0x57, 0x54, 0x51, 0x52, 0x43, 0x40, 0x45, 0x46, 0x4F, 0x4C, 0x49, 0x4A, 0x6B, 0x68, 0x6D, 0x6E, 0x67, 0x64, 0x61, 0x62, 0x73, 0x70, 0x75, 0x76, 0x7F, 0x7C, 0x79, 0x7A,
								0x3B, 0x38, 0x3D, 0x3E, 0x37, 0x34, 0x31, 0x32, 0x23, 0x20, 0x25, 0x26, 0x2F, 0x2C, 0x29, 0x2A, 0x0B, 0x08, 0x0D, 0x0E, 0x07, 0x04, 0x01, 0x02, 0x13, 0x10, 0x15, 0x16, 0x1F, 0x1C, 0x19, 0x1A };
//mult table for gf xE
const unsigned char gfMultE[256] = { 0x00, 0x0E, 0x1C, 0x12, 0x38, 0x36, 0x24, 0x2A, 0x70, 0x7E, 0x6C, 0x62, 0x48, 0x46, 0x54, 0x5A, 0xE0, 0xEE, 0xFC, 0xF2, 0xD8, 0xD6, 0xC4, 0xCA, 0x90, 0x9E, 0x8C, 0x82, 0xA8, 0xA6, 0xB4, 0xBA,
								0xDB, 0xD5, 0xC7, 0xC9, 0xE3, 0xED, 0xFF, 0xF1, 0xAB, 0xA5, 0xB7, 0xB9, 0x93, 0x9D, 0x8F, 0x81, 0x3B, 0x35, 0x27, 0x29, 0x03, 0x0D, 0x1F, 0x11, 0x4B, 0x45, 0x57, 0x59, 0x73, 0x7D, 0x6F, 0x61,
								0xAD, 0xA3, 0xB1, 0xBF, 0x95, 0x9B, 0x89, 0x87, 0xDD, 0xD3, 0xC1, 0xCF, 0xE5, 0xEB, 0xF9, 0xF7, 0x4D, 0x43, 0x51, 0x5F, 0x75, 0x7B, 0x69, 0x67, 0x3D, 0x33, 0x21, 0x2F, 0x05, 0x0B, 0x19, 0x17,
								0x76, 0x78, 0x6A, 0x64, 0x4E, 0x40, 0x52, 0x5C, 0x06, 0x08, 0x1A, 0x14, 0x3E, 0x30, 0x22, 0x2C, 0x96, 0x98, 0x8A, 0x84, 0xAE, 0xA0, 0xB2, 0xBC, 0xE6, 0xE8, 0xFA, 0xF4, 0xDE, 0xD0, 0xC2, 0xCC,
								0x41, 0x4F, 0x5D, 0x53, 0x79, 0x77, 0x65, 0x6B, 0x31, 0x3F, 0x2D, 0x23, 0x09, 0x07, 0x15, 0x1B, 0xA1, 0xAF, 0xBD, 0xB3, 0x99, 0x97, 0x85, 0x8B, 0xD1, 0xDF, 0xCD, 0xC3, 0xE9, 0xE7, 0xF5, 0xFB,
								0x9A, 0x94, 0x86, 0x88, 0xA2, 0xAC, 0xBE, 0xB0, 0xEA, 0xE4, 0xF6, 0xF8, 0xD2, 0xDC, 0xCE, 0xC0, 0x7A, 0x74, 0x66, 0x68, 0x42, 0x4C, 0x5E, 0x50, 0x0A, 0x04, 0x16, 0x18, 0x32, 0x3C, 0x2E, 0x20,
								0xEC, 0xE2, 0xF0, 0xFE, 0xD4, 0xDA, 0xC8, 0xC6, 0x9C, 0x92, 0x80, 0x8E, 0xA4, 0xAA, 0xB8, 0xB6, 0x0C, 0x02, 0x10, 0x1E, 0x34, 0x3A, 0x28, 0x26, 0x7C, 0x72, 0x60, 0x6E, 0x44, 0x4A, 0x58, 0x56,
								0x37, 0x39, 0x2B, 0x25, 0x0F, 0x01, 0x13, 0x1D, 0x47, 0x49, 0x5B, 0x55, 0x7F, 0x71, 0x63, 0x6D, 0xD7, 0xD9, 0xCB, 0xC5, 0xEF, 0xE1, 0xF3, 0xFD, 0xA7, 0xA9, 0xBB, 0xB5, 0x9F, 0x91, 0x83, 0x8D };
//mult table for gf xB
const unsigned char gfMultB[256] = { 0x00, 0x0B, 0x16, 0x1D, 0x2C, 0x27, 0x3A, 0x31, 0x58, 0x53, 0x4E, 0x45, 0x74, 0x7F, 0x62, 0x69, 0xB0, 0xBB, 0xA6, 0xAD, 0x9C, 0x97, 0x8A, 0x81, 0xE8, 0xE3, 0xFE, 0xF5, 0xC4, 0xCF, 0xD2, 0xD9,
							0x7B, 0x70, 0x6D, 0x66, 0x57, 0x5C, 0x41, 0x4A, 0x23, 0x28, 0x35, 0x3E, 0x0F, 0x04, 0x19, 0x12, 0xCB, 0xC0, 0xDD, 0xD6, 0xE7, 0xEC, 0xF1, 0xFA, 0x93, 0x98, 0x85, 0x8E, 0xBF, 0xB4, 0xA9, 0xA2,
							0xF6, 0xFD, 0xE0, 0xEB, 0xDA, 0xD1, 0xCC, 0xC7, 0xAE, 0xA5, 0xB8, 0xB3, 0x82, 0x89, 0x94, 0x9F, 0x46, 0x4D, 0x50, 0x5B, 0x6A, 0x61, 0x7C, 0x77, 0x1E, 0x15, 0x08, 0x03, 0x32, 0x39, 0x24, 0x2F,
							0x8D, 0x86, 0x9B, 0x90, 0xA1, 0xAA, 0xB7, 0xBC, 0xD5, 0xDE, 0xC3, 0xC8, 0xF9, 0xF2, 0xEF, 0xE4, 0x3D, 0x36, 0x2B, 0x20, 0x11, 0x1A, 0x07, 0x0C, 0x65, 0x6E, 0x73, 0x78, 0x49, 0x42, 0x5F, 0x54,
							0xF7, 0xFC, 0xE1, 0xEA, 0xDB, 0xD0, 0xCD, 0xC6, 0xAF, 0xA4, 0xB9, 0xB2, 0x83, 0x88, 0x95, 0x9E, 0x47, 0x4C, 0x51, 0x5A, 0x6B, 0x60, 0x7D, 0x76, 0x1F, 0x14, 0x09, 0x02, 0x33, 0x38, 0x25, 0x2E,
							0x8C, 0x87, 0x9A, 0x91, 0xA0, 0xAB, 0xB6, 0xBD, 0xD4, 0xDF, 0xC2, 0xC9, 0xF8, 0xF3, 0xEE, 0xE5, 0x3C, 0x37, 0x2A, 0x21, 0x10, 0x1B, 0x06, 0x0D, 0x64, 0x6F, 0x72, 0x79, 0x48, 0x43, 0x5E, 0x55,
							0x01, 0x0A, 0x17, 0x1C, 0x2D, 0x26, 0x3B, 0x30, 0x59, 0x52, 0x4F, 0x44, 0x75, 0x7E, 0x63, 0x68, 0xB1, 0xBA, 0xA7, 0xAC, 0x9D, 0x96, 0x8B, 0x80, 0xE9, 0xE2, 0xFF, 0xF4, 0xC5, 0xCE, 0xD3, 0xD8,
							0x7A, 0x71, 0x6C, 0x67, 0x56, 0x5D, 0x40, 0x4B, 0x22, 0x29, 0x34, 0x3F, 0x0E, 0x05, 0x18, 0x13, 0xCA, 0xC1, 0xDC, 0xD7, 0xE6, 0xED, 0xF0, 0xFB, 0x92, 0x99, 0x84, 0x8F, 0xBE, 0xB5, 0xA8, 0xA3 };
//mult table for gf xD
const unsigned char gfMultD[256] = { 0x00, 0x0D, 0x1A, 0x17, 0x34, 0x39, 0x2E, 0x23, 0x68, 0x65, 0x72, 0x7F, 0x5C, 0x51, 0x46, 0x4B, 0xD0, 0xDD, 0xCA, 0xC7, 0xE4, 0xE9, 0xFE, 0xF3, 0xB8, 0xB5, 0xA2, 0xAF, 0x8C, 0x81, 0x96, 0x9B,
								0xBB, 0xB6, 0xA1, 0xAC, 0x8F, 0x82, 0x95, 0x98, 0xD3, 0xDE, 0xC9, 0xC4, 0xE7, 0xEA, 0xFD, 0xF0, 0x6B, 0x66, 0x71, 0x7C, 0x5F, 0x52, 0x45, 0x48, 0x03, 0x0E, 0x19, 0x14, 0x37, 0x3A, 0x2D, 0x20,
								0x6D, 0x60, 0x77, 0x7A, 0x59, 0x54, 0x43, 0x4E, 0x05, 0x08, 0x1F, 0x12, 0x31, 0x3C, 0x2B, 0x26, 0xBD, 0xB0, 0xA7, 0xAA, 0x89, 0x84, 0x93, 0x9E, 0xD5, 0xD8, 0xCF, 0xC2, 0xE1, 0xEC, 0xFB, 0xF6,
								0xD6, 0xDB, 0xCC, 0xC1, 0xE2, 0xEF, 0xF8, 0xF5, 0xBE, 0xB3, 0xA4, 0xA9, 0x8A, 0x87, 0x90, 0x9D, 0x06, 0x0B, 0x1C, 0x11, 0x32, 0x3F, 0x28, 0x25, 0x6E, 0x63, 0x74, 0x79, 0x5A, 0x57, 0x40, 0x4D,
								0xDA, 0xD7, 0xC0, 0xCD, 0xEE, 0xE3, 0xF4, 0xF9, 0xB2, 0xBF, 0xA8, 0xA5, 0x86, 0x8B, 0x9C, 0x91, 0x0A, 0x07, 0x10, 0x1D, 0x3E, 0x33, 0x24, 0x29, 0x62, 0x6F, 0x78, 0x75, 0x56, 0x5B, 0x4C, 0x41,
								0x61, 0x6C, 0x7B, 0x76, 0x55, 0x58, 0x4F, 0x42, 0x09, 0x04, 0x13, 0x1E, 0x3D, 0x30, 0x27, 0x2A, 0xB1, 0xBC, 0xAB, 0xA6, 0x85, 0x88, 0x9F, 0x92, 0xD9, 0xD4, 0xC3, 0xCE, 0xED, 0xE0, 0xF7, 0xFA,
								0xB7, 0xBA, 0xAD, 0xA0, 0x83, 0x8E, 0x99, 0x94, 0xDF, 0xD2, 0xC5, 0xC8, 0xEB, 0xE6, 0xF1, 0xFC, 0x67, 0x6A, 0x7D, 0x70, 0x53, 0x5E, 0x49, 0x44, 0x0F, 0x02, 0x15, 0x18, 0x3B, 0x36, 0x21, 0x2C,
								0x0C, 0x01, 0x16, 0x1B, 0x38, 0x35, 0x22, 0x2F, 0x64, 0x69, 0x7E, 0x73, 0x50, 0x5D, 0x4A, 0x47, 0xDC, 0xD1, 0xC6, 0xCB, 0xE8, 0xE5, 0xF2, 0xFF, 0xB4, 0xB9, 0xAE, 0xA3, 0x80, 0x8D, 0x9A, 0x97 };
//mult table for gf x9
const unsigned char gfMult9[256] = { 0x00, 0x09, 0x12, 0x1B, 0x24, 0x2D, 0x36, 0x3F, 0x48, 0x41, 0x5A, 0x53, 0x6C, 0x65, 0x7E, 0x77, 0x90, 0x99, 0x82, 0x8B, 0xB4, 0xBD, 0xA6, 0xAF, 0xD8, 0xD1, 0xCA, 0xC3, 0xFC, 0xF5, 0xEE, 0xE7,
								0x3B, 0x32, 0x29, 0x20, 0x1F, 0x16, 0x0D, 0x04, 0x73, 0x7A, 0x61, 0x68, 0x57, 0x5E, 0x45, 0x4C, 0xAB, 0xA2, 0xB9, 0xB0, 0x8F, 0x86, 0x9D, 0x94, 0xE3, 0xEA, 0xF1, 0xF8, 0xC7, 0xCE, 0xD5, 0xDC,
								0x76, 0x7F, 0x64, 0x6D, 0x52, 0x5B, 0x40, 0x49, 0x3E, 0x37, 0x2C, 0x25, 0x1A, 0x13, 0x08, 0x01, 0xE6, 0xEF, 0xF4, 0xFD, 0xC2, 0xCB, 0xD0, 0xD9, 0xAE, 0xA7, 0xBC, 0xB5, 0x8A, 0x83, 0x98, 0x91,
								0x4D, 0x44, 0x5F, 0x56, 0x69, 0x60, 0x7B, 0x72, 0x05, 0x0C, 0x17, 0x1E, 0x21, 0x28, 0x33, 0x3A, 0xDD, 0xD4, 0xCF, 0xC6, 0xF9, 0xF0, 0xEB, 0xE2, 0x95, 0x9C, 0x87, 0x8E, 0xB1, 0xB8, 0xA3, 0xAA,
								0xEC, 0xE5, 0xFE, 0xF7, 0xC8, 0xC1, 0xDA, 0xD3, 0xA4, 0xAD, 0xB6, 0xBF, 0x80, 0x89, 0x92, 0x9B, 0x7C, 0x75, 0x6E, 0x67, 0x58, 0x51, 0x4A, 0x43, 0x34, 0x3D, 0x26, 0x2F, 0x10, 0x19, 0x02, 0x0B,
								0xD7, 0xDE, 0xC5, 0xCC, 0xF3, 0xFA, 0xE1, 0xE8, 0x9F, 0x96, 0x8D, 0x84, 0xBB, 0xB2, 0xA9, 0xA0, 0x47, 0x4E, 0x55, 0x5C, 0x63, 0x6A, 0x71, 0x78, 0x0F, 0x06, 0x1D, 0x14, 0x2B, 0x22, 0x39, 0x30,
								0x9A, 0x93, 0x88, 0x81, 0xBE, 0xB7, 0xAC, 0xA5, 0xD2, 0xDB, 0xC0, 0xC9, 0xF6, 0xFF, 0xE4, 0xED, 0x0A, 0x03, 0x18, 0x11, 0x2E, 0x27, 0x3C, 0x35, 0x42, 0x4B, 0x50, 0x59, 0x66, 0x6F, 0x74, 0x7D,
								0xA1, 0xA8, 0xB3, 0xBA, 0x85, 0x8C, 0x97, 0x9E, 0xE9, 0xE0, 0xFB, 0xF2, 0xCD, 0xC4, 0xDF, 0xD6, 0x31, 0x38, 0x23, 0x2A, 0x15, 0x1C, 0x07, 0x0E, 0x79, 0x70, 0x6B, 0x62, 0x5D, 0x54, 0x4F, 0x46 };
const int maxRounds = 14;						//highest possible number of rounds
const int maxWordsKey = 8;						//maximum words for the key
//holds all of the round constants for generating each key
const unsigned int roundConstants[maxRounds+1] = {0, 0x01 << 24, 0x02 << 24, 0x04 << 24, 0x08 << 24, 
													0x10 << 24, 0x20 << 24, 0x40 << 24, 0x80 << 24, 
													0x1B << 24, 0x36 << 24};
unsigned char key[maxRounds][4][4] = { 0 };		//holds all the keys for each round
int rounds = 10;								//holds the amount of rounds we'll be processing the message for

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

/*Precondition: pass in a word that needs processed through subBytes
Postcondition: returned word has been run through subBytes!*/
unsigned int subWord(unsigned int t) {
	unsigned int newT = 0;
	//break t up into bytes and send to subbytes then put it back together again
	newT = subBytesSubs[t >> 28][(t >> 24) & 0x0F] << 24;
	newT |= subBytesSubs[(t >> 20) & 0x0F][(t >> 16) & 0x0F] << 16;
	newT |= subBytesSubs[(t >> 12 & 0x0F)][(t >> 8) & 0x0F] << 8;
	newT |= subBytesSubs[(t >> 4) & 0x0F][(t) & 0x0F];

	return newT;
}

/*Precondition: keyWords array containing first key to start generating others (needs 4 words)
Postcondition: keys global array now contains all keys for each round*/
void tenRoundKeys(unsigned int keyWords[]) {
	int rounds = 10;
	unsigned int t, upperByte, index;
	unsigned int words[44] = { 0 };
	words[0] = keyWords[0];
	words[1] = keyWords[1];
	words[2] = keyWords[2];
	words[3] = keyWords[3];

	for (int i = 1; i <= rounds; i++) {
		index = i * 4;
		//make t
		t = words[index-1];
		//one-byte left rotate
		upperByte = t >> 24;
		t <<= 8;
		t |= upperByte;
		t &= 0x0FFFFFFFF;
		//run through SubWord
		t = subWord(t);
		//add round constant
		t ^= roundConstants[i];

		//generate the four words of the key for this round
		words[index] = t ^ words[index - 4];
		words[index+1] = words[index] ^ words[index - 3];
		words[index+2] = words[index + 1] ^ words[index - 2];
		words[index+3] = words[index + 2] ^ words[index - 1];

		//store new key
		key[i][0][0] = words[index] >> 24;
		key[i][1][0] = words[index] >> 16;
		key[i][2][0] = words[index] >> 8;
		key[i][3][0] = words[index];
		key[i][0][1] = words[index+1] >> 24;
		key[i][1][1] = words[index+1] >> 16;
		key[i][2][1] = words[index+1] >> 8;
		key[i][3][1] = words[index+1];
		key[i][0][2] = words[index+2] >> 24;
		key[i][1][2] = words[index+2] >> 16;
		key[i][2][2] = words[index+2] >> 8;
		key[i][3][2] = words[index+2];
		key[i][0][3] = words[index+3] >> 24;
		key[i][1][3] = words[index+3] >> 16;
		key[i][2][3] = words[index+3] >> 8;
		key[i][3][3] = words[index+3];
	}
}

/*Precondition: keyWords array containing first key to start generating others (needs 6 words)
Postcondition: keys global array now contains all keys for each round*/
void twelveRoundKeys(unsigned int keyWords[]) {
	int rounds = 12;
	unsigned int t, upperByte, index;
	unsigned int words[78] = { 0 };
	words[0] = keyWords[0];
	words[1] = keyWords[1];
	words[2] = keyWords[2];
	words[3] = keyWords[3];
	words[4] = keyWords[4];
	words[5] = keyWords[5];

	for (int i = 1; i <= rounds; i++) {
		index = i * 6;
		//make t
		t = words[index - 1];
		//one-byte left rotate
		upperByte = t >> 24;
		t <<= 8;
		t |= upperByte;
		t &= 0x0FFFFFFFF;
		//run through SubWord
		t = subWord(t);
		//add round constant
		t ^= roundConstants[i];

		//generate the four words of the key for this round
		words[index] = t ^ words[index - 6];
		words[index + 1] = words[index] ^ words[index - 5];
		words[index + 2] = words[index + 1] ^ words[index - 4];
		words[index + 3] = words[index + 2] ^ words[index - 3];
		words[index + 4] = words[index + 3] ^ words[index - 2];
		words[index + 5] = words[index + 4] ^ words[index - 1];

		//store new key
		key[i][0][0] = words[index] >> 24;
		key[i][1][0] = words[index] >> 16;
		key[i][2][0] = words[index] >> 8;
		key[i][3][0] = words[index];
		key[i][0][1] = words[index + 1] >> 24;
		key[i][1][1] = words[index + 1] >> 16;
		key[i][2][1] = words[index + 1] >> 8;
		key[i][3][1] = words[index + 1];
		key[i][0][2] = words[index + 2] >> 24;
		key[i][1][2] = words[index + 2] >> 16;
		key[i][2][2] = words[index + 2] >> 8;
		key[i][3][2] = words[index + 2];
		key[i][0][3] = words[index + 3] >> 24;
		key[i][1][3] = words[index + 3] >> 16;
		key[i][2][3] = words[index + 3] >> 8;
		key[i][3][3] = words[index + 3];
	}
}

/*Precondition: keyWords array containing first key to start generating others (needs 8 words)
Postcondition: keys global array now contains all keys for each round*/
void fourteenRoundKeys(unsigned int keyWords[]) {
	int rounds = 14;
	unsigned int t, upperByte, index, temp;
	unsigned int words[120] = { 0 };
	words[0] = keyWords[0];
	words[1] = keyWords[1];
	words[2] = keyWords[2];
	words[3] = keyWords[3];
	words[4] = keyWords[4];
	words[5] = keyWords[5];
	words[6] = keyWords[6];
	words[7] = keyWords[7];

	for (int i = 1; i <= rounds; i++) {
		index = i * 8;
		//make t
		t = words[index - 1];
		//one-byte left rotate
		upperByte = t >> 24;
		t <<= 8;
		t |= upperByte;
		t &= 0x0FFFFFFFF;
		//run through SubWord
		t = subWord(t);
		//add round constant
		t ^= roundConstants[i];

		//generate the eight words of the key for this round
		words[index] = t ^ words[index - 8];
		words[index + 1] = words[index] ^ words[index - 7];
		words[index + 2] = words[index + 1] ^ words[index - 6];
		words[index + 3] = words[index + 2] ^ words[index - 5];
		words[index + 4] = words[index + 3] ^ words[index - 4];
		temp = subWord(words[index + 3]);
		words[index + 5] = temp ^ words[index - 3];
		words[index + 6] = words[index + 5] ^ words[index - 2];
		words[index + 7] = words[index + 6] ^ words[index - 1];

		//store new key
		key[i][0][0] = words[index] >> 24;
		key[i][1][0] = words[index] >> 16;
		key[i][2][0] = words[index] >> 8;
		key[i][3][0] = words[index];
		key[i][0][1] = words[index + 1] >> 24;
		key[i][1][1] = words[index + 1] >> 16;
		key[i][2][1] = words[index + 1] >> 8;
		key[i][3][1] = words[index + 1];
		key[i][0][2] = words[index + 2] >> 24;
		key[i][1][2] = words[index + 2] >> 16;
		key[i][2][2] = words[index + 2] >> 8;
		key[i][3][2] = words[index + 2];
		key[i][0][3] = words[index + 3] >> 24;
		key[i][1][3] = words[index + 3] >> 16;
		key[i][2][3] = words[index + 3] >> 8;
		key[i][3][3] = words[index + 3];
	}
}

/*Precondition: Key in the form of a string and the # of rounds everything will be processed in
Postcondition: All keys for each round will be found in the global key array*/
void generateKeys(string keyString, int rounds) 
{
	//check the key for the right length first
	int length = keyString.length();
	if (length != 32 && length != 18 && length != 26 && length != 48 && length != 34 && length !=64) {
		cerr << "Error: Bad Input. Specify a 32, 48, or 64 digit hex value or a 16, 24, or 32 character string surrounded by single quotes for the key.";
		exit(1);
	}
	unsigned int keyWords[maxWordsKey] = { 0 };	//holds words for key processing
	unsigned int index = 0;						//index of most significant byte of key word being processed
	if (rounds == 14) index = 7;				//index initialization depends on the number of rounds
	else if (rounds == 12) index = 5;
	else index = 3;
	if (keyString[0] == '\'' & keyString[length - 1] == '\'')	//interpret as chars
	{	//put each char into k0 until it's full
		key[0][0][0] = keyString[1];			//start at 1 not 0 - '\' is at 0
		key[0][1][0] =	keyString[2];
		key[0][2][0] =	keyString[3];
		key[0][3][0] =	keyString[4];
		//column 1		
		key[0][0][1] =	keyString[5];
		key[0][1][1] =	keyString[6];
		key[0][2][1] =	keyString[7];
		key[0][3][1] =	keyString[8];
		//column 2		
		key[0][0][2] =	keyString[9];
		key[0][1][2] =	keyString[10];
		key[0][2][2] =	keyString[11];
		key[0][3][2] =	keyString[12];
		//column 3		
		key[0][0][3] =	keyString[13];
		key[0][1][3] =	keyString[14];
		key[0][2][3] =	keyString[15];
		key[0][3][3] =	keyString[16];

		//put inputted key into form for use to generate other keys
		//only need to do for rounds not stored in key[][][] - will take care of those all at once reduces LOC
		switch (rounds) {
		case (14):	//32 bytes or 8 words
			keyWords[index--] = keyString[32] | keyString[31] << 8 | keyString[30] << 16 | keyString[29] << 24;
			keyWords[index--] = keyString[28] | keyString[27] << 8 | keyString[26] << 16 | keyString[25] << 24;
		case (12):	//24 bytes or 6 words						 					  
			keyWords[index--] = keyString[24] | keyString[23] << 8 | keyString[22] << 16 | keyString[21] << 24;
			keyWords[index--] = keyString[20] | keyString[19] << 8 | keyString[18] << 16 | keyString[17] << 24;
		}
	}
	else {
		//Store k0
		key[0][0][0] = (charHexValue(keyString[0]) << 4) | charHexValue(keyString[1]);
		key[0][1][0] = (charHexValue(keyString[2]) << 4) | charHexValue(keyString[3]);
		key[0][2][0] = (charHexValue(keyString[4]) << 4) | charHexValue(keyString[5]);
		key[0][3][0] = (charHexValue(keyString[6]) << 4) | charHexValue(keyString[7]);
		key[0][0][1] = (charHexValue(keyString[8]) << 4) | charHexValue(keyString[9]);
		key[0][1][1] = (charHexValue(keyString[10]) << 4) | charHexValue(keyString[11]);
		key[0][2][1] = (charHexValue(keyString[12]) << 4) | charHexValue(keyString[13]);
		key[0][3][1] = (charHexValue(keyString[14]) << 4) | charHexValue(keyString[15]);
		key[0][0][2] = (charHexValue(keyString[16]) << 4) | charHexValue(keyString[17]);
		key[0][1][2] = (charHexValue(keyString[18]) << 4) | charHexValue(keyString[19]);
		key[0][2][2] = (charHexValue(keyString[20]) << 4) | charHexValue(keyString[21]);
		key[0][3][2] = (charHexValue(keyString[22]) << 4) | charHexValue(keyString[23]);
		key[0][0][3] = (charHexValue(keyString[24]) << 4) | charHexValue(keyString[25]);
		key[0][1][3] = (charHexValue(keyString[26]) << 4) | charHexValue(keyString[27]);
		key[0][2][3] = (charHexValue(keyString[28]) << 4) | charHexValue(keyString[29]);
		key[0][3][3] = (charHexValue(keyString[30]) << 4) | charHexValue(keyString[31]);

		//put inputted key into form for use to generate other keys
		//only need to do for rounds not stored in key[][][] - will take care of all at once reduces LOC
		switch (rounds) {
		case (14):	//32 bytes or 8 words
			keyWords[index--] = charHexValue(keyString[63]) | charHexValue(keyString[62]) << 4 | charHexValue(keyString[61]) << 8 | charHexValue(keyString[60]) << 12
								| charHexValue(keyString[59]) << 16 | charHexValue(keyString[58]) << 20 | charHexValue(keyString[57]) << 24 | charHexValue(keyString[56]) << 28;
			keyWords[index--] = charHexValue(keyString[55]) | charHexValue(keyString[54]) << 4 | charHexValue(keyString[53]) << 8 | charHexValue(keyString[52]) << 12
								| charHexValue(keyString[51]) << 16 | charHexValue(keyString[50]) << 20 | charHexValue(keyString[49]) << 24 | charHexValue(keyString[48]) << 28;
		case (12):	//24 bytes or 6 words
			keyWords[index--] = charHexValue(keyString[47]) | charHexValue(keyString[46]) << 4 | charHexValue(keyString[45]) << 8 | charHexValue(keyString[44]) << 12
								| charHexValue(keyString[43]) << 16 | charHexValue(keyString[42]) << 20 | charHexValue(keyString[41]) << 24 | charHexValue(keyString[40]) << 28;
			keyWords[index--] = charHexValue(keyString[39]) | charHexValue(keyString[38]) << 4 | charHexValue(keyString[37]) << 8 | charHexValue(keyString[36]) << 12
								| charHexValue(keyString[35]) << 16 | charHexValue(keyString[34]) << 20 | charHexValue(keyString[33]) << 24 | charHexValue(keyString[28]);
		}
	}
	
	//store w0 through w4 into keyWords array for us to generate other round keys
	//(if rounds > 10 other words got stored in keyWords in the above if/else conditional
	//to handle based on whether inputted key was string or hex based
	keyWords[index--] = key[0][3][3] | key[0][2][3] << 8 | key[0][1][3] << 16 | key[0][0][3] << 24;
	keyWords[index--] = key[0][3][2] | key[0][2][2] << 8 | key[0][1][2] << 16 | key[0][0][2] << 24;
	keyWords[index--] = key[0][3][1] | key[0][2][1] << 8 | key[0][1][1] << 16 | key[0][0][1] << 24;
	keyWords[index--] = key[0][3][0] | key[0][2][0] << 8 | key[0][1][0] << 16 | key[0][0][0] << 24;

	//Generate other round keys
	switch (rounds) {
	case 10:	tenRoundKeys(keyWords);
				break;
	case 12:	twelveRoundKeys(keyWords);
				break;
	case 14:	fourteenRoundKeys(keyWords);
				break;
	}
}

/*Precondition: Middle-text is in the state array
Postcondition: New middle-text/encrypted message in state that has been substituted by invBytesSub*/
void subBytes() {
	//interpret byte as a two-hex digit number
	//use upper digit for the row and the lower for the column
	state[0][0] = subBytesSubs[(state[0][0] >> 4)][(state[0][0] & 0x0F)];
	state[0][1] = subBytesSubs[(state[0][1] >> 4)][(state[0][1] & 0x0F)];
	state[0][2] = subBytesSubs[(state[0][2] >> 4)][(state[0][2] & 0x0F)];
	state[0][3] = subBytesSubs[(state[0][3] >> 4)][(state[0][3] & 0x0F)];
	state[1][0] = subBytesSubs[(state[1][0] >> 4)][(state[1][0] & 0x0F)];
	state[1][1] = subBytesSubs[(state[1][1] >> 4)][(state[1][1] & 0x0F)];
	state[1][2] = subBytesSubs[(state[1][2] >> 4)][(state[1][2] & 0x0F)];
	state[1][3] = subBytesSubs[(state[1][3] >> 4)][(state[1][3] & 0x0F)];
	state[2][0] = subBytesSubs[(state[2][0] >> 4)][(state[2][0] & 0x0F)];
	state[2][1] = subBytesSubs[(state[2][1] >> 4)][(state[2][1] & 0x0F)];
	state[2][2] = subBytesSubs[(state[2][2] >> 4)][(state[2][2] & 0x0F)];
	state[2][3] = subBytesSubs[(state[2][3] >> 4)][(state[2][3] & 0x0F)];
	state[3][0] = subBytesSubs[(state[3][0] >> 4)][(state[3][0] & 0x0F)];
	state[3][1] = subBytesSubs[(state[3][1] >> 4)][(state[3][1] & 0x0F)];
	state[3][2] = subBytesSubs[(state[3][2] >> 4)][(state[3][2] & 0x0F)];
	state[3][3] = subBytesSubs[(state[3][3] >> 4)][(state[3][3] & 0x0F)];
}

/*Precondition: Middle-text is in the state array
Postcondition: New middle-text/decrypted message in state that has been substituted by invBytesSub*/
void invBytesSub() {
	//interpret byte as a two-hex digit number
	//use upper digit for the row and the lower for the column
	state[0][0] = invSubBytesSubs[(state[0][0] >> 4)][(state[0][0] & 0x0F)];
	state[0][1] = invSubBytesSubs[(state[0][1] >> 4)][(state[0][1] & 0x0F)];
	state[0][2] = invSubBytesSubs[(state[0][2] >> 4)][(state[0][2] & 0x0F)];
	state[0][3] = invSubBytesSubs[(state[0][3] >> 4)][(state[0][3] & 0x0F)];
	state[1][0] = invSubBytesSubs[(state[1][0] >> 4)][(state[1][0] & 0x0F)];
	state[1][1] = invSubBytesSubs[(state[1][1] >> 4)][(state[1][1] & 0x0F)];
	state[1][2] = invSubBytesSubs[(state[1][2] >> 4)][(state[1][2] & 0x0F)];
	state[1][3] = invSubBytesSubs[(state[1][3] >> 4)][(state[1][3] & 0x0F)];
	state[2][0] = invSubBytesSubs[(state[2][0] >> 4)][(state[2][0] & 0x0F)];
	state[2][1] = invSubBytesSubs[(state[2][1] >> 4)][(state[2][1] & 0x0F)];
	state[2][2] = invSubBytesSubs[(state[2][2] >> 4)][(state[2][2] & 0x0F)];
	state[2][3] = invSubBytesSubs[(state[2][3] >> 4)][(state[2][3] & 0x0F)];
	state[3][0] = invSubBytesSubs[(state[3][0] >> 4)][(state[3][0] & 0x0F)];
	state[3][1] = invSubBytesSubs[(state[3][1] >> 4)][(state[3][1] & 0x0F)];
	state[3][2] = invSubBytesSubs[(state[3][2] >> 4)][(state[3][2] & 0x0F)];
	state[3][3] = invSubBytesSubs[(state[3][3] >> 4)][(state[3][3] & 0x0F)];
}

/*Precondition: Middle-text is in the state array
Postcondition: New middle-text/encrypted message in state that has been permuted by shiftRows*/
void shiftRows() {
	unsigned char temp = 0;
	//do nothing to the first row of the state
	//rotate the second row left by 1
	temp =		  state[1][0];
	state[1][0] = state[1][1];
	state[1][1] = state[1][2];
	state[1][2] = state[1][3];
	state[1][3] = temp;
	//rotate the third row left by 2
	temp =		  state[2][0];
	state[2][0] = state[2][1];
	state[2][1] = state[2][2];
	state[2][2] = state[2][3];
	state[2][3] = temp;
	temp =	      state[2][0];
	state[2][0] = state[2][1];
	state[2][1] = state[2][2];
	state[2][2] = state[2][3];
	state[2][3] = temp;
	//rotate the fourth row left by 3
	temp =		  state[3][0];
	state[3][0] = state[3][1];
	state[3][1] = state[3][2];
	state[3][2] = state[3][3];
	state[3][3] = temp;
	temp = state[3][0];
	state[3][0] = state[3][1];
	state[3][1] = state[3][2];
	state[3][2] = state[3][3];
	state[3][3] = temp;
	temp = state[3][0];
	state[3][0] = state[3][1];
	state[3][1] = state[3][2];
	state[3][2] = state[3][3];
	state[3][3] = temp;
}

/*Precondition: Middle-text is in the state array
Postcondition: New middle-text/decrypted message in state that has been permuted by invShiftRows*/
void invShiftRows() {
	unsigned char temp = 0;
	//do nothing to the first row of the state
	//rotate the second row right by 1
	temp = state[1][3];
	state[1][3] = state[1][2];
	state[1][2] = state[1][1];
	state[1][1] = state[1][0];
	state[1][0] = temp;
	//rotate the third row right by 2
	temp = state[2][3];
	state[2][3] = state[2][2];
	state[2][2] = state[2][1];
	state[2][1] = state[2][0];
	state[2][0] = temp;
	temp = state[2][3];
	state[2][3] = state[2][2];
	state[2][2] = state[2][1];
	state[2][1] = state[2][0];
	state[2][0] = temp;
	//rotate the fourth row right by 3
	temp = state[3][3];
	state[3][3] = state[3][2];
	state[3][2] = state[3][1];
	state[3][1] = state[3][0];
	state[3][0] = temp;
	temp = state[3][3];
	state[3][3] = state[3][2];
	state[3][2] = state[3][1];
	state[3][1] = state[3][0];
	state[3][0] = temp;
	temp = state[3][3];
	state[3][3] = state[3][2];
	state[3][2] = state[3][1];
	state[3][1] = state[3][0];
	state[3][0] = temp;
}

/*Precondition: Middle-text is in the state array
Postcondition: New middle-text/encrypted message in state that has been mixed by mixcolumns*/
void mixColumns() {
	//do math for first column - store in temp variables and then put back into the state array
	unsigned char b1 = (gfMult2[state[0][0]] ^ gfMult3[state[1][0]] ^ state[2][0] ^ state[3][0]);
	unsigned char b2 = state[0][0] ^ gfMult2[state[1][0]] ^ gfMult3[state[2][0]] ^ state[3][0];
	unsigned char b3 = state[0][0] ^ state[1][0] ^ gfMult2[state[2][0]] ^ gfMult3[state[3][0]];
	unsigned char b4 = gfMult3[state[0][0]] ^ state[1][0] ^ state[2][0] ^ gfMult2[state[3][0]];
	state[0][0] = b1;
	state[1][0] = b2;
	state[2][0] = b3;
	state[3][0] = b4;

	//do math for second column - store in temp variables and then put back into the state array
	b1 = (gfMult2[state[0][1]] ^ gfMult3[state[1][1]] ^ state[2][1] ^ state[3][1]);
	b2 = state[0][1] ^ gfMult2[state[1][1]] ^ gfMult3[state[2][1]] ^ state[3][1];
	b3 = state[0][1] ^ state[1][1] ^ gfMult2[state[2][1]] ^ gfMult3[state[3][1]];
	b4 = gfMult3[state[0][1]] ^ state[1][1] ^ state[2][1] ^ gfMult2[state[3][1]];
	state[0][1] = b1;
	state[1][1] = b2;
	state[2][1] = b3;
	state[3][1] = b4;

	//do math for third column - store in temp variables and then put back into the state array
	b1 = (gfMult2[state[0][2]] ^ gfMult3[state[1][2]] ^ state[2][2] ^ state[3][2]);
	b2 = state[0][2] ^ gfMult2[state[1][2]] ^ gfMult3[state[2][2]] ^ state[3][2];
	b3 = state[0][2] ^ state[1][2] ^ gfMult2[state[2][2]] ^ gfMult3[state[3][2]];
	b4 = gfMult3[state[0][2]] ^ state[1][2] ^ state[2][2] ^ gfMult2[state[3][2]];
	state[0][2] = b1;
	state[1][2] = b2;
	state[2][2] = b3;
	state[3][2] = b4;

	//do math for fourth column - store in temp variables and then put back into the state array
	b1 = (gfMult2[state[0][3]] ^ gfMult3[state[1][3]] ^ state[2][3] ^ state[3][3]);
	b2 = state[0][3] ^ gfMult2[state[1][3]] ^ gfMult3[state[2][3]] ^ state[3][3];
	b3 = state[0][3] ^ state[1][3] ^ gfMult2[state[2][3]] ^ gfMult3[state[3][3]];
	b4 = gfMult3[state[0][3]] ^ state[1][3] ^ state[2][3] ^ gfMult2[state[3][3]];
	state[0][3] = b1;
	state[1][3] = b2;
	state[2][3] = b3;
	state[3][3] = b4;
}

/*Precondition: Middle-text is in the state array 
Postcondition: New middle-text/decrypted message in state that inverted by invmixcolumns*/
void invMixColumns() {
	//do math for first column - store in temp variables and then put back into the state array
	unsigned char b1 = (gfMultE[state[0][0]] ^ gfMultB[state[1][0]] ^ gfMultD[state[2][0]] ^ gfMult9[state[3][0]]);
	unsigned char b2 = gfMult9[state[0][0]] ^ gfMultE[state[1][0]] ^ gfMultB[state[2][0]] ^ gfMultD[state[3][0]];
	unsigned char b3 = gfMultD[state[0][0]] ^ gfMult9[state[1][0]] ^ gfMultE[state[2][0]] ^ gfMultB[state[3][0]];
	unsigned char b4 = gfMultB[state[0][0]] ^ gfMultD[state[1][0]] ^ gfMult9[state[2][0]] ^ gfMultE[state[3][0]];
	state[0][0] = b1;
	state[1][0] = b2;
	state[2][0] = b3;
	state[3][0] = b4;

	//do math for second column - store in temp variables and then put back into the state array
	b1 = (gfMultE[state[0][1]] ^ gfMultB[state[1][1]] ^ gfMultD[state[2][1]] ^ gfMult9[state[3][1]]);
	b2 = gfMult9[state[0][1]] ^ gfMultE[state[1][1]] ^ gfMultB[state[2][1]] ^ gfMultD[state[3][1]];
	b3 = gfMultD[state[0][1]] ^ gfMult9[state[1][1]] ^ gfMultE[state[2][1]] ^ gfMultB[state[3][1]];
	b4 = gfMultB[state[0][1]] ^ gfMultD[state[1][1]] ^ gfMult9[state[2][1]] ^ gfMultE[state[3][1]];
	state[0][1] = b1;
	state[1][1] = b2;
	state[2][1] = b3;
	state[3][1] = b4;

	//do math for third column - store in temp variables and then put back into the state array
	b1 = (gfMultE[state[0][2]] ^ gfMultB[state[1][2]] ^ gfMultD[state[2][2]] ^ gfMult9[state[3][2]]);
	b2 = gfMult9[state[0][2]] ^ gfMultE[state[1][2]] ^ gfMultB[state[2][2]] ^ gfMultD[state[3][2]];
	b3 = gfMultD[state[0][2]] ^ gfMult9[state[1][2]] ^ gfMultE[state[2][2]] ^ gfMultB[state[3][2]];
	b4 = gfMultB[state[0][2]] ^ gfMultD[state[1][2]] ^ gfMult9[state[2][2]] ^ gfMultE[state[3][2]];
	state[0][2] = b1;
	state[1][2] = b2;
	state[2][2] = b3;
	state[3][2] = b4;

	//do math for fourth column - store in temp variables and then put back into the state array
	b1 = (gfMultE[state[0][3]] ^ gfMultB[state[1][3]] ^ gfMultD[state[2][3]] ^ gfMult9[state[3][3]]);
	b2 = gfMult9[state[0][3]] ^ gfMultE[state[1][3]] ^ gfMultB[state[2][3]] ^ gfMultD[state[3][3]];
	b3 = gfMultD[state[0][3]] ^ gfMult9[state[1][3]] ^ gfMultE[state[2][3]] ^ gfMultB[state[3][3]];
	b4 = gfMultB[state[0][3]] ^ gfMultD[state[1][3]] ^ gfMult9[state[2][3]] ^ gfMultE[state[3][3]];
	state[0][3] = b1;
	state[1][3] = b2;
	state[2][3] = b3;
	state[3][3] = b4;
}

/*Precondition: Middle-text is in the state array - key number for current round must be given to function
Postcondition: New middle-text/encrypted message in state that has been whitened by the round key*/
void addRoundKey(int keyNum) {
	//literally just xoring with the round key
	state[0][0] = state[0][0] ^ key[keyNum][0][0];
	state[1][0] = state[1][0] ^ key[keyNum][1][0];
	state[2][0] = state[2][0] ^ key[keyNum][2][0];
	state[3][0] = state[3][0] ^ key[keyNum][3][0];
	state[0][1] = state[0][1] ^ key[keyNum][0][1];
	state[1][1] = state[1][1] ^ key[keyNum][1][1];
	state[2][1] = state[2][1] ^ key[keyNum][2][1];
	state[3][1] = state[3][1] ^ key[keyNum][3][1];
	state[0][2] = state[0][2] ^ key[keyNum][0][2];
	state[1][2] = state[1][2] ^ key[keyNum][1][2];
	state[2][2] = state[2][2] ^ key[keyNum][2][2];
	state[3][2] = state[3][2] ^ key[keyNum][3][2];
	state[0][3] = state[0][3] ^ key[keyNum][0][3];
	state[1][3] = state[1][3] ^ key[keyNum][1][3];
	state[2][3] = state[2][3] ^ key[keyNum][2][3];
	state[3][3] = state[3][3] ^ key[keyNum][3][3];
}

/*Precondition: Message to encrypt has been loaded into the global state array
Postcondition: Encrypted message is now in state*/
void aesEncrypt() {
	addRoundKey(0);						//whitening

	for (int i = 1; i < rounds; i++) {	//round processing
		subBytes();
		shiftRows();
		mixColumns();
		addRoundKey(i);
	}
	subBytes();							//final round
	shiftRows();
	addRoundKey(rounds);
}

/*Precondition: Message to decrypt has been loaded into the global state array
Postcondition: Decrypted message is now in state*/
void aesDecrypt() {
	addRoundKey(rounds);				//whitening

	for (int i = rounds-1; i > 0; i--) {//round processing
		invShiftRows();
		invBytesSub();
		addRoundKey(i);
		invMixColumns();
	}
	invShiftRows();						//final round processing
	invBytesSub();
	addRoundKey(0);
}

/*Precondition: 
Postcondition: fills the state array with garbage*/
void stateRandom() {
	srand(clock());								//seed the random number generator
	//fill state array with garbage
	state[0][0] = (unsigned int)rand() % 256;
	state[1][0] = (unsigned int)rand() % 256;
	state[2][0] = (unsigned int)rand() % 256;
	state[3][0] = (unsigned int)rand() % 256;
	state[0][1] = (unsigned int)rand() % 256;
	state[1][1] = (unsigned int)rand() % 256;
	state[2][1] = (unsigned int)rand() % 256;
	state[3][1] = (unsigned int)rand() % 256;
	state[0][2] = (unsigned int)rand() % 256;
	state[1][2] = (unsigned int)rand() % 256;
	state[2][2] = (unsigned int)rand() % 256;
	state[3][2] = (unsigned int)rand() % 256;
	state[0][3] = (unsigned int)rand() % 256;
	state[1][3] = (unsigned int)rand() % 256;
	state[2][3] = (unsigned int)rand() % 256;
	state[3][3] = (unsigned int)rand() % 256;
}

/*Precondition: Requires 5 input arguements in the form
				AES <–action> <key> <mode> <infile> <outfile>
Postcondition: <outfile> will contained encrypted/decrypted message*/
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
	if (arg1 == "-d" || arg1 == "-D")	isEncrypt = false;
	else if (arg1 != "-e" && arg1 != "-E")
	{
		cerr << "Error: Bad input: Action must be specified as -e or -d. <action> <key> <mode> <infile> <outfile>\n";
		exit(1);
	}

	//get the key
	string keyString = argv[2];
	rounds = keyString.length();
	if (rounds == 32 | rounds == 18) rounds = 10;		//if key is either 32 hex digits or 18 characters rounds == 10
	else if (rounds == 48 | rounds == 26) rounds = 12;	//if key is either 48 hex digits or 26 characters rounds == 12
	else if (rounds == 64 | rounds == 34) rounds = 14;	//if key is either 64 hex digits or 34 characters rounds == 14
	generateKeys(keyString, rounds);					//parses through inputted string and generates all round keys

	//lowercase the inputted mode
	string temp = argv[3];
	string mode = "";
	for (short i = 0; i < temp.length(); i++)	mode += tolower(temp[i]);

	if (!(mode == "ecb" || mode == "cbc"))
	{
		cerr << "Error: Bad Input. ECB and CBC are only accepted. <action> <key> <mode> <infile> <outfile>\n";
		exit(1);
	}

	//open input and output streams
	string infile = argv[4];
	string outfile = argv[5];

	try
	{
		unsigned int fileLength = 0; //contains length of file - yippy

		ifstream inputText(infile, ifstream::binary);
		unsigned char buffer[16];
		if (!inputText.is_open())	//see if the filestream is open
		{
			cerr << "Error: File cannot be read.";
			exit(1);
		}

		//output stream setup
		ofstream outputText(outfile, ofstream::binary);
		outputText.exceptions(ofstream::failbit | ofstream::badbit);

		if (isEncrypt)
		{
			unsigned char xorVector[4][4] = { 0 };		//This will only be used for cbc mode, but to save LOC it's declared here

			//get length of file, pad on left, encrypt, and write to file
			inputText.seekg(0, inputText.end);
			fileLength = inputText.tellg();
			inputText.seekg(0, inputText.beg);

			if (mode == "ecb") {
				stateRandom();				//puts garbage in the state
				//put file length in the last 32 bits
				state[0][3] = (fileLength >> 24);
				state[1][3] = (fileLength >> 16)	& 0x0FF;
				state[2][3] = (fileLength >> 8)		& 0x0FF;
				state[3][3] = (fileLength)			& 0x0FF;
				aesEncrypt();				//encrypt the fileLength 

				//Prep new message for output	
				buffer[0]  = state[0][0];
				buffer[1]  = state[1][0];
				buffer[2]  = state[2][0];
				buffer[3]  = state[3][0];
				buffer[4]  = state[0][1];
				buffer[5]  = state[1][1];
				buffer[6]  = state[2][1];
				buffer[7]  = state[3][1];
				buffer[8]  = state[0][2];
				buffer[9]  = state[1][2];
				buffer[10] = state[2][2];
				buffer[11] = state[3][2];
				buffer[12] = state[0][3];
				buffer[13] = state[1][3];
				buffer[14] = state[2][3];
				buffer[15] = state[3][3];
				outputText.write(reinterpret_cast<char*>(&buffer), 16);

				//Do AES stuff here
				while (inputText.read(reinterpret_cast<char*>(&buffer), 16)) {
					//Process 64 bits of input into usable form for AES
					state[0][0] = (unsigned char)(buffer[0]);
					state[1][0] = (unsigned char)(buffer[1]);
					state[2][0] = (unsigned char)(buffer[2]);
					state[3][0] = (unsigned char)(buffer[3]);
					state[0][1] = (unsigned char)(buffer[4]);
					state[1][1] = (unsigned char)(buffer[5]);
					state[2][1] = (unsigned char)(buffer[6]);
					state[3][1] = (unsigned char)(buffer[7]);
					state[0][2] = (unsigned char)(buffer[8]);
					state[1][2] = (unsigned char)(buffer[9]);
					state[2][2] = (unsigned char)(buffer[10]);
					state[3][2] = (unsigned char)(buffer[11]);
					state[0][3] = (unsigned char)(buffer[12]);
					state[1][3] = (unsigned char)(buffer[13]);
					state[2][3] = (unsigned char)(buffer[14]);
					state[3][3] = (unsigned char)(buffer[15]);
					aesEncrypt();								//encrypt

					//prep new message for output
					(buffer[0])  = state[0][0];
					(buffer[1])  = state[1][0];
					(buffer[2])  = state[2][0];
					(buffer[3])  = state[3][0];
					(buffer[4])  = state[0][1];
					(buffer[5])  = state[1][1];
					(buffer[6])  = state[2][1];
					(buffer[7])  = state[3][1];
					(buffer[8])  = state[0][2];
					(buffer[9])  = state[1][2];
					(buffer[10]) = state[2][2];
					(buffer[11]) = state[3][2];
					(buffer[12]) = state[0][3];
					(buffer[13]) = state[1][3];
					(buffer[14]) = state[2][3];
					(buffer[15]) = state[3][3];
					outputText.write(reinterpret_cast<char*>(&buffer), 16);
				}
			}
			else if (mode == "cbc") {
				//initialization vector
				stateRandom();
				xorVector[0][0] = state[0][0];
				xorVector[1][0] = state[1][0];
				xorVector[2][0] = state[2][0];
				xorVector[3][0] = state[3][0];
				xorVector[0][1] = state[0][1];
				xorVector[1][1] = state[1][1];
				xorVector[2][1] = state[2][1];
				xorVector[3][1] = state[3][1];
				xorVector[0][2] = state[0][2];
				xorVector[1][2] = state[1][2];
				xorVector[2][2] = state[2][2];
				xorVector[3][2] = state[3][2];
				xorVector[0][3] = state[0][3];
				xorVector[1][3] = state[1][3];
				xorVector[2][3] = state[2][3];
				xorVector[3][3] = state[3][3];

				//encrypt and write out IV
				aesEncrypt();
				//Prep new message for output	
				buffer[0]  = state[0][0];
				buffer[1]  = state[1][0];
				buffer[2]  = state[2][0];
				buffer[3]  = state[3][0];
				buffer[4]  = state[0][1];
				buffer[5]  = state[1][1];
				buffer[6]  = state[2][1];
				buffer[7]  = state[3][1];
				buffer[8]  = state[0][2];
				buffer[9]  = state[1][2];
				buffer[10] = state[2][2];
				buffer[11] = state[3][2];
				buffer[12] = state[0][3];
				buffer[13] = state[1][3];
				buffer[14] = state[2][3];
				buffer[15] = state[3][3];
				outputText.write(reinterpret_cast<char*>(&buffer), 16);

				//write out file Length
				stateRandom();				//puts garbage in the state
				//put file length in the last 32 bits
				state[0][3] = fileLength >> 24;
				state[1][3] = (fileLength >> 16) & 0x0FF;
				state[2][3] = (fileLength >> 8) & 0x0FF;
				state[3][3] = (fileLength) & 0x0FF;

				//xor with IV
				state[0][0] ^= xorVector[0][0];
				state[1][0] ^= xorVector[1][0];
				state[2][0] ^= xorVector[2][0];
				state[3][0] ^= xorVector[3][0];
				state[0][1] ^= xorVector[0][1];
				state[1][1] ^= xorVector[1][1];
				state[2][1] ^= xorVector[2][1];
				state[3][1] ^= xorVector[3][1];
				state[0][2] ^= xorVector[0][2];
				state[1][2] ^= xorVector[1][2];
				state[2][2] ^= xorVector[2][2];
				state[3][2] ^= xorVector[3][2];
				state[0][3] ^= xorVector[0][3];
				state[1][3] ^= xorVector[1][3];
				state[2][3] ^= xorVector[2][3];
				state[3][3] ^= xorVector[3][3];
				aesEncrypt();				//encrypt the fileLength 

				//Prep new message for output	
				buffer[0] = state[0][0];
				buffer[1] = state[1][0];
				buffer[2] = state[2][0];
				buffer[3] = state[3][0];
				buffer[4] = state[0][1];
				buffer[5] = state[1][1];
				buffer[6] = state[2][1];
				buffer[7] = state[3][1];
				buffer[8] = state[0][2];
				buffer[9] = state[1][2];
				buffer[10] = state[2][2];
				buffer[11] = state[3][2];
				buffer[12] = state[0][3];
				buffer[13] = state[1][3];
				buffer[14] = state[2][3];
				buffer[15] = state[3][3];
				outputText.write(reinterpret_cast<char*>(&buffer), 16);

				//store as new xorVector
				xorVector[0][0] = state[0][0];
				xorVector[1][0] = state[1][0];
				xorVector[2][0] = state[2][0];
				xorVector[3][0] = state[3][0];
				xorVector[0][1] = state[0][1];
				xorVector[1][1] = state[1][1];
				xorVector[2][1] = state[2][1];
				xorVector[3][1] = state[3][1];
				xorVector[0][2] = state[0][2];
				xorVector[1][2] = state[1][2];
				xorVector[2][2] = state[2][2];
				xorVector[3][2] = state[3][2];
				xorVector[0][3] = state[0][3];
				xorVector[1][3] = state[1][3];
				xorVector[2][3] = state[2][3];
				xorVector[3][3] = state[3][3];

				//Do AES stuff here
				while (inputText.read(reinterpret_cast<char*>(&buffer), 16)) {
					//Process 64 bits of input into usable form for AES
					state[0][0] = (unsigned char)(buffer[0]);
					state[1][0] = (unsigned char)(buffer[1]);
					state[2][0] = (unsigned char)(buffer[2]);
					state[3][0] = (unsigned char)(buffer[3]);
					state[0][1] = (unsigned char)(buffer[4]);
					state[1][1] = (unsigned char)(buffer[5]);
					state[2][1] = (unsigned char)(buffer[6]);
					state[3][1] = (unsigned char)(buffer[7]);
					state[0][2] = (unsigned char)(buffer[8]);
					state[1][2] = (unsigned char)(buffer[9]);
					state[2][2] = (unsigned char)(buffer[10]);
					state[3][2] = (unsigned char)(buffer[11]);
					state[0][3] = (unsigned char)(buffer[12]);
					state[1][3] = (unsigned char)(buffer[13]);
					state[2][3] = (unsigned char)(buffer[14]);
					state[3][3] = (unsigned char)(buffer[15]);

					//xor with last encrypted message
					state[0][0] ^= xorVector[0][0];
					state[1][0] ^= xorVector[1][0];
					state[2][0] ^= xorVector[2][0];
					state[3][0] ^= xorVector[3][0];
					state[0][1] ^= xorVector[0][1];
					state[1][1] ^= xorVector[1][1];
					state[2][1] ^= xorVector[2][1];
					state[3][1] ^= xorVector[3][1];
					state[0][2] ^= xorVector[0][2];
					state[1][2] ^= xorVector[1][2];
					state[2][2] ^= xorVector[2][2];
					state[3][2] ^= xorVector[3][2];
					state[0][3] ^= xorVector[0][3];
					state[1][3] ^= xorVector[1][3];
					state[2][3] ^= xorVector[2][3];
					state[3][3] ^= xorVector[3][3];
					aesEncrypt();								//encrypt

					//prep new message for output
					(buffer[0]) = state[0][0];
					(buffer[1]) = state[1][0];
					(buffer[2]) = state[2][0];
					(buffer[3]) = state[3][0];
					(buffer[4]) = state[0][1];
					(buffer[5]) = state[1][1];
					(buffer[6]) = state[2][1];
					(buffer[7]) = state[3][1];
					(buffer[8]) = state[0][2];
					(buffer[9]) = state[1][2];
					(buffer[10]) = state[2][2];
					(buffer[11]) = state[3][2];
					(buffer[12]) = state[0][3];
					(buffer[13]) = state[1][3];
					(buffer[14]) = state[2][3];
					(buffer[15]) = state[3][3];
					outputText.write(reinterpret_cast<char*>(&buffer), 16);

					//store as new xorVector
					xorVector[0][0] = state[0][0];
					xorVector[1][0] = state[1][0];
					xorVector[2][0] = state[2][0];
					xorVector[3][0] = state[3][0];
					xorVector[0][1] = state[0][1];
					xorVector[1][1] = state[1][1];
					xorVector[2][1] = state[2][1];
					xorVector[3][1] = state[3][1];
					xorVector[0][2] = state[0][2];
					xorVector[1][2] = state[1][2];
					xorVector[2][2] = state[2][2];
					xorVector[3][2] = state[3][2];
					xorVector[0][3] = state[0][3];
					xorVector[1][3] = state[1][3];
					xorVector[2][3] = state[2][3];
					xorVector[3][3] = state[3][3];
				}
			}
			//check last read to see how many bytes came
			//pad and encrypt
			int bytesLeft = inputText.gcount();
			if (bytesLeft > 0)
			{
				stateRandom();		//fill state with padding
				//Process remaining bits for AES
				switch (bytesLeft) {
				case 15: state[2][3] = (unsigned char)(buffer[14]);
				case 14: state[1][3] = (unsigned char)(buffer[13]);
				case 13: state[0][3] = (unsigned char)(buffer[12]);
				case 12: state[3][2] = (unsigned char)(buffer[11]);
				case 11: state[2][2] = (unsigned char)(buffer[10]);
				case 10: state[1][2] = (unsigned char)(buffer[9]);
				case 9:  state[0][2] = (unsigned char)(buffer[8]);
				case 8:  state[3][1] = (unsigned char)(buffer[7]);
				case 7:  state[2][1] = (unsigned char)(buffer[6]);
				case 6:  state[1][1] = (unsigned char)(buffer[5]);
				case 5:  state[0][1] = (unsigned char)(buffer[4]);
				case 4:  state[3][0] = (unsigned char)(buffer[3]);
				case 3:  state[2][0] = (unsigned char)(buffer[2]);
				case 2:  state[1][0] = (unsigned char)(buffer[1]);
				case 1:  state[0][0] = (unsigned char)(buffer[0]);
				}
				if (mode == "cbc") {
					//xor with last encrypted message
					state[0][0] ^= xorVector[0][0];
					state[1][0] ^= xorVector[1][0];
					state[2][0] ^= xorVector[2][0];
					state[3][0] ^= xorVector[3][0];
					state[0][1] ^= xorVector[0][1];
					state[1][1] ^= xorVector[1][1];
					state[2][1] ^= xorVector[2][1];
					state[3][1] ^= xorVector[3][1];
					state[0][2] ^= xorVector[0][2];
					state[1][2] ^= xorVector[1][2];
					state[2][2] ^= xorVector[2][2];
					state[3][2] ^= xorVector[3][2];
					state[0][3] ^= xorVector[0][3];
					state[1][3] ^= xorVector[1][3];
					state[2][3] ^= xorVector[2][3];
					state[3][3] ^= xorVector[3][3];
				}
				aesEncrypt();

				//prep new message for output
				(buffer[0])  = state[0][0];
				(buffer[1])  = state[1][0];
				(buffer[2])  = state[2][0];
				(buffer[3])  = state[3][0];
				(buffer[4])  = state[0][1];
				(buffer[5])  = state[1][1];
				(buffer[6])  = state[2][1];
				(buffer[7])  = state[3][1];
				(buffer[8])  = state[0][2];
				(buffer[9])  = state[1][2];
				(buffer[10]) = state[2][2];
				(buffer[11]) = state[3][2];
				(buffer[12]) = state[0][3];
				(buffer[13]) = state[1][3];
				(buffer[14]) = state[2][3];
				(buffer[15]) = state[3][3];
				outputText.write(reinterpret_cast<char*>(&buffer), 16);
			}
		}
		//decrypt
		else {	
			//read in first 16 bytes, decrypt
			inputText.read(reinterpret_cast<char*>(&buffer), 16);
			state[0][0] = (unsigned char)(buffer[0]);
			state[1][0] = (unsigned char)(buffer[1]);
			state[2][0] = (unsigned char)(buffer[2]);
			state[3][0] = (unsigned char)(buffer[3]);
			state[0][1] = (unsigned char)(buffer[4]);
			state[1][1] = (unsigned char)(buffer[5]);
			state[2][1] = (unsigned char)(buffer[6]);
			state[3][1] = (unsigned char)(buffer[7]);
			state[0][2] = (unsigned char)(buffer[8]);
			state[1][2] = (unsigned char)(buffer[9]);
			state[2][2] = (unsigned char)(buffer[10]);
			state[3][2] = (unsigned char)(buffer[11]);
			state[0][3] = (unsigned char)(buffer[12]);
			state[1][3] = (unsigned char)(buffer[13]);
			state[2][3] = (unsigned char)(buffer[14]);
			state[3][3] = (unsigned char)(buffer[15]);
			aesDecrypt();

			bool isNotLastBlock = true;		//need to know when we hit the last block to take care of padding
			unsigned long long count = 0;	//keeps track of how many characters have been processed (file length)

			if (mode == "ecb") {
				fileLength =  (unsigned int)state[0][3] << 24 | ((unsigned int)state[1][3] << 16) | ((unsigned int)state[2][3] << 8) |((unsigned int)state[3][3]);
				
				while (inputText.read(reinterpret_cast<char*>(&buffer), 16)) {	//grab 16 characters
					//Process 64 bits of input into usable form for AES
					state[0][0] = (unsigned char)(buffer[0]);
					state[1][0] = (unsigned char)(buffer[1]);
					state[2][0] = (unsigned char)(buffer[2]);
					state[3][0] = (unsigned char)(buffer[3]);
					state[0][1] = (unsigned char)(buffer[4]);
					state[1][1] = (unsigned char)(buffer[5]);
					state[2][1] = (unsigned char)(buffer[6]);
					state[3][1] = (unsigned char)(buffer[7]);
					state[0][2] = (unsigned char)(buffer[8]);
					state[1][2] = (unsigned char)(buffer[9]);
					state[2][2] = (unsigned char)(buffer[10]);
					state[3][2] = (unsigned char)(buffer[11]);
					state[0][3] = (unsigned char)(buffer[12]);
					state[1][3] = (unsigned char)(buffer[13]);
					state[2][3] = (unsigned char)(buffer[14]);
					state[3][3] = (unsigned char)(buffer[15]);
					aesDecrypt();				//decrypt

					//prep new message for output
					(buffer[0])  = state[0][0];
					(buffer[1])  = state[1][0];
					(buffer[2])  = state[2][0];
					(buffer[3])  = state[3][0];
					(buffer[4])  = state[0][1];
					(buffer[5])  = state[1][1];
					(buffer[6])  = state[2][1];
					(buffer[7])  = state[3][1];
					(buffer[8])  = state[0][2];
					(buffer[9])  = state[1][2];
					(buffer[10]) = state[2][2];
					(buffer[11]) = state[3][2];
					(buffer[12]) = state[0][3];
					(buffer[13]) = state[1][3];
					(buffer[14]) = state[2][3];
					(buffer[15]) = state[3][3];

					count += 16;							//processed 16 more characters - add to count
					isNotLastBlock = count <= fileLength;	//less than filelength? haven't reached the last block
					if (isNotLastBlock)	outputText.write(reinterpret_cast<char*>(&buffer), 16);
					else                outputText.write(reinterpret_cast<char*>(&buffer), fileLength % 16);	//reached last block only output some of characters (not the padding)
				}
			}
			else if (mode == "cbc") {
				//get the IV
				unsigned char xorVector[4][4] = { 0 };
				xorVector[0][0] = state[0][0];
				xorVector[1][0] = state[1][0];
				xorVector[2][0] = state[2][0];
				xorVector[3][0] = state[3][0];
				xorVector[0][1] = state[0][1];
				xorVector[1][1] = state[1][1];
				xorVector[2][1] = state[2][1];
				xorVector[3][1] = state[3][1];
				xorVector[0][2] = state[0][2];
				xorVector[1][2] = state[1][2];
				xorVector[2][2] = state[2][2];
				xorVector[3][2] = state[3][2];
				xorVector[0][3] = state[0][3];
				xorVector[1][3] = state[1][3];
				xorVector[2][3] = state[2][3];
				xorVector[3][3] = state[3][3];

				//get file Length
				inputText.read(reinterpret_cast<char*>(&buffer), 16);
				state[0][0] = (unsigned char)(buffer[0]);
				state[1][0] = (unsigned char)(buffer[1]);
				state[2][0] = (unsigned char)(buffer[2]);
				state[3][0] = (unsigned char)(buffer[3]);
				state[0][1] = (unsigned char)(buffer[4]);
				state[1][1] = (unsigned char)(buffer[5]);
				state[2][1] = (unsigned char)(buffer[6]);
				state[3][1] = (unsigned char)(buffer[7]);
				state[0][2] = (unsigned char)(buffer[8]);
				state[1][2] = (unsigned char)(buffer[9]);
				state[2][2] = (unsigned char)(buffer[10]);
				state[3][2] = (unsigned char)(buffer[11]);
				state[0][3] = (unsigned char)(buffer[12]);
				state[1][3] = (unsigned char)(buffer[13]);
				state[2][3] = (unsigned char)(buffer[14]);
				state[3][3] = (unsigned char)(buffer[15]);
				aesDecrypt();

				//xor with IV
				state[0][0] ^= xorVector[0][0];
				state[1][0] ^= xorVector[1][0];
				state[2][0] ^= xorVector[2][0];
				state[3][0] ^= xorVector[3][0];
				state[0][1] ^= xorVector[0][1];
				state[1][1] ^= xorVector[1][1];
				state[2][1] ^= xorVector[2][1];
				state[3][1] ^= xorVector[3][1];
				state[0][2] ^= xorVector[0][2];
				state[1][2] ^= xorVector[1][2];
				state[2][2] ^= xorVector[2][2];
				state[3][2] ^= xorVector[3][2];
				state[0][3] ^= xorVector[0][3];
				state[1][3] ^= xorVector[1][3];
				state[2][3] ^= xorVector[2][3];
				state[3][3] ^= xorVector[3][3];

				fileLength = (unsigned int)state[0][3] << 24 | ((unsigned int)state[1][3] << 16) | ((unsigned int)state[2][3] << 8) | ((unsigned int)state[3][3]);

				//store new xorVector
				xorVector[0][0] = (unsigned char)(buffer[0]);
				xorVector[1][0] = (unsigned char)(buffer[1]);
				xorVector[2][0] = (unsigned char)(buffer[2]);
				xorVector[3][0] = (unsigned char)(buffer[3]);
				xorVector[0][1] = (unsigned char)(buffer[4]);
				xorVector[1][1] = (unsigned char)(buffer[5]);
				xorVector[2][1] = (unsigned char)(buffer[6]);
				xorVector[3][1] = (unsigned char)(buffer[7]);
				xorVector[0][2] = (unsigned char)(buffer[8]);
				xorVector[1][2] = (unsigned char)(buffer[9]);
				xorVector[2][2] = (unsigned char)(buffer[10]);
				xorVector[3][2] = (unsigned char)(buffer[11]);
				xorVector[0][3] = (unsigned char)(buffer[12]);
				xorVector[1][3] = (unsigned char)(buffer[13]);
				xorVector[2][3] = (unsigned char)(buffer[14]);
				xorVector[3][3] = (unsigned char)(buffer[15]);

				//Do AES stuff here
				while (inputText.read(reinterpret_cast<char*>(&buffer), 16)) {	//read in 16 characters
					//Process 64 bits of input into usable form for AES
					state[0][0] = (unsigned char)(buffer[0]);
					state[1][0] = (unsigned char)(buffer[1]);
					state[2][0] = (unsigned char)(buffer[2]);
					state[3][0] = (unsigned char)(buffer[3]);
					state[0][1] = (unsigned char)(buffer[4]);
					state[1][1] = (unsigned char)(buffer[5]);
					state[2][1] = (unsigned char)(buffer[6]);
					state[3][1] = (unsigned char)(buffer[7]);
					state[0][2] = (unsigned char)(buffer[8]);
					state[1][2] = (unsigned char)(buffer[9]);
					state[2][2] = (unsigned char)(buffer[10]);
					state[3][2] = (unsigned char)(buffer[11]);
					state[0][3] = (unsigned char)(buffer[12]);
					state[1][3] = (unsigned char)(buffer[13]);
					state[2][3] = (unsigned char)(buffer[14]);
					state[3][3] = (unsigned char)(buffer[15]);
					aesDecrypt();					//decrypt

					//xor with IV
					state[0][0] ^= xorVector[0][0];
					state[1][0] ^= xorVector[1][0];
					state[2][0] ^= xorVector[2][0];
					state[3][0] ^= xorVector[3][0];
					state[0][1] ^= xorVector[0][1];
					state[1][1] ^= xorVector[1][1];
					state[2][1] ^= xorVector[2][1];
					state[3][1] ^= xorVector[3][1];
					state[0][2] ^= xorVector[0][2];
					state[1][2] ^= xorVector[1][2];
					state[2][2] ^= xorVector[2][2];
					state[3][2] ^= xorVector[3][2];
					state[0][3] ^= xorVector[0][3];
					state[1][3] ^= xorVector[1][3];
					state[2][3] ^= xorVector[2][3];
					state[3][3] ^= xorVector[3][3];

					//store new xorVector
					xorVector[0][0] = (unsigned char)(buffer[0]);
					xorVector[1][0] = (unsigned char)(buffer[1]);
					xorVector[2][0] = (unsigned char)(buffer[2]);
					xorVector[3][0] = (unsigned char)(buffer[3]);
					xorVector[0][1] = (unsigned char)(buffer[4]);
					xorVector[1][1] = (unsigned char)(buffer[5]);
					xorVector[2][1] = (unsigned char)(buffer[6]);
					xorVector[3][1] = (unsigned char)(buffer[7]);
					xorVector[0][2] = (unsigned char)(buffer[8]);
					xorVector[1][2] = (unsigned char)(buffer[9]);
					xorVector[2][2] = (unsigned char)(buffer[10]);
					xorVector[3][2] = (unsigned char)(buffer[11]);
					xorVector[0][3] = (unsigned char)(buffer[12]);
					xorVector[1][3] = (unsigned char)(buffer[13]);
					xorVector[2][3] = (unsigned char)(buffer[14]);
					xorVector[3][3] = (unsigned char)(buffer[15]);

					//prep new message for output
					(buffer[0])  = state[0][0];
					(buffer[1])  = state[1][0];
					(buffer[2])  = state[2][0];
					(buffer[3])  = state[3][0];
					(buffer[4])  = state[0][1];
					(buffer[5])  = state[1][1];
					(buffer[6])  = state[2][1];
					(buffer[7])  = state[3][1];
					(buffer[8])  = state[0][2];
					(buffer[9])  = state[1][2];
					(buffer[10]) = state[2][2];
					(buffer[11]) = state[3][2];
					(buffer[12]) = state[0][3];
					(buffer[13]) = state[1][3];
					(buffer[14]) = state[2][3];
					(buffer[15]) = state[3][3];

					count += 16;							//processed 16 more characters - add to count
					isNotLastBlock = count <= fileLength;	//less than filelength? haven't reached the last block
					if (isNotLastBlock)	outputText.write(reinterpret_cast<char*>(&buffer), 16);
					else                outputText.write(reinterpret_cast<char*>(&buffer), fileLength % 16);	//reached last block only output some of characters (not the padding)
				}
			}
		}
	}
	catch (exception e)	//Houston, we have a problem
	{
		cerr << e.what() << endl;
		exit(1);
	}
	//print out how long it took to run
	clock_t runtime = clock() - start;
	cout << runtime / 1000 << "." << runtime % 1000 << " s" << endl;
}//close main