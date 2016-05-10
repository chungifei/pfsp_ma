#include <chrono>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include "flow_shop.h"

using namespace std;

int main() {
    // �H���ƺؤl�C
    srand(time(NULL));

    // �p�ɾ��C
    typedef chrono::high_resolution_clock Clock;

    // �q�Ĥ@��Ū������W��C
    string line;
    getline(cin, line);
    char *pos = (char *)line.c_str();
    size_t numJobs = strtol(pos, &pos, 10);
    size_t numMachines = strtol(pos, nullptr, 10);
	MA ma(numJobs, numMachines, 100, 3);

    // Ū������C
	cin >> ma;

    // ���N�ﵽ�k�B�����h���k�B�T�ҷj�M�k�C
	
    int result;
    auto start = Clock::now();
    result = ma.search();
    auto end = Clock::now();
    chrono::duration<double, milli> duration = end - start;
    cout << "MA: " << result << "�]" << duration.count() / 1000.0 << "��^\n";
    return 0;
}
