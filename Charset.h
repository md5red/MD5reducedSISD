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

#ifndef CHARSET_H
 #define CHARSET_H


#include <string>

// The class.
class CCharset
{
private:
	char* m_cCharset;
	int m_iLen;

public:
	CCharset();
	~CCharset();

	bool Initialize(std::string chars);

	int GetAt(const int pos); // Gets the char at the given position.
	size_t GetLen(); // Returns the length of our charset.
	char *GetCharset(); // Returns the complete charset.

	__int64 GetCombs(const int forLen); // Returns possible combinations for the given length.
	__int64 GetAllCombs(const int min, const int max); // Same as above, but as range interval.
};


#endif
	