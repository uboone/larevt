#ifndef DBFOLDER_CXX 
#define DBFOLDER_CXX 1

#include "DBFolder.h"
#include "WebDBIConstants.h"
#include "larevt/CalibrationDBI/IOVData/IOVDataConstants.h"
#include "larevt/CalibrationDBI/IOVData/TimeStampDecoder.h"
#include "WebError.h"
#include <sstream>
#include <limits>
#include <iomanip>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include "wda.h"
#include "sqlite3.h"
#include "cetlib_except/exception.h"
#include "cetlib/search_path.h"

namespace lariov {

  // Constructor.

  DBFolder::DBFolder(const std::string& name, const std::string& url, const std::string& url2,
		     const std::string& tag, bool usesqlite, bool testmode)
  {

    fFolderName = name;
    fURL = url;
    fURL2 = url2;
    fTag = tag;
    fUseSQLite = usesqlite;
    fTestMode = testmode;
    if (fURL[fURL.length()-1] == '/') {
      fURL = fURL.substr(0, fURL.length()-1);
    }

    fCachedRowNumber = -1;
    fCachedChannel = 0;
    
    fMaximumTimeout = 4*60; //4 minutes

    // If UsqSQLite is true, hunt for sqlite database file.
    // It is an error if this file can't be found.

    //std::cout << "DBFolder: Folder name = " << fFolderName << std::endl;
    if(fUseSQLite) {
      std::string dbname = fFolderName + ".db";
      cet::search_path sp("FW_SEARCH_PATH");
      fSQLitePath = sp.find_file(dbname);   // Throws exception if not found.
      //std::cout << "DBFolder: SQLite database path = " << fSQLitePath << std::endl;
    }
    //else
    //  std::cout << "DBFolder: database url = " << fURL << std::endl;

    if(fTestMode && fURL2 != "") {
      std::cout << "\nDBFolder test mode, will compare the following urls data." << std::endl;
      std::cout << fURL << std::endl;
      std::cout << fURL2 << "\n" << std::endl;
    }
    if(fTestMode && fUseSQLite) {
      std::cout << "\nDBFolder test mode, will compare the following url and sqlite data." << std::endl;
      std::cout << fURL << std::endl;
      std::cout << fSQLitePath << "\n" << std::endl;
    }
  }

  // Destructor.

  DBFolder::~DBFolder() {}

  // Data accessors.

  int DBFolder::GetNamedChannelData(DBChannelID_t channel, const std::string& name, bool& data) {

    int err = 0;

    // Make sure cached row is valid.

    GetRow(channel);

    // Get column index.

    size_t col = GetColumn(name);

    // Get value.

    long value = fCachedRow.getLongData(col);
    data = (value != 0);

    return err;
  }
  
  int DBFolder::GetNamedChannelData(DBChannelID_t channel, const std::string& name, long& data) {

    int err = 0;

    // Make sure cached row is valid.

    GetRow(channel);

    // Get column index.

    size_t col = GetColumn(name);

    // Get value.

    data = fCachedRow.getLongData(col);

    // Done.

    return err;
  }

  int DBFolder::GetNamedChannelData(DBChannelID_t channel, const std::string& name, double& data) {

    int err = 0;

    // Make sure cached row is valid.

    GetRow(channel);

    // Get column index.

    size_t col = GetColumn(name);

    // Get value.

    data = fCachedRow.getDoubleData(col);

    // Done.

    return err;
  }

  int DBFolder::GetNamedChannelData(DBChannelID_t channel, const std::string& name, std::string& data) {

    int err = 0;

    // Make sure cached row is valid.

    GetRow(channel);

    // Get column index.

    size_t col = GetColumn(name);

    // Get value.

    data = fCachedRow.getStringData(col);

    // Done.

    return err;
  }

  // Not sure why the following accessor is included.  Doesn't seem to be used.

