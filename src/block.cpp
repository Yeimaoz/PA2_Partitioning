#include "block.h"

ostream &operator<<(ostream &output, const Block &in){
    Block buffer = in;
    output << "Block " << buffer.name << " { ";
    for (auto net : buffer.nets)
        cout << net->name << " ";
    output << "}";
	return output; 
}

Block::Block(){}

Block::Block(string name){
    this->name = name;
}

Block::Block(const Block& in){
    this->name = in.name;
    this->belongs2 = in.belongs2;
    this->nets = in.nets;
}

void Block::add_net(Net* net){
    this->nets.insert(net);
}