#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <mpi.h>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <pthread.h>
#include <atomic>
#include <unistd.h>
#include <ctime>
#include <sstream>  // for string streams 

using namespace std;
namespace fs = filesystem;

// The list of the input files
vector<string> file_list;

// The input data directory path
string input_dir;
// The output data directory path
string output_dir;

int total_jobs;
atomic_int current_jobID;
int MPI_size;
int MPI_rank;

void* job_distribute(void* _arg) {
	
    char recv_buf;
    int send_buf;
    MPI_Status status;

    send_buf = current_jobID++;
    while (send_buf < total_jobs) {
        MPI_Recv(&recv_buf, 1, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        MPI_Send(&send_buf, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
        send_buf = current_jobID++;
    }
    
    int finished_proc = 1;	// the finished processed
    send_buf = -1;
	while (finished_proc < MPI_size) {
		MPI_Recv(&recv_buf, 1, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        MPI_Send(&send_buf, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
        finished_proc++;
	}
    pthread_exit(NULL);
}

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
	// MPI init
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &MPI_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &MPI_rank);

    time_t start_time, end_time;

    input_dir = argv[1];
    output_dir = argv[2];

    //cout << "Rank "<< MPI_rank << "/" << MPI_size << endl;
    if (MPI_rank == 0) {
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
    if (MPI_rank == 0) {
        total_jobs = file_list.size();
        cout << "There are " << file_list.size() << " input files." << endl;
    }

    current_jobID = 0;
    MPI_Barrier(MPI_COMM_WORLD);

    /*
        TODO: Fix the bug that the program will crash when MPI_size >= number of input file.
    */
    if (MPI_rank == 0) {
        // create a thread to distribute jobs
        pthread_t thread;
	    pthread_create(&thread, 0, job_distribute, NULL);
        MPI_Barrier(MPI_COMM_WORLD);
        int job_id;
        int exec_status;

        job_id = current_jobID++;
        while (job_id < total_jobs) {
            //cout << "Rank " << MPI_rank << " get job " << job_id << endl;
            cout << "Rank " << setw(3) << MPI_rank << " get file " << file_list[job_id] << endl;
            string cmd = "./xvfb-run-safe /home/sc20/engine210/brc/Fiji.app/ImageJ-linux64 -macro /home/sc20/engine210/brc/code/macro/tsmacro.ijm " + input_dir + file_list[job_id] + "," + output_dir + file_list[job_id] + " >> log.txt";
            //cout << cmd << endl;
            exec_status = system(cmd.c_str());
            //cout << "Rank " << MPI_rank << " finished job " << job_id << " with return value " << exec_status << endl;
            cout << "Rank " << setw(3) << MPI_rank << " finished processing file " << file_list[job_id] << " with return value " << exec_status << endl;
            job_id = current_jobID++;
        }
        pthread_join(thread, NULL);
        cout << "Rank " << setw(3) << MPI_rank << " end processing" << endl;

        // merge data
        cout << "Rank " << setw(3) << MPI_rank << " start to merge data" << endl;
        merge();
        cout << "Finish merging" << endl;
        time(&end_time);
        int time_diff = end_time - start_time;
        printf("Processing time: %02d:%02d:%02d\n", time_diff / 3600, (time_diff%3600)/60, time_diff % 60);
    }
    else {
        int job_id;
        char send_buf;
        int exec_status;
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Send(&send_buf, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        MPI_Recv(&job_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        while (job_id != -1) {
            cout << "Rank " << setw(3) << MPI_rank << " get file " << file_list[job_id] << endl;
            string cmd = "./xvfb-run-safe /home/sc20/engine210/brc/Fiji.app/ImageJ-linux64 -macro /home/sc20/engine210/brc/code/macro/tsmacro.ijm " + input_dir + file_list[job_id] + "," + output_dir + file_list[job_id] + " >> log.txt";
            //cout << cmd << endl;
            exec_status = system(cmd.c_str());
            //cout << "Rank " << MPI_rank << " finished job " << job_id << " with return value " << exec_status << endl;
            cout << "Rank " << setw(3) << MPI_rank << " finished processing file " << file_list[job_id] << " with return value " << exec_status << endl;

            MPI_Send(&send_buf, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            MPI_Recv(&job_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        cout << "Rank " << setw(3) << MPI_rank << " end" << endl;
    }

    MPI_Finalize();

	return 0;
}
