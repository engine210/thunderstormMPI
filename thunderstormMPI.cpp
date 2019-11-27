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

// The list of the input files
vector<string> file_list;
// The input data directory path
string input_dir;
// The output data directory path
string output_dir;

void merge() {
    string file_name = file_list[0];
    string cmd = "awk -F\",\" 'NR==1{printf \"\\\"time\\\",%s,%s,\\\"z\\\",%s,%s,%s,%s,%s\\n\",$2,$3,$4,$5,$6,$7,$8} NR>1{printf \"1,%s,%s,%s,%s,%s,%s,%s,%s\\n\",$2,$3,$1,$4,$5,$6,$7,$8}' " + output_dir + file_name + ".csv > " + output_dir + "output.csv";
    int exec_status = system(cmd.c_str());
    for (int i = 1; i < file_list.size(); i++) {
        file_name = file_list[i];
        ostringstream time; 
        time << i+1; 
        string cmd = "awk -F\",\" 'NR>1{printf \"" + time.str() + ",%s,%s,%s,%s,%s,%s,%s,%s\\n\",$2,$3,$1,$4,$5,$6,$7,$8}' " + output_dir + file_name + ".csv >> " + output_dir + "output.csv";
        int exec_status = system(cmd.c_str());
    }
}

int main(int argc, char* argv[]) {
	int size, rank;
	// MPI init
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    time_t start_time, end_time;

    // The input data directory path
    input_dir = argv[1];
    // The output data directory path
    output_dir = argv[2];

    //cout << "Rank "<< rank << "/" << size << " start"<< endl;
    if (rank == 0) {
        time(&start_time);
        cout << "Input data directory: " << input_dir << endl;
        cout << "Output data directory: " << output_dir << endl;
    }
    
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

    MPI_Barrier(MPI_COMM_WORLD);

    int* status = new int[file_list.size()];
    for (int i = rank; i < file_list.size(); i += size) {
        string cmd = "./xvfb-run-safe /home/sc20/engine210/brc/Fiji.app/ImageJ-linux64 -macro /home/sc20/engine210/brc/code/macro/tsmacro.ijm " + input_dir + file_list[i] + "," + output_dir + file_list[i] + " > log.txt";
        //cout << cmd << endl;
        cout << "rank " << setw(3) << rank << " get file " << file_list[i] << endl;
        status[i] = system(cmd.c_str());
        cout << "rank " << setw(3) << rank << " finished processing file " << file_list[i] <<  " with system return value "<< status[i] << endl;
        //cout << "rank " << rank << " finish processing file " << file_list[i] << endl;
    }

    /*
    for (int i = rank; i < file_list.size(); i += size) {
        cout << "rank: " << rank << ", file number: " << i <<  ", system call status: "<< status[i] << endl;
    }
    */

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        // merge data
        cout << "Rank " << rank << " start to merge data" << endl;
        merge();
        cout << "Finish merging" << endl;
        time(&end_time);
        int time_diff = end_time - start_time;
        printf("Processing time: %02d:%02d:%02d\n", time_diff / 3600, (time_diff%3600)/60, time_diff % 60);
    }

    MPI_Finalize();

	return 0;
}
