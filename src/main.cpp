#include <iostream>
#include "partitioner.h"
using std::cout;
using std::endl;

int main(int argc, char* argv[]){
    cout << "Here you can get the input/output filenames" << endl;
    for (int i = 0; i < argc; ++i)
        cout << "argv[" << i << "]: " << argv[i] << endl;
    Partitioner partitioner;
    partitioner.read_file(argv[1]);
    partitioner.initialize();
    partitioner.FM_algorithm();
    partitioner.write_file(argv[2]);
    return 0;
}
