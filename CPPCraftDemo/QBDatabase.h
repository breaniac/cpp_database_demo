#pragma once

#include "QBRecord.h"

#include <string>
#include <unordered_map>
#include <vector>

class QBDatabase
{
public:
    /// @brief Insert a record into the database.
    /// @param rec The record to be inserted.
    void insert(QBRecord rec);

    /// @brief Find all records where a specific column matches a given string.
    /// @param columnName The name of the column to search.
    /// @param matchString The string to match in the specified column.
    /// @return A collection of records that match the criteria.
    QBRecordCollection findMatching(const std::string& columnName,
                                    const std::string& matchString) const;

    /// @brief Remove the record with the given ID.
    /// @param id The ID of the record to remove.
    /// @return True if the record was found and removed, false otherwise.
    bool deleteById(unsigned int id);

    /// @brief Get the number of records currently in the database.
    /// @return The number of records in the database.
    size_t size() const;

private:
    std::unordered_map<unsigned int, QBRecord>           db_records;  // id -> record (primary store + col0 index)
    std::unordered_map<long, std::vector<unsigned int>>  db_idx_col2; // col2 -> [record ids]
};
