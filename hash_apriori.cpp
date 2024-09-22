#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <sstream>
#include <algorithm>

using namespace std;

typedef set<int> Itemset;


int hashItemset(const Itemset& itemset) {
    int sum = 0;
    for (int item : itemset) {
        sum += item;
    }
    return sum % 40; // 40 buckets
}


vector<Itemset> generateHashBasedCandidates(const vector<Itemset>& transactions, map<int, int>& hashBuckets, int minSupport) {
    map<pair<int, int>, int> candidateCounts;

    for (const auto& transaction : transactions) {
        vector<int> items(transaction.begin(), transaction.end());
        int n = items.size();

        
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                Itemset candidate = { items[i], items[j] };
                int hashValue = hashItemset(candidate);
                hashBuckets[hashValue]++;

                candidateCounts[{items[i], items[j]}]++;
            }
        }
    }

    
    vector<Itemset> frequentItemsets;
    for (const auto& candidate : candidateCounts) {
        Itemset itemset = { candidate.first.first, candidate.first.second };
        int hashValue = hashItemset(itemset);

        // count exceeds the minimum support
        if (hashBuckets[hashValue] >= minSupport) {
            frequentItemsets.push_back(itemset);
        }
    }

    return frequentItemsets;
}


map<Itemset, int> countSupport(const vector<Itemset>& transactions, const vector<Itemset>& candidates) {
    map<Itemset, int> itemsetCounts;
    for (const auto& transaction : transactions) {
        for (const auto& candidate : candidates) {
            if (includes(transaction.begin(), transaction.end(), candidate.begin(), candidate.end())) {
                itemsetCounts[candidate]++;
            }
        }
    }
    return itemsetCounts;
}


vector<Itemset> filterBySupport(const map<Itemset, int>& candidateCounts, int minSupport) {
    vector<Itemset> frequentItemsets;
    for (const auto& candidateCount : candidateCounts) {
        if (candidateCount.second >= minSupport) {
            frequentItemsets.push_back(candidateCount.first);
        }
    }
    return frequentItemsets;
}

// Hash-based Apriori algorithm
void hashBasedApriori(const vector<Itemset>& transactions, int globalMinSupport, const string& outputFilename) {
    ofstream outputFile(outputFilename);

    map<int, int> itemCounts;
    for (const auto& transaction : transactions) {
        for (int item : transaction) {
            itemCounts[item]++;
        }
    }

    vector<Itemset> frequentItemsets;
    for (const auto& itemCount : itemCounts) {
        if (itemCount.second >= globalMinSupport) {
            frequentItemsets.push_back({ itemCount.first });
        }
    }

    outputFile << "Frequent 1-itemsets:\n";
    for (const auto& itemset : frequentItemsets) {
        for (int item : itemset) {
            outputFile << item << " ";
        }
        outputFile << endl;
    }

  
    map<int, int> hashBuckets;  
    vector<Itemset> candidates = generateHashBasedCandidates(transactions, hashBuckets, globalMinSupport);

    
    map<Itemset, int> candidateCounts = countSupport(transactions, candidates);
    frequentItemsets = filterBySupport(candidateCounts, globalMinSupport);

    if (!frequentItemsets.empty()) {
        outputFile << "\nFrequent 2-itemsets:\n";
        for (const auto& itemset : frequentItemsets) {
            for (int item : itemset) {
                outputFile << item << " ";
            }
            outputFile << endl;
        }
    }

    outputFile.close();
    cout << "Results saved to " << outputFilename << endl;
}

//read transactions from text file
vector<Itemset> readTransactions(const string& filename) {
    ifstream file(filename);
    vector<Itemset> transactions;
    string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            stringstream ss(line);
            Itemset transaction;
            int item;
            while (ss >> item) {
                transaction.insert(item);
            }
            transactions.push_back(transaction);
        }
        file.close();
    }
    else {
        cerr << "Unable to open file" << endl;
    }

    return transactions;
}

int main() {
    string inputFilename = "td.txt";
    string outputFilename = "output_hash.txt";
    int globalMinSupport = 10;  

    vector<Itemset> transactions = readTransactions(inputFilename);

    hashBasedApriori(transactions, globalMinSupport, outputFilename);

    return 0;
}
