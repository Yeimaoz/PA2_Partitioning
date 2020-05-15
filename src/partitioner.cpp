#include "partitioner.h"

Partitioner::Partitioner(){
    cout << "Creating a partitioner ..." << endl;
}

void Partitioner::read_file(string filename){
    cout << "Parsing " << filename << " ..." << endl;
    fstream infile(filename, ios::in);
    string buffer;
    string line;

    while (getline(infile, line)){
        if (!eon(line)){
            buffer += " " + line; 
            continue;
        }
        Net* net = parse_net(buffer != "" ? buffer + " " + line : line);
        this->nets[net->name] = net;
        buffer = "";
    }
}

bool Partitioner::eon(string line){
    smatch match;
    regex expr("}");
    return regex_search(line, match, expr);
}

Net* Partitioner::parse_net(string line){
    // net name extraction
    smatch match_net;
    regex expr_net("NET *([^ ]+) *\\{");
    regex_search(line, match_net, expr_net);
    string net_name = match_net[1];
    Net* net = new Net(net_name); 

    // blocks extraction
    smatch match_bks;
    regex expr_bks("\\{(.*)\\}");
    regex_search(line, match_bks, expr_bks);

    // split blocks by \w
    // directly add blocks to net, if it exists
    // if not, construct it, then add it to graph and net
    smatch match_bk;
    regex expr_bk("[^ ]*");
    istringstream iss(match_bks[1]);
    for(std::string block_name; iss >> block_name; ){
        Block* block;
        auto block_iter = this->blocks.find(block_name);
        if (block_iter != this->blocks.end())
            block = block_iter->second;
        else {
            block = new Block(block_name);
            this->blocks[block_name] = block;
        } 
        net->add_block(block);
    }
    return net;
}