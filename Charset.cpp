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

#include "Main.h"
#include <iostream>
#include <string>
#include "Charset.h"

using namespace std;


// Charset blocks.
const char h_charset_num[] = "0123456789";
const char h_charset_alpha_l[] = "abcdefghijklmnopqrstuvwxyz";
const char h_charset_alpha_h[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char h_charset_printable[] = " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

// Constructor.
CCharset::CCharset()
{
}

// Destructor.
CCharset::~CCharset()
{
	SAFE_DELETE(m_cCharset);
}

// Init.
bool CCharset::Initialize(string chars)
{
	// Length plausible?
	if (chars.size() > 4)
		return false;

	// Fill.
	string charset;
	for (size_t i = 0; i < chars.size(); i++) {
		if (chars[i] == 'c')
			charset += h_charset_alpha_h;

		if (chars[i] == 's')
			charset += h_charset_alpha_l;

		if (chars[i] == 'd')
			charset += h_charset_num;

		if (chars[i] == 'x')
			charset += h_charset_printable;
	}

	// Save length.
	m_iLen = static_cast<int>(charset.size());

	// Save charset.
	m_cCharset = new char[m_iLen + 1];
	m_cCharset[m_iLen] = '\0';
	memcpy(m_cCharset, charset.c_str(), m_iLen);

	// Sucess.
	return true;
}

int CCharset::GetAt(const int pos)
{
	return m_cCharset[pos];
}

size_t CCharset::GetLen()
{
	return m_iLen;
}

char *CCharset::GetCharset()
{
	return m_cCharset;
}

__int64 CCharset::GetCombs(const int forLen)
{
	__int64 combs = 1;
	for (int i = 0; i < forLen; i++)
		combs *= m_iLen;
	return combs;
}

__int64 CCharset::GetAllCombs(const int min, const int max)
{
	__int64 combs = 0;
	for (int i = min; i <= max; i++)
		combs += GetCombs(i);

	return combs;
}