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
#include <boost/format.hpp>
#include "Stats.h"

using namespace std;


CStats::CStats()
{
	// Sum computed per thread.
	m_iSumComputed = new __int64[boost::thread::hardware_concurrency()];

	// A few timers.
	m_tLastPrint = new CTimer();
	m_tOverall = new CTimer();

	// Zero sum computed.
	for (unsigned int i = 0; i < boost::thread::hardware_concurrency(); i++)
		m_iSumComputed[i] = 0;

	// Start our timers.
	m_tLastPrint->Start();
	m_tOverall->Start();
}

CStats::~CStats()
{
	// Cleanup.
	delete[] m_iSumComputed;

	// Cleanup the rest.
	delete m_tLastPrint;
	delete m_tOverall;
}

void CStats::Update(CPlain *forPlain, CCharset *forCharset, const int threadIdx)
{
	// Update, assuming that this is called once x^3 combs are solved.
	m_iSumComputed[threadIdx] += forCharset->GetCombs(2);

	// Do we need to print to console?
	m_mPrint.lock(); // cout is not thread-safe, so do this only once.
	if (m_tLastPrint->Elapsed() > UPDATE_EVERY) {
		// Print.
		cout << "[+] Bruting... " << str(boost::format("%.2f") % GetProgress(forPlain, forCharset)) << "% (" << str(boost::format("%.2f") % GetSpeed()) << " MHash/sec.)	\r";
				
		// Reset.
		m_tLastPrint->Reset();
	}
	m_mPrint.unlock();
}

double CStats::GetSpeed()
{
	// Calc.
	double hashPerMs = static_cast<long double>(GetSumComputed()) / GetTime();
	double hashPerS = hashPerMs * 1000;
	double MHashPerS = hashPerS / 1000000;

	// Return.
	return MHashPerS;
}

double CStats::GetProgress(CPlain *forPlain, CCharset *forCharset)
{
	// Compute.
	return (GetSumComputed() / static_cast<long double>(forCharset->GetCombs(forPlain->GetLen())) * 100);
}

__int64 CStats::GetSumComputed()
{
	// Enumerate.
	__int64 totalComputed = 0;
	for (unsigned int i = 0; i < boost::thread::hardware_concurrency(); i++)
		totalComputed += m_iSumComputed[i];

	// Return.
	return totalComputed;
}

__int64 CStats::GetETA(CPlain *forPlain, CCharset *forCharset)
{
	// Calc the remaining combinations.
	__int64 remaining = forCharset->GetCombs(forPlain->GetLen()) - GetSumComputed();

	// Return.
	return remaining / (__int64)GetSpeed() / 1000000;
}

int CStats::GetTime()
{
	return m_tOverall->Elapsed();
}