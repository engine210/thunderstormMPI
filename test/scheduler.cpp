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

using namespace std;

int total_jobs = 10;
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


int main(int argc, char* argv[]) {
	// MPI init
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &MPI_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &MPI_rank);

    srand(MPI_rank);

    if (MPI_rank == 0) {
        // create a thread to distribute jobs
        pthread_t thread;
	    pthread_create(&thread, 0, job_distribute, NULL);

        int job_id;
        while (current_jobID < total_jobs) {
            job_id = current_jobID++;
            cout << "Rank " << MPI_rank << " get job " << job_id << endl;
            int delay = rand() % 6 + 1;
            cout << "Rank " << MPI_rank << " delay: " << delay << endl;
            sleep(delay);
            cout << "Rank " << MPI_rank << " finished job " << job_id << endl;
        }
        pthread_join(thread, NULL);
        cout << "Rank " << MPI_rank << " end" << endl;
    }
    else {
        int job_id;
        char send_buf;
        MPI_Send(&send_buf, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        MPI_Recv(&job_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        while (job_id != -1) {
            cout << "Rank " << MPI_rank << " get job " << job_id << endl;
            int delay = rand() % 6 + 1;
            cout << "Rank " << MPI_rank << " delay: " << delay << endl;
            sleep(delay);
            cout << "Rank " << MPI_rank << " finished job " << job_id << endl;

            MPI_Send(&send_buf, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            MPI_Recv(&job_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        cout << "Rank " << MPI_rank << " end" << endl;
    }

    MPI_Finalize();

	return 0;
}
