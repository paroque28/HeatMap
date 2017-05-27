// Copyright Vladimir Prus 2002-2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

/* The simplest usage of the library.
 */

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include "src/heat.h"
using namespace std;

int main(int ac, const char* av[])
{
    try {

        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,h", "produce help message")
                ("size,s", po::value<uint>(), "set matrix size")
                ("left,l",po::value<double>(), "set left border temperature")
                ("right,r",po::value<double>(), "set right border temperature")
                ("top,t",po::value<double>(), "set top border temperature")
                ("bottom,b",po::value<double>(), "set bottom border temperature")
                ("accurancy,a",po::value<double>(), "set accurancy for temperature (default 0.1)")
                ;

        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return 0;
        }

        if (vm.count("size")) {
            uint size = vm["size"].as<uint>();
            cout << "Matrix size was set to "
                 << size << "\n";
            if(!(vm.count("top") && vm.count("bottom") && vm.count("left") && vm.count("right")))
            {
                cout << "Border temperatures were not set.\n";
                return 1;
            }
            //Set variables
            double left,right,top,bottom;
            left = vm["left"].as<double>();
            right = vm["right"].as<double>();
            top = vm["top"].as<double>();
            bottom = vm["bottom"].as<double>();
            //Solve for temperatures
            double acc = 0.1;
            if (vm.count("accurancy")) acc=vm["accurancy"].as<double>();
            double* temp = getTemperatureMatrix(size, left, right, top, bottom,acc);
            //double*** vectores = getVectores(temp);
        } else {
            cout << "Matrix size was not set.\n";
            return 1;
        }
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }

    return 0;
}
