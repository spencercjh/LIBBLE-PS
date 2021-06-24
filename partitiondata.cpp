/*********************
g++ **.c -o ** -std=c++11
./partitiondata [filename] [number]
attention: it will make a new file named "data"+"_"
**********************/

#include <sys/stat.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "argv error!\n";
        return -1;
    }

    std::string data_files = argv[1];
    int number = atoi(argv[2]);

    std::ifstream fin(data_files.c_str());
    if (!fin) {
        std::cerr << "file open error!\n";
        return -1;
    }
    // check and make directory
    std::string partition_directory = data_files + "_";
    struct stat buffer {};
    if (stat(partition_directory.c_str(), &buffer) != 0 &&
        mkdir(partition_directory.c_str(), 0777) == -1) {
        std::cerr << "Creating " << partition_directory << " failed" << std::endl;
        return -1;
    }
    std::ofstream fout[number];
    for (int i = 0; i < number; i++) {
        fout[i].open((partition_directory + "/part" + std::to_string(i + 1)).c_str());
        if (!fout[i]) {
            std::cerr << "file open error!\n";
            return -1;
        }
    }

    std::string buf;
    char str0[] = " ";
    int count = 0;
    while (getline(fin, buf)) {
        char temp[buf.size() + 1];
        buf.copy(temp, buf.size());
        temp[buf.size()] = '\0';
        strtok(temp, str0);//label
        char *result = NULL;
        result = strtok(NULL, str0);
        if (result == NULL) continue;
        fout[count % number] << buf << std::endl;
        count++;
    }
    std::cout << "number of data: " << count << std::endl;
    fin.close();
    for (int i = 0; i < number; i++) {
        fout[i].close();
    }

    return 0;
}
