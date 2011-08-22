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
#include "Timer.h"

using namespace std;


// Constructor.
CTimer::CTimer()
{
}

// Destructor.
CTimer::~CTimer()
{
}

// Start
void CTimer::Start()
{
	// Go!
	m_ptStart = boost::posix_time::microsec_clock::local_time();
}

// Elapsed time.
int CTimer::Elapsed()
{
	// Get the end time.
	m_ptEnd = boost::posix_time::microsec_clock::local_time();

	// Return ms.
	return static_cast<int>((m_ptEnd - m_ptStart).total_milliseconds());
}

// Reset.
void CTimer::Reset()
{
	// Set both times to a new starting value.
	m_ptStart = boost::posix_time::microsec_clock::local_time();
	m_ptEnd = boost::posix_time::microsec_clock::local_time();
}