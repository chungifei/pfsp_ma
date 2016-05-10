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
    // 隨機數種子。
    srand(time(NULL));

    // 計時器。
    typedef chrono::high_resolution_clock Clock;

    // 從第一行讀取測資規格。
    string line;
    getline(cin, line);
    char *pos = (char *)line.c_str();
    size_t numJobs = strtol(pos, &pos, 10);
    size_t numMachines = strtol(pos, nullptr, 10);
	MA ma(numJobs, numMachines, 100, 3);

    // 讀取測資。
	cin >> ma;

    // 迭代改善法、模擬退火法、禁忌搜尋法。
	
    int result;
    auto start = Clock::now();
    result = ma.search();
    auto end = Clock::now();
    chrono::duration<double, milli> duration = end - start;
    cout << "MA: " << result << "（" << duration.count() / 1000.0 << "秒）\n";
    return 0;
}