  /*
  int DBFolder::GetNamedChannelData(DBChannelID_t channel, const std::string& name, std::vector<double>& data) {
    
    data.clear();
    
    Tuple tup;
    size_t col = this->GetRow(channel, name, tup);
    int err=0;
    double buf[kBUFFER_SIZE];
    
    DataRec *dataRec = (DataRec *)tup;
    // for c2: col is an unsigned int and cannot be less than 0
    // if (col < 0 || col >= dataRec->ncolumns) {
    if (col >= dataRec->ncolumns) {
      err=-1;
      return err;
    }
    
    char* sptr = dataRec->columns[col];
    if ( *sptr == '[') sptr +=1;  //expect an initial bracket and skip it
    else {
      err=-2;
      return err;
    }
    
    char* eptr;
    double val;
    unsigned int array_size=0;
    for (unsigned int i=0; i < kBUFFER_SIZE; ++i) {
      val = strtod(sptr, &eptr); //Try to convert
      if (sptr==eptr) break;     //conversion failed
      if (*sptr=='\0') break;    //end loop if buffer ends
      
      buf[array_size++] = val;
      
      if ( *eptr == ']') break;  //found the closing bracket, we're done
      else sptr = eptr+1;        //point to the next value
    }
    
    data.insert(data.begin(), buf, buf + array_size);
    releaseTuple(tup);
    return err;
  }
  */

  int DBFolder::GetChannelList( std::vector<DBChannelID_t>& channels ) const {

    channels = fCache.channels();
    return 0;
  }

  // Update cached row.

  void DBFolder::GetRow(DBChannelID_t channel) {

    // Check if we need to update the cached row.

    if (fCachedChannel != channel ||
	!fCachedRow.isValid() ||
	fCachedRow.getLongData(0) != channel) {

      // Update cached row number (binary serach).

      int row = fCache.getRowNumber(channel);

      //  Throw an exception if we didn't find a matching role.

      if(row < 0) {
	std::string msg = "Channel " + std::to_string(channel) + " is not found in database!";
	throw WebError(msg);
      }

      // Update cached row.

      fCachedRowNumber = row;
      fCachedChannel = channel;
      fCachedRow = fCache.getRow(row);
    }
  }

  // Find matching column.

  size_t DBFolder::GetColumn(const std::string& name) const
  {
    int col = fCache.getColNumber(name);

    // See if we found a matching column.

    if(col < 0) {
      std::string msg = "Column " + name + " is not found in database!";
      throw WebError(msg);
    }

    // Done.

    return col;
  }

  //returns true if an Update is performed, false if not
  bool DBFolder::UpdateData( DBTimeStamp_t raw_time) {
  
    //convert to IOVTimeStamp
    IOVTimeStamp ts = TimeStampDecoder::DecodeTimeStamp(raw_time);

    //check if cache is updated
    if (IsValid(ts)) return false;

    //release cached data.
    fCache.clear();
    fCachedRow.clear();
    fCachedRowNumber = -1;
    fCachedChannel = 0;
      
    //get full url string
    std::stringstream fullurl;
    fullurl << fURL << "/data?f=" << fFolderName
            << "&t=" << ts.DBStamp();
    if (fTag.length() > 0) fullurl << "&tag=" << fTag;

    //std::cout << "In DBFolder::UpdateData" << std::endl;
    //std::cout << "t=" << raw_time/1000000000 << std::endl;
    //std::cout << "Full url = " << fullurl.str() << std::endl;

    //get new dataset
    if(fSQLitePath != "" && !fTestMode) {
      GetSQLiteData(raw_time/1000000000, fCache);
    }
    else {
      if(fTestMode) {
	std::cout << "Accessing primary calibration data from http conditions database server." << std::endl;
	std::cout << "Folder = " << fFolderName << std::endl;
      }
      int err = 0;
      Dataset data = getDataWithTimeout(fullurl.str().c_str(), NULL, fMaximumTimeout, &err);
      int status = getHTTPstatus(data);
      if (status != 200) {
	std::string msg = "HTTP error from " + fullurl.str()+": status: " + std::to_string(status) + ": " + std::string(getHTTPmessage(data));
	throw WebError(msg);
      }
      fCache.update(data, true);
    }
    //DumpDataset(fCache);


    // If test mode is selected, get comparison data.

    if(fTestMode) {
      if(fSQLitePath != "") {
	DBDataset compare1;
	std::cout << "Accessing comparison data from sqlite database " << fSQLitePath << std::endl;
	GetSQLiteData(raw_time/1000000000, compare1);
	CompareDataset(fCache, compare1);
      }
      if(fURL2 != "") {
	std::cout <<"Accessing comparison data from second database url." << std::endl;
	std::stringstream fullurl2;
	fullurl2 << fURL2 << "/data?f=" << fFolderName
		 << "&t=" << ts.DBStamp();
	if (fTag.length() > 0) fullurl2 << "&tag=" << fTag;
	std::cout << "Full url = " << fullurl2.str() << std::endl;
	int err = 0;
	Dataset data = getDataWithTimeout(fullurl2.str().c_str(), NULL, fMaximumTimeout, &err);
	int status = getHTTPstatus(data);
	if (status != 200) {
	  std::string msg = "HTTP error from " + fullurl2.str()+": status: " + std::to_string(status) + ": " + std::string(getHTTPmessage(data));
	  throw WebError(msg);
	}
	DBDataset compare2(data, true);
	CompareDataset(fCache, compare2);
      }
    }
    return true;
  }

