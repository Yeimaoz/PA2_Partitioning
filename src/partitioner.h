#ifndef PARTITIONER_H
#define PARTITIONER_H
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <map>
#include "net.h"
#include "block.h"
using namespace std;

class Partitioner{
    public:
        Partitioner();
        // file io
        void read_file(string);

    private:
        map<string, Block*> blocks;
        map<string, Net*> nets;

        // read file
        bool eon(string);
        Net* parse_net(string);
};
#endif
