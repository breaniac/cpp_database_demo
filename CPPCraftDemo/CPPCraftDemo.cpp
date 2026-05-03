#include "stdafx.h"

#include "QBRecord.h"
#include "QBDatabase.h"

#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <chrono>
#include <iostream>

constexpr unsigned int dummy_data_size = 1000;
// Why this ITER will be used:
// we want to have a long enough time for the profiler to get accurate measurements, but not too long to make the test run for a very long time. 100 iterations should be enough to get a good measurement while keeping the test duration reasonable.
constexpr int ITER = 100;
constexpr int id_to_delete = 500;
constexpr int id_to_delete_nonexist = 99999999;
constexpr const char *substr_search_col1 = "testdata500";
constexpr const char *substr_search_col2 = "24";


/**
    Return records that contains a string in the StringValue field
    records - the initial set of records to filter
    matchString - the string to search for
*/
QBRecordCollection QBFindMatchingRecords(const QBRecordCollection &records, const std::string &columnName, const std::string &matchString)
{
    QBRecordCollection result;
    std::copy_if(records.begin(), records.end(), std::back_inserter(result), [&](QBRecord rec){
        if (columnName == "column0") {
            unsigned int matchValue = std::stoul(matchString);
            return matchValue == rec.column0;
        } else if (columnName == "column1") {
            return rec.column1.find(matchString) != std::string::npos;
        } else if (columnName == "column2") {
            long matchValue = std::stol(matchString);
            return matchValue == rec.column2;
        } else if (columnName == "column3") {
            return rec.column3.find(matchString) != std::string::npos;
        } else {
            return false;
        }
    });
    return result;
}

/**
    Utility to populate a record collection
    prefix - prefix for the string value for every record
    numRecords - number of records to populate in the collection
*/
QBRecordCollection populateDummyData(const std::string& prefix, unsigned int numRecords)
{
    QBRecordCollection data;
    data.reserve(numRecords);
    for (unsigned int i = 0; i < numRecords; i++) {
        QBRecord rec = {i, prefix + std::to_string(i), static_cast<long>(i % 100), std::to_string(i) + prefix};
        data.emplace_back(rec);
    }
    return data;
}

static QBDatabase buildDatabase(const QBRecordCollection &data)
{
    QBDatabase db;
    for (const QBRecord &rec : data) {
        db.insert(rec);
    }
    return db;
}

int main(int argc, char *argv[])
{
    using namespace std::chrono;
    // populate a bunch of data
    auto data = populateDummyData("testdata", dummy_data_size);
    QBDatabase db = buildDatabase(data);
    assert(db.size() == dummy_data_size);

    bool ok = db.deleteById(id_to_delete);
    bool nok = db.deleteById(id_to_delete_nonexist);
    assert(ok && !nok);
    assert(db.findMatching("column0", std::to_string(id_to_delete)).empty());

    std::cout << "All correctness checks passed." << std::endl;

    // Find a record that contains and measure the perf
    auto startTimer = steady_clock::now();
    for (int i = 0; i < ITER; ++i) {
        auto filteredSet = QBFindMatchingRecords(data, "column1", substr_search_col1);
        auto filteredSet2 = QBFindMatchingRecords(data, "column2", substr_search_col2);
    }
    double oldTime = duration<double>(steady_clock::now() - startTimer).count();

    auto startTimerNew = steady_clock::now();
    for (int i = 0; i < ITER; ++i) {
        auto filteredSet = db.findMatching("column1", substr_search_col1);
        auto filteredSet2 = db.findMatching("column2", substr_search_col2);
    }
    double newTime = duration<double>(steady_clock::now() - startTimerNew).count();

    std::cout << "profiler old: " << oldTime << " s" << std::endl;
    std::cout << "profiler new: " << newTime << " s" << std::endl;
    if (newTime > 0.0) {
        std::cout << "speedup: " << (oldTime / newTime) << "x" << std::endl;
    }

    return 0;
}
