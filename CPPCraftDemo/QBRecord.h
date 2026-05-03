#pragma once

#include <string>
#include <vector>

/// @brief Represents a record in the QBDatabase.
struct QBRecord
{
    unsigned int column0;   /// Prim key canditade
    std::string  column1;   /// Substring search candidate  
    long         column2;   /// Non-unique exact match candidate
    std::string  column3;   /// Substring search candidate
};

/// @brief Represents a Record Collections.
typedef std::vector<QBRecord> QBRecordCollection;
