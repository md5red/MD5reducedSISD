// MD5.cpp - Modification (c) 2011 Tobias Sielaff.
//
// Thanks for the idea to reverse of the last round and for very
// valuable suggestions to sc00bz.
// 
// Also thanks to OpenSSL for their streamlined implementation of
// F and G.
//
// Based on:
// MD5.CC - source code for the C++/object oriented translation and 
//          modification of MD5.
//
// Translation and modification (c) 1995 by Mordechai T. Abzug 
//
// This translation/ modification is provided "as is," without express or 
// implied warranty of any kind.
//
// The translator/ modifier does not claim (1) that MD5 will do what you think 
// it does; (2) that this translation/ modification is accurate; or (3) that 
// this software is "merchantible."  (Language for this disclaimer partially 
// copied from the disclaimer below).
//
// Based on:
// MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm
// MDDRIVER.C - test driver for MD2, MD4 and MD5
//
// Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
// rights reserved.
//
// License to copy and use this software is granted provided that it
// is identified as the "RSA Data Security, Inc. MD5 Message-Digest
// Algorithm" in all material mentioning or referencing this software
// or this function.
//
// License is also granted to make and use derivative works provided
// that such works are identified as "derived from the RSA Data
// Security, Inc. MD5 Message-Digest Algorithm" in all material
// mentioning or referencing the derived work.
//
// RSA Data Security, Inc. makes no representations concerning either
// the merchantability of this software or the suitability of this
// software for any particular purpose. It is provided "as is"
// without express or implied warranty of any kind.
//
// These notices must be retained in any copies of any part of this
// documentation and/or software.

#include "Main.h"
#include <boost/thread.hpp>
#include "MD5.h"

using namespace std;


// Constants for MD5Transform routine.
// Although we could use C++ style constants, defines are actually better,
// since they let us easily evade scope clashes.
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

// Orginal state, pre-reversed.
uint4 state_org[4];

// Create 128bit struct for comparing
uint4 *chk_a, *chk_b, *chk_c, *chk_d;

