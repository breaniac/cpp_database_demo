#include "stdafx.h"
#include "QBDatabase.h"
#include <algorithm>

size_t QBDatabase::size() const
{
    return db_records.size();
}

void QBDatabase::insert(QBRecord rec)
{
    unsigned int id = rec.column0;
    db_idx_col2[rec.column2].push_back(id);
    db_records.emplace(id, std::move(rec));
}

QBRecordCollection QBDatabase::findMatching(const std::string& columnName,
                                             const std::string& matchString) const
{
    QBRecordCollection result;

    if (columnName == "column0") {
        unsigned int key = std::stoul(matchString);
        auto it = db_records.find(key);
        if (it != db_records.end())
            result.push_back(it->second);

    } else if (columnName == "column2") {
        long key = std::stol(matchString);
        auto it = db_idx_col2.find(key);
        
        if (it != db_idx_col2.end()) {
            for (unsigned int id : it->second) {
                auto rec_it = db_records.find(id);
                if (rec_it != db_records.end())
                    result.push_back(rec_it->second);
            }
        }

    } else if (columnName == "column1") {
        for (const auto& entry : db_records) {
            if (entry.second.column1.find(matchString) != std::string::npos)
                result.push_back(entry.second);
        }

    } else if (columnName == "column3") {
        for (const auto& entry : db_records) {
            if (entry.second.column3.find(matchString) != std::string::npos)
                result.push_back(entry.second);
        }
    }

    return result;
}

bool QBDatabase::deleteById(unsigned int id)
{
    auto rec_it = db_records.find(id);
    if (rec_it == db_records.end())
        return false;

    long col2 = rec_it->second.column2;

    auto col2_it = db_idx_col2.find(col2);
    if (col2_it != db_idx_col2.end()) {
        std::vector<unsigned int>& ids = col2_it->second;
        ids.erase(std::remove(ids.begin(), ids.end(), id), ids.end());
        
        if (ids.empty())
            db_idx_col2.erase(col2_it);
    }

    db_records.erase(rec_it);
    return true;
}
