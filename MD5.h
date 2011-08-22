/*
MD5reducedSISD
Copyright (C) 2011 Tobias Sielaff

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MD5_H
 #define MD5_H

// Some types:
typedef unsigned       int uint4; // assumes integer is 4 words long
typedef unsigned short int uint2; // assumes short integer is 2 words long
typedef unsigned      char uint1; // assumes char is 1 word long

// Some methods:
bool MD5Init(std::string hash);
void MD5Reverse(const char* plain, const int len, const int threadIdx);
void MD5Reverse_8plus(const char* plain, const int len, const int threadIdx);
bool MD5Calculate(const char word[8], const int len, const int threadIdx);
bool MD5Calculate_8plus(const char word[8], const int len, const int threadIdx);
void MD5Cleanup();


#endif