// Copyright Vladimir Prus 2002-2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

/* The simplest usage of the library.
 */

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <chrono>
#include <omp.h>
#include "src/heat.h"
using namespace std;

int main(int ac, const char* av[])
{
    try {

        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,h", "produce help message")
                ("size,s", po::value<uint>()->default_value(10), "set matrix size")
                ("left,l",po::value<double>()->default_value(30), "set left border temperature")
                ("right,r",po::value<double>()->default_value(20), "set right border temperature")
                ("top,t",po::value<double>()->default_value(10), "set top border temperature")
                ("bottom,b",po::value<double>()->default_value(10), "set bottom border temperature")
                ("accurancy,a",po::value<double>()->default_value(0.1), "set accurancy for temperature")
                ("density,d",po::value<uint>()->default_value(6), "set maximum density for vectors")
                ("k,k",po::value<double>()->default_value(1), "set k' constant for Heat Transfer")
                ;

        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return 0;
        }
        else{
            uint size = vm["size"].as<uint>();
            cout << "Matrix size was set to "
                 << size << "\n";
            //Set variables
            double left,right,top,bottom,k;
            unsigned int density;
            left = vm["left"].as<double>();
            right = vm["right"].as<double>();
            top = vm["top"].as<double>();
            bottom = vm["bottom"].as<double>();
            density = vm["density"].as<uint>();
            k = vm["k"].as<double>();
            //Solve for temperatures
            double acc = 0.1;
            if (vm.count("accurancy")) acc=vm["accurancy"].as<double>();
            double t1 = omp_get_wtime();
            double* temp = getTemperatureMatrix(size, left, right, top, bottom,acc);
            double t2 = omp_get_wtime();
            std::cout<< "Duration: "<<(t2-t1)*1000<<"ms" <<std::endl;
            double* vectores = getVectores(temp,size,density, left, right, top, bottom,k);
            free(vectores);
            free(temp);

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