/* F, G, H and I are basic MD5 functions.
*/
// F and G taken straight from the OpenSSL implementation.
#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z)))) 
#define G(x, y, z) ((y) ^ ((z) & ((x) ^ (y)))) 
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
*/
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
#define ROTATE_RIGHT(x, n)  (((x) >> (n)) | ((x) << (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
*/
#define FF(a, b, c, d, x, s, ac) { \
	(a) += F ((b), (c), (d)) + (x) + (uint4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
	(a) += G ((b), (c), (d)) + (x) + (uint4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
	(a) += H ((b), (c), (d)) + (x) + (uint4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
	(a) += I ((b), (c), (d)) + (x) + (uint4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define II_REV(a, b, c, d, x, s, ac) \
	(a) = ROTATE_RIGHT(((a) - (b)), (s)) - (x) - (uint4)(ac) - I((b), (c), (d));


// Initialization.
bool MD5Init(string hash)
{
	// Length correct?
	if (hash.size() != 32)
		return false;

	// Only hex?
	for (size_t i = 0; i < hash.size(); i++)
		if (!isxdigit(hash[i]))
			return false;

	// Temporary var.
	uint1 digest_rev[16];

	// Turn hex encoded string back into bytes.
	for (unsigned int i = 0; i < hash.size() / 2; i++) {
		int val = 0;
		stringstream ss;
		ss << hash.substr(i * 2, 2);
		ss >> setprecision(2) >> hex >> val;
		digest_rev[i] = (uint1)val;
	}

	// Copy everything to our state array.
	memcpy(state_org, digest_rev, 16);

	// Substract the magic.
	state_org[0] -= 0x67452301;
	state_org[1] -= 0xefcdab89;
	state_org[2] -= 0x98badcfe;
	state_org[3] -= 0x10325476;

	// Init the compare array (thread-safety).
	// A very big thank you to "Nebenläufige Programmierung".
	chk_a = new uint4[boost::thread::hardware_concurrency()];
	chk_b = new uint4[boost::thread::hardware_concurrency()];
	chk_c = new uint4[boost::thread::hardware_concurrency()];
	chk_d = new uint4[boost::thread::hardware_concurrency()];

	// Everything fine.
	return true;
}

// Reverse the last round of MD5 once data[1] changes.
// Why not strlen? I don't null-terminate the plain.
void MD5Reverse(const char* plain, const int len, const int threadIdx)
{
	// Create x.
	const uint4 *x = (uint4*)&plain[0];

	// Init.
	uint4 revA = state_org[0];
	uint4 revB = state_org[1];
	uint4 revC = state_org[2];
	uint4 revD = state_org[3];
	
	// Reverse.		
	II_REV (revB, revC, revD, revA,      0, S44, 0xeb86d391); // 64
	II_REV (revC, revD, revA, revB,      0, S43, 0x2ad7d2bb); // 63
	II_REV (revD, revA, revB, revC,      0, S42, 0xbd3af235); // 62
	II_REV (revA, revB, revC, revD,      0, S41, 0xf7537e82); // 61
	II_REV (revB, revC, revD, revA,      0, S44, 0x4e0811a1); // 60
	II_REV (revC, revD, revA, revB,      0, S43, 0xa3014314); // 59
	II_REV (revD, revA, revB, revC,      0, S42, 0xfe2ce6e0); // 58
	II_REV (revA, revB, revC, revD,      0, S41, 0x6fa87e4f); // 57
	II_REV (revB, revC, revD, revA,   x[1], S44, 0x85845dd1); // 56
	II_REV (revC, revD, revA, revB,      0, S43, 0xffeff47d); // 55
	II_REV (revD, revA, revB, revC,      0, S42, 0x8f0ccc92); // 54
	II_REV (revA, revB, revC, revD,      0, S41, 0x655b59c3); // 53
	II_REV (revB, revC, revD, revA,      0, S44, 0xfc93a039); // 52
	II_REV (revC, revD, revA, revB, len<<3, S43, 0xab9423a7); // 51
	II_REV (revD, revA, revB, revC,      0, S42, 0x432aff97); // 50
	II_REV (revA, revB, revC, revD,      0, S41, 0xf4292244); // 49

	// Save it (thread-dependend memory location).
	chk_a[threadIdx] = revA;
	chk_b[threadIdx] = revB;
	chk_c[threadIdx] = revC;
	chk_d[threadIdx] = revD;
}

// Same as above, but can be used for plains with length 8 to 11.
// Compared to the routine above, this is a bit slower because we need to access x[2].
void MD5Reverse_8plus(const char* plain, const int len, const int threadIdx)
{
	// Create x.
	const uint4 *x = (uint4*)&plain[0];

	// Init.
	uint4 revA = state_org[0];
	uint4 revB = state_org[1];
	uint4 revC = state_org[2];
	uint4 revD = state_org[3];
			
	// Reverse.		
	II_REV (revB, revC, revD, revA,      0, S44, 0xeb86d391); // 64
	II_REV (revC, revD, revA, revB,   x[2], S43, 0x2ad7d2bb); // 63
	II_REV (revD, revA, revB, revC,      0, S42, 0xbd3af235); // 62
	II_REV (revA, revB, revC, revD,      0, S41, 0xf7537e82); // 61
	II_REV (revB, revC, revD, revA,      0, S44, 0x4e0811a1); // 60
	II_REV (revC, revD, revA, revB,      0, S43, 0xa3014314); // 59
	II_REV (revD, revA, revB, revC,      0, S42, 0xfe2ce6e0); // 58
	II_REV (revA, revB, revC, revD,      0, S41, 0x6fa87e4f); // 57
	II_REV (revB, revC, revD, revA,   x[1], S44, 0x85845dd1); // 56
	II_REV (revC, revD, revA, revB,      0, S43, 0xffeff47d); // 55
	II_REV (revD, revA, revB, revC,      0, S42, 0x8f0ccc92); // 54
	II_REV (revA, revB, revC, revD,      0, S41, 0x655b59c3); // 53
	II_REV (revB, revC, revD, revA,      0, S44, 0xfc93a039); // 52
	II_REV (revC, revD, revA, revB, len<<3, S43, 0xab9423a7); // 51
	II_REV (revD, revA, revB, revC,      0, S42, 0x432aff97); // 50
	II_REV (revA, revB, revC, revD,      0, S41, 0xf4292244); // 49

	// Save it (thread-dependend memory location).
	chk_a[threadIdx] = revA;
	chk_b[threadIdx] = revB;
	chk_c[threadIdx] = revC;
	chk_d[threadIdx] = revD;
}

// Calculation.
bool MD5Calculate(const char word[8], const int len, const int threadIdx)
{
	// Initialize hash value for this chunk:
	uint4 a = 0x67452301;
	uint4 b = 0xEFCDAB89;
	uint4 c = 0x98BADCFE;
	uint4 d = 0x10325476;

	// The length as litte endian.
	int len_le = len << 3;

	// Set x.
	const uint4 *x = (uint4*)&word[0];

	/* Round 1 */
	FF (a, b, c, d,   x[0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c,   x[1], S12, 0xe8c7b756); /* 2 */
	FF (c, d, a, b,      0, S13, 0x242070db); /* 3 */
	FF (b, c, d, a,      0, S14, 0xc1bdceee); /* 4 */
	FF (a, b, c, d,      0, S11, 0xf57c0faf); /* 5 */
	FF (d, a, b, c,      0, S12, 0x4787c62a); /* 6 */
	FF (c, d, a, b,      0, S13, 0xa8304613); /* 7 */
	FF (b, c, d, a,      0, S14, 0xfd469501); /* 8 */
	FF (a, b, c, d,      0, S11, 0x698098d8); /* 9 */
	FF (d, a, b, c,      0, S12, 0x8b44f7af); /* 10 */
	FF (c, d, a, b,      0, S13, 0xffff5bb1); /* 11 */
	FF (b, c, d, a,      0, S14, 0x895cd7be); /* 12 */
	FF (a, b, c, d,      0, S11, 0x6b901122); /* 13 */
	FF (d, a, b, c,      0, S12, 0xfd987193); /* 14 */
	FF (c, d, a, b, len_le, S13, 0xa679438e); /* 15 */
	FF (b, c, d, a,      0, S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG (a, b, c, d,   x[1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c,      0, S22, 0xc040b340); /* 18 */
	GG (c, d, a, b,      0, S23, 0x265e5a51); /* 19 */
	GG (b, c, d, a,   x[0], S24, 0xe9b6c7aa); /* 20 */
	GG (a, b, c, d,      0, S21, 0xd62f105d); /* 21 */
	GG (d, a, b, c,      0, S22, 0x2441453 ); /* 22 */
	GG (c, d, a, b,      0, S23, 0xd8a1e681); /* 23 */
	GG (b, c, d, a,      0, S24, 0xe7d3fbc8); /* 24 */
	GG (a, b, c, d,      0, S21, 0x21e1cde6); /* 25 */
	GG (d, a, b, c, len_le, S22, 0xc33707d6); /* 26 */
	GG (c, d, a, b,      0, S23, 0xf4d50d87); /* 27 */
	GG (b, c, d, a,      0, S24, 0x455a14ed); /* 28 */
	GG (a, b, c, d,      0, S21, 0xa9e3e905); /* 29 */
	GG (d, a, b, c,      0, S22, 0xfcefa3f8); /* 30 */
	GG (c, d, a, b,      0, S23, 0x676f02d9); /* 31 */
	GG (b, c, d, a,      0, S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH (a, b, c, d,      0, S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c,      0, S32, 0x8771f681); /* 34 */
	HH (c, d, a, b,      0, S33, 0x6d9d6122); /* 35 */
	HH (b, c, d, a, len_le, S34, 0xfde5380c); /* 36 */
	HH (a, b, c, d,   x[1], S31, 0xa4beea44); /* 37 */
	HH (d, a, b, c,      0, S32, 0x4bdecfa9); /* 38 */
	HH (c, d, a, b,      0, S33, 0xf6bb4b60); /* 39 */
	HH (b, c, d, a,      0, S34, 0xbebfbc70); /* 40 */
	HH (a, b, c, d,      0, S31, 0x289b7ec6); /* 41 */
	HH (d, a, b, c,   x[0], S32, 0xeaa127fa); /* 42 */
	HH (c, d, a, b,      0, S33, 0xd4ef3085); /* 43 */
	HH (b, c, d, a,      0, S34, 0x4881d05 ); /* 44 */
	HH (a, b, c, d,      0, S31, 0xd9d4d039); /* 45 */
	// Rest skipped.

	/* Round 4 */
	// Oh round 4, where art thou?

	// Create temporary a.
	unsigned int tmp_a = a + x[0];

	// Check
	if (tmp_a == chk_a[threadIdx]) {
		// Compute the last 3 steps to achieve 100% certainty.
		HH (d, a, b, c,    0, S32, 0xe6db99e5); /* 46 */
		HH (c, d, a, b,    0, S33, 0x1fa27cf8); /* 47 */
		HH (b, c, d, a,    0, S34, 0xc4ac5665); /* 48 */

		// Yes, this could be optimized further. But it gets called
		// so seldomly, it doesn't matter.
		if (b == chk_b[threadIdx] && c == chk_c[threadIdx] && d == chk_d[threadIdx])
			return true;
	}

	// Not found.
	return false;
}

// Same as above, but can be used for plains with length 8 to 11.
// Compared to the routine above, this is ~1M slower because we need to access x[2].
bool MD5Calculate_8plus(const char word[12], const int len, const int threadIdx)
{
	// Initialize hash value for this chunk:
	uint4 a = 0x67452301;
	uint4 b = 0xEFCDAB89;
	uint4 c = 0x98BADCFE;
	uint4 d = 0x10325476;

	// The length as litte endian.
	int len_le = len << 3;

	// Set x.
	const uint4 *x = (uint4*)&word[0];

	/* Round 1 */
	FF (a, b, c, d,   x[0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c,   x[1], S12, 0xe8c7b756); /* 2 */
	FF (c, d, a, b,   x[2], S13, 0x242070db); /* 3 */
	FF (b, c, d, a,      0, S14, 0xc1bdceee); /* 4 */
	FF (a, b, c, d,      0, S11, 0xf57c0faf); /* 5 */
	FF (d, a, b, c,      0, S12, 0x4787c62a); /* 6 */
	FF (c, d, a, b,      0, S13, 0xa8304613); /* 7 */
	FF (b, c, d, a,      0, S14, 0xfd469501); /* 8 */
	FF (a, b, c, d,      0, S11, 0x698098d8); /* 9 */
	FF (d, a, b, c,      0, S12, 0x8b44f7af); /* 10 */
	FF (c, d, a, b,      0, S13, 0xffff5bb1); /* 11 */
	FF (b, c, d, a,      0, S14, 0x895cd7be); /* 12 */
	FF (a, b, c, d,      0, S11, 0x6b901122); /* 13 */
	FF (d, a, b, c,      0, S12, 0xfd987193); /* 14 */
	FF (c, d, a, b, len_le, S13, 0xa679438e); /* 15 */
	FF (b, c, d, a,      0, S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG (a, b, c, d,   x[1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c,      0, S22, 0xc040b340); /* 18 */
	GG (c, d, a, b,      0, S23, 0x265e5a51); /* 19 */
	GG (b, c, d, a,   x[0], S24, 0xe9b6c7aa); /* 20 */
	GG (a, b, c, d,      0, S21, 0xd62f105d); /* 21 */
	GG (d, a, b, c,      0, S22, 0x2441453 ); /* 22 */
	GG (c, d, a, b,      0, S23, 0xd8a1e681); /* 23 */
	GG (b, c, d, a,      0, S24, 0xe7d3fbc8); /* 24 */
	GG (a, b, c, d,      0, S21, 0x21e1cde6); /* 25 */
	GG (d, a, b, c, len_le, S22, 0xc33707d6); /* 26 */
	GG (c, d, a, b,      0, S23, 0xf4d50d87); /* 27 */
	GG (b, c, d, a,      0, S24, 0x455a14ed); /* 28 */
	GG (a, b, c, d,      0, S21, 0xa9e3e905); /* 29 */
	GG (d, a, b, c,   x[2], S22, 0xfcefa3f8); /* 30 */
	GG (c, d, a, b,      0, S23, 0x676f02d9); /* 31 */
	GG (b, c, d, a,      0, S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH (a, b, c, d,      0, S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c,      0, S32, 0x8771f681); /* 34 */
	HH (c, d, a, b,      0, S33, 0x6d9d6122); /* 35 */
	HH (b, c, d, a, len_le, S34, 0xfde5380c); /* 36 */
	HH (a, b, c, d,   x[1], S31, 0xa4beea44); /* 37 */
	HH (d, a, b, c,      0, S32, 0x4bdecfa9); /* 38 */
	HH (c, d, a, b,      0, S33, 0xf6bb4b60); /* 39 */
	HH (b, c, d, a,      0, S34, 0xbebfbc70); /* 40 */
	HH (a, b, c, d,      0, S31, 0x289b7ec6); /* 41 */
	HH (d, a, b, c,   x[0], S32, 0xeaa127fa); /* 42 */
	HH (c, d, a, b,      0, S33, 0xd4ef3085); /* 43 */
	HH (b, c, d, a,      0, S34, 0x4881d05 ); /* 44 */
	HH (a, b, c, d,      0, S31, 0xd9d4d039); /* 45 */
	// Rest skipped.

	/* Round 4 */
	// Oh round 4, where art thou?

	// Create temporary a.
	unsigned int tmp_a = a + x[0];

	// Check
	if (tmp_a == chk_a[threadIdx]) {
		// Compute the last 3 steps to achieve 100% certainty.
		HH (d, a, b, c,    0, S32, 0xe6db99e5); /* 46 */
		HH (c, d, a, b,    0, S33, 0x1fa27cf8); /* 47 */
		HH (b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */

		// Yes, this could be optimized further. But it gets called
		// so seldomly, it doesn't matter.
		if (b == chk_b[threadIdx] && c == chk_c[threadIdx] && d == chk_d[threadIdx])
			return true;
	}

	// Not found.
	return false;
}

// Cleanup.
void MD5Cleanup()
{
	delete[] chk_a;
	delete[] chk_b;
	delete[] chk_c;
	delete[] chk_d;
}