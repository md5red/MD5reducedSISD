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

#ifndef STATS_H
 #define STATS_H


#include "Timer.h"
#include "Plain.h"
#include "Charset.h"
#include <boost/thread.hpp>

class CStats
{
private:
	__int64 *m_iSumComputed; // per Thread.

	CTimer *m_tLastPrint; // Don't print every single update.
	CTimer *m_tOverall; // Overall counter.

	boost::mutex m_mPrint; // Our mutex for locking down the printing.

public:
	CStats();
	~CStats();

	void Update(CPlain *forPlain, CCharset *forCharset, const int threadIdx); // Update our stats for the specified thread.

	double GetSpeed(); // In MHash/second.
	double GetProgress(CPlain *forPlain, CCharset *forCharset); // In percent.
	__int64 GetSumComputed(); // This might not be 100% accurate all the time, but its enough. We are here for the speed not for the accuracy.
	__int64 GetETA(CPlain *forPlain, CCharset *forCharset); // In seconds.
	int GetTime(); // Return the time this instance of stats was active.
};


#endif