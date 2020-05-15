#include "net.h"

ostream &operator<<( ostream &output , const Net& in ){

	Net buffer = in;
    output << "Net " << buffer.name << " { ";
    for (auto block : buffer.blocks)
        cout << block->name << " ";
    output << "}";
	return output; 
}

Net::Net(){}

Net::Net(string name){
    this->name = name;
}

Net::Net(const Net& in){
    this->name = in.name;
    this->blocks = in .blocks;
}

void Net::add_block(Block* block){
    this->blocks.insert(block);
}
