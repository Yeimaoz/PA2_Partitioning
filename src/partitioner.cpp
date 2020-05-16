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
    find_fanouts();
    evaluate_p_value();
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
        block->add_net(net);
    }
    return net;
}

void Partitioner::initialize(){
    construct_initial_solution();
    evaluate_cut_size();
    evaluate_block_cost();
    construct_gain_bucket();
}

void Partitioner::construct_initial_solution(){
    srand(time(NULL));
    for (auto& b : this->blocks){
        // b.second->belongs2 = (b.first[1]-48) % 2;
        b.second->belongs2 = rand() % 2;
        this->cuts[b.second->belongs2].insert(b.second);
    }
}

void Partitioner::find_fanouts(){
    for(auto& block : this->blocks){
        for(auto net : block.second->nets){
            vector<Block*> fanouts;
            for(auto bk : net->blocks){
                if (bk != block.second)
                    fanouts.push_back(bk);
            }
            block.second->fanouts.push_back(fanouts);
        }
    } 
}

void Partitioner::FM_algorithm(){
    Block* candidate = get_candidate();
    if (candidate){
        move(candidate);
    }
}

void Partitioner::evaluate_p_value(){
    int p_value = -1;
    for(auto& block : this->blocks)
        p_value = (int(block.second->nets.size()) > p_value ? block.second->nets.size() : p_value);
    this->p_value = p_value;
}

void Partitioner::construct_gain_bucket(){
    this->gain_bucket = vector<set<Block*>>(this->p_value*2+1, set<Block*>());
    for(auto& block : this->blocks){
        if (!block.second->moved)
            this->gain_bucket[block.second->cost+this->p_value].insert(block.second);
    }
}

void Partitioner::evaluate_cut_size(){
    int cut_size = 0;
    for(auto& net : this->nets){
        int belongs[2] = {0, 0};
        for(auto block : net.second->blocks){
            belongs[block->belongs2] = 1;
            if (belongs[0] && belongs[1]){
                cut_size += 1;
                break;
            }
        }
    }
    this->cut_size = cut_size;
}

void Partitioner::evaluate_block_cost(){
    for(auto& block : this->blocks)
        block.second->update();
}

Block* Partitioner::get_candidate(){
    for(int i = this->gain_bucket.size()-1; i > this->p_value; --i)
        for(auto block : this->gain_bucket[i])
            return block;
    return NULL;
}

vector<vector<Block*>> Partitioner::get_distribution(vector<Block*>& blocks){
    vector<vector<Block*>> distribution(2, vector<Block*>());
    for(auto block : blocks)
        distribution[block->belongs2].push_back(block);
    return distribution;
}

void Partitioner::move(Block* candidate){
    //print_blocks();
    print_gain_bucket();
    cout << this->cut_size << endl;
    candidate->moved = true;
    cout << *candidate << " " << candidate->moved <<endl;
    int move2 = !candidate->belongs2;
    for(int i = 0; i < candidate->nets.size(); ++i){
        auto distribution = get_distribution(candidate->fanouts[i]);
        // cout << *candidate->nets[i] << endl;
        // cout << distribution[0].size() << " " << distribution[1].size() << endl;
        // line 6
        if (distribution[move2].empty()){
            for(auto block : candidate->nets[i]->blocks)
                if (!block->moved)
                    block->cost++;
        } else if (distribution[move2].size() == 1){
            for(auto block : distribution[move2])
                if (!block->moved)
                    block->cost--;   
        }
        // line 8
        if (distribution[candidate->belongs2].empty()){
            for(auto block : candidate->nets[i]->blocks)
                if (!block->moved)
                    block->cost--;   
        } else if (distribution[candidate->belongs2].size() == 1){
            for(auto block : distribution[candidate->belongs2])
                if (!block->moved)
                    block->cost++;   
        }
    }
    construct_gain_bucket();
    candidate->belongs2 = move2;
    evaluate_cut_size();
    cout << this->cut_size << endl;
    print_gain_bucket();
    // print_blocks();
}

void Partitioner::print_blocks(){
    cout << "#Block: " << this->blocks.size() << endl;
    for (auto block : this->blocks)
        cout << " -> " << *(block.second) << " ( " << block.second->cost << " )"<< endl;
}

void Partitioner::print_nets(){
    cout << "#Net: " << this->nets.size() << endl;
    for (auto nets : this->nets)
        cout << " -> " << *(nets.second) << endl;
    cout << endl;
}

void Partitioner::print(){
    print_blocks();
    print_nets();
}

void Partitioner::print_gain_bucket(){
    cout << "#Bucket: p = " << this->p_value << endl;
    for(int i = this->gain_bucket.size()-1; i >= 0 ; --i){
        cout << " -> Bucket[" << i-this->p_value << "]: ";
        for(auto block : this->gain_bucket[i])
            cout << block->name << " ";
        cout << endl;
    }
}

void Partitioner::print_results(){
    cout << "cut_size " << this->cut_size << endl;
    for (int i = 0; i < this->group_size; ++i){
        cout << "#" << char(i+65)<< ": " << this->cuts[i].size() << endl;
        cout << " -> { ";
        for (auto& b : this->cuts[i])
            cout << b->name << " ";
        cout << "}" << endl;
    }
}