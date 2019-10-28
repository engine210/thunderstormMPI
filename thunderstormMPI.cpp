#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <mpi.h>
#include <filesystem>
#include <vector>
#include <algorithm>

using namespace std;
namespace fs = filesystem;

int main(int argc, char* argv[]) {
	int size, rank;
	// MPI init
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // The input data directory path
    string input_dir = argv[1];
    // The output data directory path
    string output_dir = argv[2];

    cout << "Rank "<< rank << "/" << size << endl;
    if (rank == 0) {
        cout << "Input data directory: " << input_dir << endl;
        cout << "Output data directory: " << output_dir << endl;
    }
    
    // The list of the input files
    vector<string> file_list;
    // reading input file
    for (const auto & entry : fs::directory_iterator(input_dir)) {
        string file_path = entry.path();
        string file_name = file_path.substr(input_dir.length(), file_path.length());
        file_name.pop_back();
        file_name.pop_back();
        file_name.pop_back();
        file_name.pop_back();
        
		file_list.emplace_back(file_name);
	}
    // Sort the input file according to the file name
    sort(file_list.begin(), file_list.end());
    if (rank == 0) {
        cout << "There are " << file_list.size() << " input files." << endl;
    }

    int* status = new int[file_list.size()];
    for (int i = rank; i < file_list.size(); i += size) {
        string cmd = "./xvfb-run-safe /home/sc20/engine210/brc/Fiji.app/ImageJ-linux64 -macro /home/sc20/engine210/brc/code/macro/tsmacro.ijm " + input_dir + file_list[i] + "," + output_dir + file_list[i];
        cout << cmd << endl;
        status[i] = system(cmd.c_str());
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = rank; i < file_list.size(); i += size) {
        cout << "rank: " << rank << ", file number: " << i <<  ", system call status: "<< status[i] << endl;
    }

    MPI_Finalize();

	return 0;
}
