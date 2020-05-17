#include <iostream>
#include <map>
#include <string>
#include <getopt.h>
#include "partitioner.h"
using namespace std;

map<string, string> get_options(int argc, char* argv[]){
    map<string, string> options;
    int c;
    while ((c = getopt(argc, argv, "m:i:o:g:d:")) != -1) {
        switch (c) {
            case 'm': 
                options["mode"] = optarg; 
                break;
            case 'i': 
                options["input"] = optarg; 
                break;
            case 'o':
                options["output"] = optarg; 
                break;   
            case 'd':
                options["dut"] = optarg; 
                break;   
            case 'g':
                options["generate"] = optarg; 
                break;   
        }
    }
    return options;
}

void generate_solution(string& input, string& output){
    Partitioner partitioner;
    partitioner.read_file(input);
    partitioner.initialize();
    partitioner.FM_algorithm();
    partitioner.write_file(output);
}

int main(int argc, char* argv[]){
    map<string, string> options = get_options(argc, argv);
    auto mode = options.find("mode");
    if (mode == options.end()){
        cout << "Option \"-m\" is required ..." << endl;
        cout << " -> valid values: RUN, TEST, GEN ..." << endl;
        cout << " -> for instance: ./PA2.CHECKER -m TEST -i cases/case0 -d results/case0.out ..." << endl;
        return -1;
    } else {
        if (mode->second == "RUN"){
            auto input_it = options.find("input"), output_it = options.find("output");
            if (input_it == options.end() | output_it == options.end()){
                cout << "Option \"-i\" & \"-o\" are required ..." << endl;
                cout << " -> valid values: the related paths of the input and output files ..." << endl;
                cout << " -> for instance: ./PA2.CHECKER -m RUN -i cases/case0 -o results/case0.out ..." << endl;
                return -1;
            }
            generate_solution(input_it->second, output_it->second);
        }
    }
    return 0;
}
