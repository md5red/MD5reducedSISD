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
#include <csignal>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include "MD5.h"
#include "Charset.h"
#include "Package.h"
#include "Stats.h"
#include "Plain.h"
#include "Bruter.h"

using namespace std;


void sigHandler(int sig)
{
	// Stop.
	CBruter::Abort();
}

int main(int argc, char *argv[])
{
	// Init signal handler.
	signal(SIGABRT, &sigHandler);
	signal(SIGINT, &sigHandler);
	signal(SIGTERM, &sigHandler);

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("help", "Procudes this help message")
		("version", "Print program message\n")
		("h", boost::program_options::value<string>(), "Hash to crack. A MD5 hash is exactly 32 hex chars long")
		("c", boost::program_options::value<string>(), "Sets the charset. Can be every combination of the following:\n  c: \tA-Z\n  s: \ta-z\n  d: \t0-9\n  x: \tprintable special characters\n")
		("min", boost::program_options::value<int>(), "Minimum password length. Must be over 4")
		("max", boost::program_options::value<int>(), "Maximum password length. Must be 11 or less");
	boost::program_options::variables_map vm;
	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
		boost::program_options::notify(vm); 
	}
	catch(boost::program_options::error) {
		// Print.
		cout << "[-] Error: Wrong paramaters. Use MD5reduced --help for more information" << endl;

		// Exit.
		return EXIT_FAILURE;
	}

	// Print the help screen.
	if (vm.count("help")) {
		// Print.
		cout << desc << endl;

		// Exit.
		return EXIT_SUCCESS;
	}

	// Print the help screen.
	if (vm.count("version")) {
		// Print.
		cout << "MD5reduced " << VERSION << endl;
		cout << "(c) Tobias Sielaff, 2011" << endl;

		// Exit.
		return EXIT_SUCCESS;
	}

	// All needed params?
	if (!vm.count("h") || !vm.count("c") || !vm.count("min") || !vm.count("max")) {
		// Print.
		cout << "[-] Error: Missing paramaters. Use MD5reduced --help for more information" << endl;

		// Exit.
		return EXIT_FAILURE;
	}

	// Init MD5.
	string hash = vm["h"].as<string>();
	if (!MD5Init(hash)) {
		// Print.
		cout << "[-] Error: Invaild hash specified. Use MD5reduced --help for more information" << endl;

		// Exit.
		return EXIT_FAILURE;
	}

	// Init charset.
	CCharset charset;
	if (!charset.Initialize(vm["c"].as<string>())) {
		// Print.
		cout << "[-] Error: Invaild charset specified. Use MD5reduced --help for more information" << endl;

		// Exit.
		return EXIT_FAILURE;
	}

	// Init package.
	CPackage package(&charset);

	// Init length.
	int min = vm["min"].as<int>();
	int max = vm["max"].as<int>();
	if (min < 4 || max > 11 || min > max) {
		// Print.
		cout << "[-] Error: Invaild length specified. Use MD5reduced --help for more information" << endl;

		// Exit.
		return EXIT_FAILURE;
	}
	
	// Print config.
	cout << "------------------------------------" << endl;
	cout << "Hash: " << hash << endl;
	cout << "Charset: " << charset.GetCharset() << endl;
	cout << "Min. length: " << min << endl;
	cout << "Max. length: " << max << endl;
	cout << "Threads: " << boost::thread::hardware_concurrency() << endl;
	cout << "Total combinations: " << charset.GetAllCombs(min, max) << endl;
	cout << "------------------------------------" << endl << endl;

	// Bench, start!
	CTimer timer;
	timer.Start();

	// Go!
	for (int i = min; i <= max; i++) {
		// Init stats, plain & bruter.
		CStats *stats = new CStats();
		CPlain *plain = new CPlain(i);
		CBruter *bruter = new CBruter(plain, &charset, &package, stats);

		// Go!
		bruter->Launch();
		bruter->Wait();

		// Cracked?
		if (bruter->HasResult()) {
			cout << "[+] Cracked! => " << bruter->GetResult() << "					" << endl;
			break;
		}

		// Cleanup.
		delete bruter;
		delete plain;
		delete stats;
	}
	cout << "[+] Finished (in " << str(boost::format("%.2f") % (timer.Elapsed() / 1000.0)) << " seconds)!					" << endl;

	// Cleanup.
	MD5Cleanup();

	// Done!
	return EXIT_SUCCESS;
}