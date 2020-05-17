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
#include <algorithm>
#include "net.h"
#include "block.h"
using namespace std;

class Partitioner{
    public:
        Partitioner();
        // file io
        void read_file(string);
        void write_file(string);

        // partition
        void initialize();
        void FM_algorithm();

        // information
        void print();
        void print_blocks();
        void print_nets();
        void print_gain_bucket();
        void print_results();

    private:
        int group_size = 2;
        int cut_size = 1e9;
        int p_value;

        map<string, Block*> blocks;
        map<string, Net*> nets;
        vector<set<Block*>> gain_bucket;
        set<Block*> cuts[2];

        // read file
        bool eon(string);
        Net* parse_net(string);

        // initialization
        void construct_initial_solution();
        void construct_gain_bucket();
        void find_fanouts();
        void evaluate_cut_size();
        void evaluate_block_cost();
        void evaluate_p_value();

        // FM algorithm
        Block* get_candidate();
        vector<vector<Block*>> get_distribution(vector<Block*>&);
        void move(Block*);
        void update_gain_bucket(set<Block*>&);
};
#endif
