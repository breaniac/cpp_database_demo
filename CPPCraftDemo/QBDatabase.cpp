#include "stdafx.h"
#include "QBDatabase.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <mutex>

// Move constructor implementation
QBDatabase::QBDatabase(QBDatabase &&other) noexcept
{
    std::unique_lock<std::shared_mutex> lock_other(other.m_mutex);

    db_records = std::move(other.db_records);
    db_idx_col2 = std::move(other.db_idx_col2);

    other.db_records.clear();
    other.db_idx_col2.clear();
}

size_t QBDatabase::size() const
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    return db_records.size();
}

int QBDatabase::insert(QBRecord rec)
{
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    unsigned int id = rec.column0;
    if(db_records.find(id) != db_records.end()) {
        std::cerr << "There is already inserted id: " << id << std::endl;
        return -1;
    }
    
    db_idx_col2[rec.column2].push_back(id);
    db_records.emplace(id, std::move(rec));
    return 0;
}

QBRecordCollection QBDatabase::findMatching(const std::string& columnName,
                                             const std::string& matchString) const
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    QBRecordCollection result;

    try {
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
    // A guard — catching exeptions from std::stoul/stol
    } catch (const std::exception& e) {
        std::cerr << "Error in findMatching: " << e.what() << std::endl;
        return {};
    }

    return result;
}

bool QBDatabase::deleteById(unsigned int id)
{
    std::unique_lock<std::shared_mutex> lock(m_mutex);
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
