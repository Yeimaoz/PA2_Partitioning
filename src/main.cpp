#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>    
#include <vector>       
#include <random>       
#include <chrono>  
#include <ctime>     
#include <getopt.h>
#include "partitioner.h"
using namespace std;

map<string, string> get_options(int argc, char* argv[]){
    map<string, string> options;
    int c;
    while ((c = getopt(argc, argv, "m:i:o:d:n:e:c:")) != -1) {
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
            case 'n':
                options["#node"] = optarg; 
                break;   
            case 'e':
                options["#edge"] = optarg; 
                break;
            case 'c':
                options["constraint"] = optarg; 
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

void generate_graph(string& output, int nodes, int edges, int constraints){  
    srand(time(NULL));
    cout << output << " " << nodes << " " << edges << " " << constraints << endl;
    vector<int> nodeset(nodes);
    for(int i = 0; i < nodes; ++i)
        nodeset[i] = i;
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine e(seed);
    fstream outfile(output, ios::out);
    for(int i = 0; i < edges; ++i){
        outfile << "Net n" << i+1 << " { ";
        int connect2 = rand() % (constraints-1) + 2;
        shuffle(nodeset.begin(), nodeset.end(), e);
        for(int j = 0; j < connect2; ++j){
            outfile << "c" << nodeset[j] + 1 << " ";
        }
        outfile << "}" << endl;
    }
    outfile.close();
}

int main(int argc, char* argv[]){
    map<string, string> options = get_options(argc, argv);
    auto mode = options.find("mode");
    if (mode == options.end()){
        cout << "Option \"-m\" is required ..." << endl;
        cout << " -> \"-m\": should be RUN, TEST or GEN ..." << endl;
        cout << " -> GEN: randomly generate a testcase, \"./PA2.CHECKER -m GEN\" to learn more details ..." << endl;
        cout << " -> RUN: generate a sample solution, \"./PA2.CHECKER -m RUN\" to learn more details ..." << endl;
        cout << " -> TEST: check the correctness of the solution you want to test, \"./PA2.CHECKER -m TEST\" to learn more details ..." << endl;
        cout << " -> For instance: \"./PA2.CHECKER -m TEST -i cases/case0 -d results/case0.out\" to test the correctness of results/case0.out ..." << endl;
        return -1;
    } else {
        if (mode->second == "RUN"){
            cout << "Descripttion: generate a sample solution of given testcase ..." << endl;
            auto input_it = options.find("input"), output_it = options.find("output");
            if (input_it == options.end() | output_it == options.end()){
                cout << " -> Option \"-i\" & \"-o\" are required for RUN mode ..." << endl;
                if (input_it == options.end())
                    cout << " --> \"-i\": the related path of the input file ..." << endl;
                if (output_it == options.end())
                    cout << " --> \"-o\": the related path of the output file ..." << endl;
                cout << " --> For instance: \"./PA2.CHECKER -m RUN -i cases/case0 -o results/case0.out\" ..." << endl;
                cout << " ---> In this case, this program will generate a sample solution of partitioned cases/case0 ..." << endl;
                return -1;
            }
            generate_solution(input_it->second, output_it->second);
        } 
        else if (mode->second == "GEN"){
            cout << "Descripttion: randomly generate a testcase restricted by given number of nodes, number of hyperedges and" << endl;
            cout << " the maximum number of nodes any hyperedge connects to ..." << endl;
            auto output_it = options.find("output");
            auto node_it = options.find("#node");
            auto edge_it = options.find("#edge");
            auto constraint_it = options.find("constraint");
            if (output_it == options.end() | node_it == options.end() | edge_it == options.end() | constraint_it == options.end()){
                cout << " -> Option \"-o\", \"-n\", \"-e\" and \"-c\" are required for GEN mode ..." << endl;
                if (output_it == options.end())
                    cout << " --> \"-o\": the related path of the output file you want to place ..." << endl;
                if (node_it == options.end())
                    cout << " --> \"-n\": the number of nodes you'd like have in this graph ..." << endl;
                if (edge_it == options.end())
                    cout << " --> \"-e\": the number of hyperedges you'd like have in this graph ..." << endl;
                if (constraint_it == options.end())
                    cout << " --> \"-c\": the maximum number of nodes any hyperedge connects  to you'd like restrict in this graph ..." << endl;
                cout << " --> For instance: \"./PA2.CHECKER -m GEN -o cases/case10 -n 10 -e 8 -c 3\" ..." << endl;
                cout << " ---> In this case, this program will randomly generate a graph, cases/case10. And there are 10 nodes, 8 hyperedges" << endl;
                cout << " ---> and the maximum number of nodes each hyperedge connects to is less and equal to 3 ..." << endl;
                return -1;
            }
            generate_graph(output_it->second, stoi(node_it->second), stoi(edge_it->second), stoi(constraint_it->second));
        }
        else if (mode->second == "TEST"){
            cout << "Descripttion: check the given partitioned result is valid for the given testcase ..." << endl;
            auto output_it = options.find("output");
            auto node_it = options.find("#node");
        }
    }
    return 0;
}
