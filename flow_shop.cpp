#include <climits>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <string>
#include <utility>
#include <algorithm>
#include "flow_shop.h"

using namespace std;

// 樸素算完工時間的方法。
int FlowShop::calcMakespan() {
    makespanFR[0][0] = data[0][currentSequence[0]];
    for(size_t m = 1; m < numMachines; ++m) {
        makespanFR[m][0] = data[m][currentSequence[0]] + makespanFR[m-1][0];
    }
    for(size_t j = 1; j < numJobs; ++j) {
        makespanFR[0][j] =
            makespanFR[0][j-1] + data[0][currentSequence[j]];
        for(size_t m = 1; m < numMachines; ++m) {
            makespanFR[m][j] =
                (
                 (makespanFR[m][j-1] > makespanFR[m-1][j]) ?
                 makespanFR[m][j-1] : makespanFR[m-1][j]
                ) +
                data[m][currentSequence[j]];
        }
    }
    return makespanFR.back().back();

}// 樸素算完工時間的方法。
int FlowShop::calcMakespanCustom(vector<size_t> &sequence) {
    makespanFR[0][0] = data[0][sequence[0]];
    for(size_t m = 1; m < numMachines; ++m) {
        makespanFR[m][0] = data[m][sequence[0]] + makespanFR[m-1][0];
    }
    for(size_t j = 1; j < numJobs; ++j) {
        makespanFR[0][j] =
            makespanFR[0][j-1] + data[0][sequence[j]];
        for(size_t m = 1; m < numMachines; ++m) {
            makespanFR[m][j] =
                (
                 (makespanFR[m][j-1] > makespanFR[m-1][j]) ?
                 makespanFR[m][j-1] : makespanFR[m-1][j]
                ) +
                data[m][sequence[j]];
        }
    }
    return makespanFR.back().back();
}

// 快速算鄰域完工時間的方法。
int FlowShop::calcMakespanFastPrep() {
    makespanBR[numMachines-1][numJobs-1] =
        data[numMachines-1][currentSequence[numJobs-1]];
    for(size_t m = numMachines - 1; m > 0; --m) {
        makespanBR[m-1][numJobs-1] =
            makespanBR[m][numJobs-1] + data[m-1][currentSequence[numJobs-1]];
    }
    for(size_t j = numJobs - 1; j > 0; --j) {
        makespanBR[numMachines-1][j-1] =
            makespanBR[numMachines-1][j] + data[numMachines-1][currentSequence[j-1]];
        for(size_t m = numMachines - 1; m > 0; --m) {
            makespanBR[m-1][j-1] =
                (
                 (makespanBR[m-1][j] > makespanBR[m][j-1]) ?
                 makespanBR[m-1][j] : makespanBR[m][j-1]
                ) +
                data[m-1][currentSequence[j-1]];
        }
    }
    int newMakespan = calcMakespan();
    makespanFRPrime = makespanFR;
    return newMakespan;
}

// 快速算鄰域完工時間的方法：更新x值。
void FlowShop::calcMakespanFastUpdateX(size_t theX) {
    x = y = theX;
    makespanXValues[0] = data[0][currentSequence[x]];
    for(size_t m = 1; m < numMachines; ++m) {
        makespanXValues[m] =
            makespanXValues[m-1] + data[m][currentSequence[x]];
    }
    makespanFRPrime = makespanFR;
}

// 快速算鄰域完工時間的方法：曾加y值。
int FlowShop::calcMakespanFast(size_t yIncr = 1) {
    for(; yIncr > 0; --yIncr) {
        ++y;
        makespanFRPrime[0][y-1] =
            makespanFRPrime[0][y-2] + data[0][currentSequence[y]];
        for(size_t m = 1; m < numMachines; ++m) {
            makespanFRPrime[m][y-1] =
                (
                 (makespanFRPrime[m-1][y-1] > makespanFRPrime[m][y-2]) ?
                 makespanFRPrime[m-1][y-1] : makespanFRPrime[m][y-2]
                ) +
                data[m][currentSequence[y]];
        }
    }
    int makespan = -1;
    int temp, tempMax;
    size_t maxP, maxQ, tempMaxP;
    for(size_t q = 0; q < numMachines; ++q) {
        tempMax = -1;
        for(size_t p = 0; p <= q; ++p) {
            if(tempMax <
                (temp =
                (makespanFRPrime[p][y-1] +
                makespanXValues[q] -
                makespanXValues[p] +
                data[p][currentSequence[x]]))) {
                tempMax = temp;
                tempMaxP = p;
            }
        }
        if(makespan < (temp = tempMax + makespanBR[q][y+1])) {
            makespan = temp;
            maxQ = q;
            maxP = tempMaxP;
        }
    }
    return makespan;
}

// 工作序列插入方法。
void FlowShop::insert(int pos1, int pos2) {

        currentSequence = prevSequence;
        int temp = currentSequence[pos1];
        if(pos2>pos1)
        {
            for(int i = pos1; i < pos2; ++i)
            {
                currentSequence[i] = currentSequence[i+1];
            }
        }
        else
        {
            for(int i = pos1; i > pos2; --i)
            {
                currentSequence[i] = currentSequence[i-1];
            }
        }
        currentSequence[pos2] = temp;
}