  // Query data from sqlite database.
  // The return value of type Dataset (aka void*), is partially opaque type HttpResponse*
  // (defined in wda.c and copied above).

  void DBFolder::GetSQLiteData(int t, DBDataset& data) const
  {
    if(fSQLitePath == "")
      return;

    //std::cout << "DBFolder::GetSQLiteData" << std::endl;
    //std::cout << "t=" << t << std::endl;
    //std::cout << "sqlite path = " << fSQLitePath << std::endl;

    // Open sqlite database.

    //std::cout << "Opening sqlite database " << fSQLitePath << std::endl;
    sqlite3* db;
    int rc = sqlite3_open(fSQLitePath.c_str(), &db);
    if(rc != SQLITE_OK) {
      std::cout << "Failed to open sqlite database " << fSQLitePath << std::endl;
      throw cet::exception("DBFolder") << "Failed to open sqlite database " << fSQLitePath;
    }

    // Query begin time of IOV.

    std::string table_iovs = fFolderName + "_iovs";
    std::string table_tag_iovs = fFolderName + "_tag_iovs";
    std::ostringstream sql;
    sql << "SELECT " << table_iovs << ".iov_id," << table_iovs << ".begin_time"
	<< " FROM " << table_tag_iovs << "," << table_iovs
	<< " WHERE " << table_tag_iovs << ".tag='" << fTag << "'"
	<< " AND " << table_tag_iovs << ".iov_id=" << table_iovs << ".iov_id"
	<< " AND " << table_iovs << ".begin_time <= " << t
	<< " ORDER BY " << table_iovs << ".begin_time desc";
    //std::cout << "sql = " << sql.str() << std::endl;

    // Prepare query.

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, 0);
    if(rc != SQLITE_OK) {
      std::cout << "sqlite3_prepare_v2 failed." << fSQLitePath << std::endl;
      std::cout << "Failed sql = " << sql.str() << std::endl;
      throw cet::exception("DBFolder") << "sqlite3_prepare_v2 error.";
    }

    // Execute query.
    // Just retrieve first row.
    // It is an error if we don't get at least one row.

    rc = sqlite3_step(stmt);
    //int iov_id = 0;
    int begin_time = 0;
    if(rc == SQLITE_ROW) {
      //iov_id = sqlite3_column_int(stmt, 0);
      begin_time = sqlite3_column_int(stmt, 1);
      //std::cout << "iov_id = " << iov_id << std::endl;
      //std::cout << "begin_time = " << begin_time << std::endl;
    }
    else {
      std::cout << "sqlite3_step returned error result = " << rc << std::endl;
      throw cet::exception("DBFolder") << "sqlite3_step error.";
    }

    // Delete query.

    sqlite3_finalize(stmt);

    // Query end time of IOV.

    sql.str("");
    sql << "SELECT " << table_iovs << ".begin_time"
	<< " FROM " << table_tag_iovs << "," << table_iovs
	<< " WHERE " << table_tag_iovs << ".tag='" << fTag << "'"
	<< " AND " << table_tag_iovs << ".iov_id=" << table_iovs << ".iov_id"
	<< " AND " << table_iovs << ".begin_time > " << t
	<< " ORDER BY " << table_iovs << ".begin_time";
    //std::cout << "sql = " << sql.str() << std::endl;

    // Prepare query.

