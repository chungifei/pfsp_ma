#ifndef FLOW_SHOP_H
#define FLOW_SHOP_H
#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>

const double EULER = 2.7182818284;

// 流程式生產排程。
class FlowShop {
    friend std::istream & operator>>(std::istream &is, FlowShop &fs);
    friend std::ostream & operator<<(std::ostream &os, const FlowShop &fs);

    public:
    FlowShop(size_t theNumJobs, size_t theNumMachines) :
        numComparisons(0),
        numJobs(theNumJobs),
        numMachines(theNumMachines),
        x(0),
        y(0) {
        if(numJobs < 2 || numMachines < 2) {
            throw std::invalid_argument("至少要兩個工作與兩臺機器。");
        }
        for(size_t i = 0; i < numJobs; ++i) {
            currentSequence.push_back(i);
        }
        prevSequence = currentSequence;
        makespanBR.resize(numMachines);
        makespanFR.resize(numMachines);
        makespanFRPrime.resize(numMachines);
        makespanXValues.resize(numMachines);
        for(size_t i = 0; i < numMachines; ++i) {
            makespanBR[i].resize(numJobs);
            makespanFR[i].resize(numJobs);
            makespanFRPrime[i].resize(numJobs);
        }
    }

    // 樸素算完工時間的方法。
    int calcMakespan();
    int calcMakespanCustom(std::vector<size_t> &sequence);

    // 快速算鄰域完工時間的方法。
    int calcMakespanFastPrep(); // 工作序列改了之後要呼叫這個方法。
    void calcMakespanFastUpdateX(size_t theX);
    int calcMakespanFast(size_t yIncr /*= 1*/);

    // 回傳分析過幾個工作序列。
    size_t getNumComparisons() { return numComparisons; }

    // 工作序列插入方法。
    void insert(int pos1, int pos2);

    // 工作序列隨機插入方法。
    void randInsert();

    // 所有衍生類別必需實作搜索方法。
    virtual int search() = 0;

    // 資料取值存值方法。
    const std::vector<std::vector<int>> & getData() { return data; }
    void setData(const std::vector<std::vector<int>> &theData) {
        data = theData;
        bestMakespan = makespan = calcMakespan();
    }

    protected:
    int bestMakespan;
    int makespan;
    size_t numComparisons;
    size_t numJobs;
    size_t numMachines;
    size_t x;
    size_t y;
    std::vector<std::vector<int>> data; // 使用方法：data[machine][job]。保持原始排序。
    std::vector<size_t> currentSequence; // 目前工作序列。
    std::vector<size_t> prevSequence; // 上一個工作序列。

    // 快遠算完工時間方法用的變數。
    std::vector<std::vector<int>> makespanBR;
    std::vector<std::vector<int>> makespanFR;
    std::vector<std::vector<int>> makespanFRPrime;
    std::vector<int> makespanXValues;
};

// Memetic algorithm.
class MA : public FlowShop {
    public:
    MA(size_t theNumJobs, size_t theNumMachines, size_t thePopulationSize, size_t theTournamentSize) :
		populationCounter(0),
		populationSize(thePopulationSize),
		tournamentSize(theTournamentSize),
        FlowShop(theNumJobs, theNumMachines) {}

	int search();
    int localSearch();

    private:
	void crossover(std::vector<size_t> &childA, std::vector<size_t> &childB, std::vector<size_t> &parentA, std::vector<size_t> &parentB);
	void tournament(std::vector<size_t> &parent);
    void generate(); // 産生隨機的起始點。
    int searchHelper();

	size_t populationCounter;
    size_t populationSize;
	size_t tournamentSize;
	std::vector<std::vector<size_t>> population;
};

std::istream & operator>>(std::istream &is, FlowShop &fs);
std::ostream & operator<<(std::ostream &os, const FlowShop &fs);

#endif
