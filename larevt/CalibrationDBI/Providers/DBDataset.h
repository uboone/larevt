#ifndef DBDATASET_H
#define DBDATASET_H
//=================================================================================
//
// Name: DBDataset.h
//
// Purpose: Header for class DBDataset.
//          This class represents data extracted from the opaque wda struct Dataset,
//          which struct represents the result of a calibration database query on a 
//          particular database table in an IOV database.  Data is extracted and 
//          copied from the Dataset struct using the wda api.
//
//          Database data are essentially a rectangular array of values, indexed by
//          (row, column).  Accessors are provided to access data as string, long, 
//          or double.
//
//          Rows are labeled by channel number.  Columns are labeled by name and type.
//
//          Binary values, which are stored as strings by wda, are stored as
//          binary values in a union in this class.
//
//          Columns are labeled by column name and type.
//
// Data members:
//
// fBeginTime - IOV begin validity time.
// fEndTime   - IOV end validity time.
// fNRows     - Number of rows.
// fNCols     - Number of columns.
// fColNames  - Names of columns.
// fColTypes  - Data types of columns.
// fChannels  - Channel numbers (indexed by row number).
// fData      - Calibration data.
//
// Normally, the first element of each row is an integer channel number.
// Furthermore, it can be assumed that rows are ordered by increasing channel number.
// 
// Calibration data is contained in data member fData, which is a rectangular array 
// of strings of dimension fNRows x fNCols.  For efficiency, calibration data are
// stored using a single std::vector<std::string>, which array is allocated at
// construction or update time.  Elements are accessed columnwise as follows.
//
// value = fData[fNCols*row + column]
//
// Or use the provided accessors.
//
// Nested class DBRow provides access to data from a single database row.
//
// Created: 26-Oct-2020 - H. Greenlee
//
//=================================================================================

#include <string>
#include <vector>
#include "larevt/CalibrationDBI/Interface/CalibrationDBIFwd.h"
#include "larevt/CalibrationDBI/IOVData/IOVTimeStamp.h"

namespace lariov
{
  class DBDataset
  {

  public:

    // Nested union for storing binary values.

    union Value
    {
      long longValue;
      double doubleValue;
      char* charValue;
    };
    typedef Value value_type;

    // Nested class representing data from one row.

    class DBRow
    {
    public:

      // Constructors.

      DBRow() : fData(0) {}
      DBRow(const value_type* s) : fData(s) {}

      // Accessors.

      bool isValid() const {return fData != 0;}
      std::string getStringData(size_t col) const {return std::string(fData[col].charValue);}
      long getLongData(size_t col) const {return (fData[col].longValue);}
      double getDoubleData(size_t col) const {return fData[col].doubleValue;}

      // Modifiers.

      void clear() {fData = 0; return;}

    private:

      // Data member.

      const value_type* fData;   // Borrowed reference from enclosing class.
    };

  // Back to main class.

  public:

    // Constructors, destructor.

    DBDataset();                                   // Default constructor.
    DBDataset(void* dataset, bool release=false);  // Initializing constructor.
    DBDataset(const DBDataset&) = delete;          // Uncopyable.
    DBDataset(DBDataset&&) = delete;               // Unmovable.
    ~DBDataset();                                  // Destructor.

    // Assignment.

    DBDataset& operator=(const DBDataset&) = delete;  // Uncopyable.
    DBDataset& operator=(DBDataset&&) = delete;       // Unmovable.

    // Simple accessors.

    const IOVTimeStamp& beginTime() const {return fBeginTime;}
    const IOVTimeStamp& endTime() const {return fEndTime;}
    size_t nrows() const {return fNRows;}
    size_t ncols() const {return fNCols;}
    const std::vector<std::string>& colNames() const {return fColNames;}
    const std::vector<std::string>& colTypes() const {return fColTypes;}
    const std::vector<DBChannelID_t>& channels() const {return fChannels;}
    const std::vector<value_type>& data() const {return fData;}

    // Determine row and column numbers.

    int getRowNumber(DBChannelID_t ch) const;
    int getColNumber(const std::string& name) const;

    // Access one row.

    DBRow getRow(size_t row) const {return DBRow(&fData[fNCols*row]);}

    // Modifiers.

    void clear();                                    // Return to default-constructed state.
    void update(void* dataset, bool release=false);  // Reinitialize.
    void setBeginTime(const IOVTimeStamp& t) {fBeginTime = t;}
    void setEndTime(const IOVTimeStamp& t) {fEndTime = t;}
    void setNRows(size_t nrows) {fNRows = nrows;}
    void setNCols(size_t ncols) {fNCols = ncols;}

    // Modifiable collection accessors.

    std::vector<std::string>& colNames() {return fColNames;}
    std::vector<std::string>& colTypes() {return fColTypes;}
    std::vector<DBChannelID_t>& channels() {return fChannels;}
    std::vector<value_type>& data() {return fData;}

  private:

    // Data members.

    IOVTimeStamp fBeginTime;               // IOV begin time.
    IOVTimeStamp fEndTime;                 // IOV end time.
    size_t fNRows;                         // Number of rows.
    size_t fNCols;                         // Number of columns.
    std::vector<std::string> fColNames;    // Column names.
    std::vector<std::string> fColTypes;    // Column types.
    std::vector<DBChannelID_t> fChannels;  // Channels.
    std::vector<value_type> fData;         // Calibration data (length fNRows*fNCols).
  };
}

#endif