    rc = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, 0);
    if(rc != SQLITE_OK) {
      std::cout << "sqlite3_prepare_v2 failed." << fSQLitePath << std::endl;
      std::cout << "Failed sql = " << sql.str() << std::endl;
      throw cet::exception("DBFolder") << "sqlite3_prepare_v2 error.";
    }

    // Execute query.
    // Just retrieve first row.
    // If we don't get any rows, then end time is infinite.

    rc = sqlite3_step(stmt);
    int end_time = 0;
    if(rc == SQLITE_ROW) {
      end_time = sqlite3_column_int(stmt, 0);
      //std::cout << "end_time = " << end_time << std::endl;
    }
    else if(rc != SQLITE_DONE) {
      std::cout << "sqlite3_step returned error result = " << rc << std::endl;
      throw cet::exception("DBFolder") << "sqlite3_step error.";
    }

    // Delete query.

    sqlite3_finalize(stmt);

    // Query count of channels.
    // We do this so that we know how much memory to allocate.

    std::string table_data = fFolderName + "_data";
    sql.str("");
    sql << "SELECT COUNT(DISTINCT channel)"
	<< " FROM " << table_data << "," << table_iovs << "," << table_tag_iovs
	<< " WHERE " << table_tag_iovs << ".tag='" << fTag << "'"
	<< " AND " << table_iovs << ".iov_id=" << table_tag_iovs << ".iov_id"
	<< " AND " << table_data << ".__iov_id=" << table_tag_iovs << ".iov_id"
	<< " AND " << table_iovs << ".begin_time <= " << t;
    //std::cout << "sql = " << sql.str() << std::endl;

    // Prepare query.

    rc = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, 0);
    if(rc != SQLITE_OK) {
      std::cout << "sqlite3_prepare_v2 failed." << fSQLitePath << std::endl;
      std::cout << "Failed sql = " << sql.str() << std::endl;
      throw cet::exception("DBFolder") << "sqlite3_prepare_v2 error.";
    }

    // Execute query.
    // Retrieve one row.
    // It is an error if we don't get at least one row.

    rc = sqlite3_step(stmt);
    unsigned int nrows = 0;
    if(rc == SQLITE_ROW) {
      nrows = sqlite3_column_int(stmt, 0);
      //std::cout << "Number of data rows = " << nrows << std::endl;
    }
    else {
      std::cout << "sqlite3_step returned error result = " << rc << std::endl;
      throw cet::exception("DBFolder") << "sqlite3_step error.";
    }

    // Delete query.

    sqlite3_finalize(stmt);

    // Stash begin time.

    data.setBeginTime(IOVTimeStamp(begin_time, 0));

    // Stash end time.

    if(end_time == 0)
      data.setEndTime(IOVTimeStamp::MaxTimeStamp());
    else
      data.setEndTime(IOVTimeStamp(end_time, 0));

    // Stash number of rows.

    data.setNRows(nrows);

    // Main data query.

    sql.str("");
    sql << "SELECT " << table_data << ".*,MAX(begin_time)"
	<< " FROM " << table_data << "," << table_iovs << "," << table_tag_iovs
	<< " WHERE " << table_tag_iovs << ".tag='" << fTag << "'"
	<< " AND " << table_iovs << ".iov_id=" << table_tag_iovs << ".iov_id"
	<< " AND " << table_data << ".__iov_id=" << table_tag_iovs << ".iov_id"
	<< " AND " << table_iovs << ".begin_time <= " << t
	<< " GROUP BY channel"
	<< " ORDER BY channel";
    //std::cout << "sql = " << sql.str() << std::endl;

    // Prepare query.

    rc = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, 0);
    if(rc != SQLITE_OK) {
      std::cout << "sqlite3_prepare_v2 failed." << fSQLitePath << std::endl;
      std::cout << "Failed sql = " << sql.str() << std::endl;
      throw cet::exception("DBFolder") << "sqlite3_prepare_v2 error.";
    }

    // Execute the query and retrieve one row.
    // We do this to extract the number, names, and types of relevant columns.

    int ncols = sqlite3_column_count(stmt);
    std::vector<std::string>& column_names = data.colNames();
    std::vector<std::string>& column_types = data.colTypes();
    column_names.reserve(ncols);
    column_types.reserve(ncols);
    rc = sqlite3_step(stmt);
    if(rc == SQLITE_ROW) {

      // Loop over columns.

      for(int col = 0; col < ncols; ++col) {
	std::string colname = sqlite3_column_name(stmt, col);

	// Ignore columns that begin with "_".
	// Also ignore utility column MAX(begin_time).

	if(colname[0] != '_' && colname.substr(0,3) != "MAX") {
	  column_names.push_back(colname);
	  int dtype = sqlite3_column_type(stmt, col);
	  if(dtype == SQLITE_INTEGER)
	    column_types.push_back("integer");
	  else if(dtype == SQLITE_FLOAT)
	    column_types.push_back("real");
	  else if(dtype == SQLITE_TEXT)
	    column_types.push_back("text");
	  else if(dtype == SQLITE_NULL)
	    column_types.push_back("NULL");
	  else {
	    std::cout << "Unknown type " << dtype << std::endl;
	    throw cet::exception("DBFolder") << "Unknown type " << dtype;
	  }
	  //std::cout << "Column " << col 
	  //	    << ", name=" << column_names.back()
	  //	    << ", type=" << column_types.back() << std::endl;
	}
      }
    }
    else {
      std::cout << "No data rows." << std::endl;
      throw cet::exception("DBFolder") << "No data rows.";
    }

    // Stash the number of relevant columns.

    data.setNCols(column_names.size());

    // Get modifiable values and channels collections, and reserve memory for them.

    std::vector<DBDataset::value_type>& values = data.data();
    std::vector<DBChannelID_t>& channels = data.channels();
    values.reserve(data.nrows() * data.ncols());
    channels.reserve(data.nrows());

    // Re-execute query.
    // Retrieve all data rows and stash in result.

    rc = sqlite3_reset(stmt);
    if(rc != SQLITE_OK) {
      std::cout << "sqlite3_reset failed." << std::endl;
      throw cet::exception("DBFolder") << "sqlite3_failed.";
    }
    nrows = 0;
    while(rc != SQLITE_DONE) {
      rc = sqlite3_step(stmt);
      if(rc == SQLITE_ROW) {
	++nrows;
	//std::cout << nrows << " rows." << std::endl;
	if(nrows > data.nrows()) {
	  std::cout << "Too many data rows " << nrows << std::endl;
	  throw cet::exception("DBFolder") << "Too many data rows " << nrows;
	}

	// Loop over columns.
	// Remember that ncols is the number of columns returned by the query,
	// not the number of columns that get stored (some columns are ignored).

	bool firstcol = true;
	for(int col = 0; col < ncols; ++col) {
	  std::string colname = sqlite3_column_name(stmt, col);

	  // Ignore columns that begin with "_".
	  // Also ignore utility column MAX(begin_time).

	  if(colname[0] != '_' && colname.substr(0,3) != "MAX") {
	    int dtype = sqlite3_column_type(stmt, col);
	    values.emplace_back(DBDataset::value_type());

	    if(dtype == SQLITE_INTEGER) {
	      values.back().longValue = sqlite3_column_int(stmt, col);
	      //std::cout << "Value = " << values.back().longValue << std::endl;
	      if(firstcol)
		channels.push_back(values.back().longValue);
	    }
	    else if(dtype == SQLITE_FLOAT) {
	      values.back().doubleValue = sqlite3_column_double(stmt, col);
	      //std::cout << "Value = " << values.back().doubleValue << std::endl;	    
	      if(firstcol) {
		std::cout << "First column has wrong type float." << std::endl;
		throw cet::exception("DBFolder") << "First column has wrong type float.";
	      }
	    }
	    else if(dtype == SQLITE_TEXT) {
	      const char* s = (const char*)sqlite3_column_text(stmt, col);
	      size_t nch = strlen(s) + 1;
	      values.back().charValue = new char[nch];
	      memcpy(values.back().charValue, s, nch);
	      //std::cout << "Value = " << values.back().charValue << std::endl;
	      if(firstcol) {
		std::cout << "First column has wrong type text." << std::endl;
		throw cet::exception("DBFolder") << "First column has wrong type text.";
	      }
	    }
	    else if(dtype == SQLITE_NULL) {
	      values.back().longValue = 0;
	      //std::cout << "Value = NULL" << std::endl;	    
	      if(firstcol) {
		std::cout << "First column has wrong type null." << std::endl;
		throw cet::exception("DBFolder") << "First column has wrong type null.";
	      }
	    }
	    else {
	      std::cout << "Unrecognized sqlite data type" << std::endl;
	      throw cet::exception("DBFolder") << "Unrecognized sqlite data type.";
	    }
	    firstcol = false;
	  }
	}
      }
      else if(rc != SQLITE_DONE) {
	std::cout << "sqlite3_step returned error result = " << rc << std::endl;
	throw cet::exception("DBFolder") << "sqlite3_step error.";
      }
    }
    if(nrows != data.nrows()) {
      std::cout << "Wrong number of data rows " << nrows << std::endl;
      throw cet::exception("DBFolder") << "Wrong number of data rows " << nrows;
    }
    if(values.size() != data.nrows() * data.ncols()) {
      std::cout << "Wrong number of values " << values.size() << std::endl;
      throw cet::exception("DBFolder") << "Wrong number of values " << values.size();
    }

    // Delete statement.

    sqlite3_finalize(stmt);

    // Close database.

    sqlite3_close(db);

    // Done.

    return;
  }

  // Dump dataset by rows.

  void DBFolder::DumpDataset(const DBDataset& data) const
  {
    size_t nrows = data.nrows();
    size_t ncols = data.ncols();
    std::cout << "Dataset contains " << nrows << " rows and " << ncols << " columns." << std::endl;

    // Begin time.

    std::cout << "IOV start time = " << data.beginTime().DBStamp() << std::endl;

    // End time.

    std::cout << "IOV end time = " << data.endTime().DBStamp() << std::endl;

    // Columnn names.

    const std::vector<std::string>& names = data.colNames();
    for (size_t c=0; c<ncols; ++c)
      std::cout << "Column " << c << ", name = " << names[c] << std::endl;

    // Row 3 - column types.

    const std::vector<std::string>& types = data.colTypes();
    for (size_t c=0; c<ncols; ++c)
      std::cout << "Column " << c << ", type = " << types[c] << std::endl;

    // Data rows.

    for(size_t row=0; row<nrows; ++row) {
      std::cout << "\nRow " << row << std::endl;
      DBDataset::DBRow dbrow = data.getRow(row);

      // Loop over columns.

      for(size_t col=0; col<ncols; ++col) {
	if(types[col] == "bigint" || types[col] == "integer" || types[col] == "boolean") {
	  long value = dbrow.getLongData(col);
	  std::cout << names[col] << " = " << value << std::endl;
	}
	else if(types[col] == "real") {
	  double value = dbrow.getDoubleData(col);
	  std::cout << names[col] << " = " << value << std::endl;
	}
	else if(types[col] == "text" or types[col] == "boolean") {
	  std::string value = dbrow.getStringData(col);
	  std::cout << names[col] << " = " << value << std::endl;
	}
	else {
	  std::cout << "Unknown type " << types[col] << std::endl;
	  throw cet::exception("DBFolder") << "Unknown type.";
	}
      }
    }
  }

  void DBFolder::CompareDataset(const DBDataset& data1, DBDataset& data2) const
  {
    bool compare_ok = true;
    std::cout << "\nComparing datasets." << std::endl;

    size_t nrows1 = data1.nrows();
    size_t nrows2 = data2.nrows();
    //std::cout << "Dataset 1 contains " << nrows1 << " rows." << std::endl;
    //std::cout << "Dataset 2 contains " << nrows2 << " rows." << std::endl;
    if(nrows1 != nrows2)
      compare_ok = false;

    // Compare begin time.

    std::string begin1 = data1.beginTime().DBStamp();
    std::string begin2 = data2.beginTime().DBStamp();
    if(begin1 != begin2)
      compare_ok = false;

    // Compare end time.

    std::string end1 = data1.beginTime().DBStamp();
    std::string end2 = data2.beginTime().DBStamp();
    if(end1 != end2)
      compare_ok = false;

    // Compare column names.

    size_t ncols1 = data1.ncols();
    size_t ncols2 = data2.ncols();
    const std::vector<std::string>& names1 = data1.colNames();
    const std::vector<std::string>& names2 = data2.colNames();
    if(ncols1 != ncols2 || ncols1 != names1.size() || ncols2 != names2.size()) {
      std::cout << "Columns names size mismatch " << ncols1 
		<< " vs. " << ncols2 
		<< " vs. " << names1.size()
		<< " vs. " << names2.size()
		<< std::endl;
      compare_ok = false;
    }
    if(compare_ok) {
      for (size_t c=0; c<ncols1; ++c) {
	if(names1[c] != names2[c]) {
	  std::cout << "Name mismatch " << names1[c] << " vs. " << names2[c] << std::endl;
	  compare_ok = false;
	}
      }
    }

    // Compare column types.

    const std::vector<std::string>& types1 = data1.colTypes();
    const std::vector<std::string>& types2 = data2.colTypes();
    if(ncols1 != ncols2 || ncols1 != types1.size() || ncols2 != types2.size()) {
      std::cout << "Column types ize mismatch " << ncols1 
		<< " vs. " << ncols2 
		<< " vs. " << types1.size()
		<< " vs. " << types2.size()
		<< std::endl;
      compare_ok = false;
    }
    if(compare_ok) {
      for (size_t c=0; c<ncols2; ++c) {

	// Type "bigint" matches "integer."
	// Type "boolean" matches "integer."

	std::string type1 = types1[c];
	std::string type2 = types2[c];
	if(type1 == "bigint" || type1 == "boolean")
	  type1 = "integer";
	if(type2 == "bigint" || type2 == "boolean")
	  type2 = "integer";
	if(type1 != type2) {
	  std::cout << "Type mismatch " << type1 << " vs. " << type2 << std::endl;
	  compare_ok = false;
	}
      }
    }

    // Compare channels.

    const std::vector<DBChannelID_t>& channels1 = data1.channels();
    const std::vector<DBChannelID_t>& channels2 = data2.channels();
    if(nrows1 != nrows2 || nrows1 != channels1.size() || nrows2 != channels2.size()) {
      std::cout << "Channels size mismatch " << nrows1 
		<< " vs. " << nrows2 
		<< " vs. " << channels1.size()
		<< " vs. " << channels2.size()
		<< std::endl;
      compare_ok = false;
    }
    if(compare_ok) {
      for (size_t r=0; r<nrows1; ++r) {
	if(channels1[r] != channels2[r]) {
	  std::cout << "Channel mismatch " << channels1[r] << " vs. " << channels2[r] << std::endl;
	  compare_ok = false;
	}
      }
    }

    // Compare number of values.

    if(data1.data().size() != data2.data().size()) {
      std::cout << "Values size mismatch " << data1.data().size()
		<< " vs. " << data2.data().size()
		<< std::endl;
      compare_ok = false;
    }

    // Data rows.
    if(compare_ok) {
      for(size_t row=0; row<nrows1; ++row) {

	DBDataset::DBRow dbrow1 = data1.getRow(row);
	DBDataset::DBRow dbrow2 = data2.getRow(row);
	//std::cout << "\nRow " << row << std::endl;

	// Loop over columns.

	for(size_t col=0; col<ncols1; ++col) {
	  if(types1[col] == "integer" || types1[col] == "bigint" ||  types1[col] == "boolean") {
	    long value1 = dbrow1.getLongData(col);
	    long value2 = dbrow2.getLongData(col);
	    //std::cout << names1[col] << " 1 = " << value1 << std::endl;
	    //std::cout << names2[col] << " 2 = " << value2 << std::endl;
	    if(value1 != value2) {
	      std::cout << "Value mismatch " << value1 << " vs. " << value2 << std::endl;
	      compare_ok = false;
	    }
	  }
	  else if(types1[col] == "real") {
	    double value1 = dbrow1.getDoubleData( col);
	    double value2 = dbrow2.getDoubleData( col);
	    //std::cout << names1[col] << " 1 = " << value1 << std::endl;
	    //std::cout << names2[col] << " 2 = " << value2 << std::endl;
	    if(value1 != value2) {
	      std::cout << "Value mismatch " << value1 << " vs. " << value2 << std::endl;
	      compare_ok = false;
	    }
	  }
	  else if(types1[col] == "text") {
	    std::string value1 = dbrow2.getStringData(col);
	    std::string value2 = dbrow2.getStringData(col);
	    if(value1 != value2) {
	      std::cout << "Value mismatch " << value1 << " vs. " << value2 << std::endl;
	      compare_ok = false;
	    }
	  }
	  else {
	    std::cout << "Unknown type " << types1[col] << std::endl;
	    throw cet::exception("DBFolder") << "Unknown type.";
	  }
	}
      }
    }

    if(compare_ok) {
      std::cout << "Comparison OK.\n" << std::endl;
    }
    else{
      std::cout << "Comparison fail." << std::endl;
      throw cet::exception("DBFolder") << "Comparison fail.";
    }
    return;
  }

}//end namespace lariov
  
#endif  
