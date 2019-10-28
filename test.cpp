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
	
	string dir = argv[1];

	cout << "The working directory is " << dir << endl;
	
	vector<string> files;
	int file_count = 0;
	for (const auto & entry : fs::directory_iterator(dir)) {
		file_count++;
		files.emplace_back(entry.path());
	}
	
	sort(files.begin(), files.end());
	for (auto file : files) {
		string name = file.substr(dir.length(), file.length());
		cout << name << endl;
	}
	cout << file_count << endl;

	return 0;
}
