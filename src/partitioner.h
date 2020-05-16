#ifndef PARTITIONER_H
#define PARTITIONER_H
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <map>
#include <set>
#include <ctime>
#include "net.h"
#include "block.h"
using namespace std;

class Partitioner{
    public:
        Partitioner();
        // file io
        void read_file(string);
        void partition();

    private:
        int group_size = 2;

        map<string, Block*> blocks;
        map<string, Net*> nets;
        set<Block*> cuts[2];

        // read file
        bool eon(string);
        Net* parse_net(string);

        // partition
        void construct_initial_solution();

        // information
        void print();
        void print_results();
};
#endif