// 工作序列隨機插入方法。
void FlowShop::randInsert() {
        // {1, 2, ..., (n-1)}
        size_t pos1 = rand() % (numJobs - 1);

        // {(pos1+1), (pos1+2), ..., n}.
        size_t pos2 = rand() % (numJobs - pos1 - 1) + pos1 + 1;

        // 插入動作。
        prevSequence = currentSequence;
        insert(pos1, pos2);
}

// Memetic algorithm.
void MA::generate() {
    vector<size_t> candidates = currentSequence;
    for(size_t i = 0; i < numJobs; ++i) {
        auto it = candidates.begin();
        it += rand() % candidates.size();
        currentSequence[i] = *it;
        candidates.erase(it);
    }
}

int MA::search() {
	int best = INT_MAX;
	// Randomly generate population.
	for(size_t i = 0; i < populationSize; ++i) {
		vector<size_t> candidates = currentSequence;
		population.push_back(currentSequence);
		for(size_t j = 0; j < numJobs; ++j) {
			auto it = candidates.begin();
			it += rand() % candidates.size();
			population[i][j] = *it;
			candidates.erase(it);
		}
	}
	for(size_t i = 0; i < 500; ++i) {
		vector<size_t> parentA;
		vector<size_t> parentB;
		vector<vector<size_t>> offspring;
		// Build up new generation.
		while(populationCounter < populationSize) {
			// Tournament selection.
			tournament(parentA);
			tournament(parentB);
			offspring.push_back(population[0]);
			offspring.push_back(population[0]);
			crossover(offspring[offspring.size() - 1], offspring[offspring.size() - 2], parentA, parentB);
			populationCounter += 2;
		}
		populationCounter = 0;
		for(size_t i = 0; i < offspring.size(); ++i) {
			currentSequence = offspring[i];
			bestMakespan = localSearch();
			//offspring[i] = currentSequence;
			if(bestMakespan < best) {
				best = bestMakespan;
			}
		}
		int currentMakespan;
		population = offspring;
		offspring.clear();
	}
	return best;
}

void MA::tournament(vector<size_t> &parent) {
	vector<size_t> tournamentIndices;
	tournamentIndices.resize(tournamentSize);
	int currentMakespan;
	for(size_t i = 0; i < tournamentSize; ++i) {
		tournamentIndices[i] = rand() % population.size();
	}
	size_t winner = 0;
	int winnerMakespan = calcMakespanCustom(population[tournamentIndices[0]]);
	for(size_t i = 1; i < tournamentSize; ++i) {
		currentMakespan = calcMakespanCustom(population[tournamentIndices[i]]);
		if(currentMakespan < winnerMakespan) {
			winnerMakespan = currentMakespan;
			winner = i;
		}
	}
	parent = population[tournamentIndices[winner]];
}

void MA::crossover(vector<size_t> &childA, vector<size_t> &childB, vector<size_t> &parentA, vector<size_t> &parentB) {
	size_t crossPoint = rand() % numJobs;
	for(size_t i = 0; i < crossPoint; ++i) {
		childA[i] = parentA[i];
		childB[i] = parentB[i];
	}
	vector<pair<size_t,size_t>> arrA,arrB;
	for(size_t i=crossPoint;i<numJobs;i++){
		arrA.push_back(make_pair(parentA[i], find(parentB.begin(),parentB.end(),parentA[i]) - parentB.begin()) );
		arrB.push_back(make_pair(parentB[i], find(parentA.begin(),parentA.end(),parentB[i]) - parentA.begin()) );
	}
	sort(arrA.begin(), arrA.end(), [&](pair<size_t, size_t> a, pair<size_t, size_t> b) { return get<1>(a) < get<1>(b); });
	sort(arrB.begin(), arrB.end(), [&](pair<size_t, size_t> a, pair<size_t, size_t> b) { return get<1>(a) < get<1>(b); });
	for(size_t i=crossPoint,j=0;i<numJobs;i++,j++){
		childA[i] = get<0>(arrA[j]);
		childB[i] = get<0>(arrB[j]);
	}

}

int MA::localSearch() {
    int now, best = INT_MAX;
    while((now = searchHelper()) < best) {
        best = now;
    }
    return best;
}

int MA::searchHelper() {
    bestMakespan = calcMakespanFastPrep();
    int tempMakespan;
    size_t bestI = 0, bestJ = 0;
    for(size_t i = 0; i < numJobs; ++i) {
        calcMakespanFastUpdateX(i);
        for(size_t j=i + 1; j < numJobs; ++j) {
            tempMakespan = calcMakespanFast();
            if(tempMakespan <= bestMakespan) {
                bestMakespan = tempMakespan;
                bestI = i;
                bestJ = j;
            }
        }
    }
    insert(bestI, bestJ);
    return bestMakespan;
}

istream & operator>>(istream &is, FlowShop &fs) {
    string line;
    char *pos;
    for(size_t i = 0; i < fs.numMachines; ++i) {
        getline(is, line);
	    fs.data.push_back(vector<int>());
        pos = (char *)line.c_str();
        for(size_t j = 0; j < fs.numJobs; ++j) {
            fs.data[i].push_back(strtol(pos, &pos, 10));
        }
    }
    fs.bestMakespan = fs.makespan = fs.calcMakespan();
    return is;
}

ostream & operator<<(ostream &os, const FlowShop &fs) {
    for(size_t i = 0; i < fs.numMachines; ++i) {
        for(size_t j = 0; j < fs.numJobs; ++j) {
            cout << setw(3) << fs.data[i][fs.currentSequence[j]] << ' ';
        }
        cout << endl;
    }
    return os;
}

