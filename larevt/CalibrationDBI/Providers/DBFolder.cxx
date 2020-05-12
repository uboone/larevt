#include "DBFolder.h"
#include "WebDBIConstants.h"
#include "larevt/CalibrationDBI/IOVData/TimeStampDecoder.h"
#include "WebError.h"

#include <sstream>
#include <stdlib.h>
#include <cstring>
#include "wda.h"
#include "sqlite3.h"
#include "cetlib_except/exception.h"
#include "cetlib/search_path.h"

namespace lariov {

  // Copied from wda.c

  typedef struct {
      size_t ncolumns;    // Number of columns in CSV row
      size_t nelements;   // Number of elements in data array
      char **columns;     // Pointers to columns
  } DataRec;

  typedef struct {
    char *memory;       // The buffer from HTTP response
    size_t size;        // The size of the buffer (in bytes)
    size_t allocsize;   // The allocated size of the buffer (in bytes)
    char **rows;        // Array of rows in the buffer
    size_t nrows;       // Number of rows
    size_t idx;         // Current index
    long http_code;     // Status code
    DataRec *dataRecs[1]; // Array of parsed data records. Filled by getTuple calls
  } HttpResponse;

  DBFolder::DBFolder(const std::string& name, const std::string& url, const std::string& url2,
		     const std::string& tag, bool usesqlite, bool testmode) :
    fCachedStart(0,0), fCachedEnd(0,0) {

    fFolderName = name;
    fURL = url;
    fURL2 = url2;
    fTag = tag;
    fUseSQLite = usesqlite;
    fTestMode = testmode;
    if (fURL[fURL.length()-1] == '/') {
      fURL = fURL.substr(0, fURL.length()-1);
    }

    fCachedDataset = 0;
    fNRows =0;
    fColumns.clear();
    fTypes.clear();
    fCachedRow = -1;
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

  DBFolder::~DBFolder() {
    if (fCachedDataset) releaseDataset(fCachedDataset);
  }

  int DBFolder::GetNamedChannelData(DBChannelID_t channel, const std::string& name, bool& data) {

    Tuple tup;
    size_t col = this->GetTupleColumn(channel, name, tup);
    int err=0;
    char buf[kBUFFER_SIZE];
    int str_size = getStringValue(tup, col, buf, kBUFFER_SIZE, &err);
    std::string value = std::string(buf, str_size);
    data = false;
    if (value =="True" or value == "1") {
      data = true;
    }
    else if (value == "False" or value == "0") {
      data = false;
    }
    else std::cout<<"(DBFolder) ERROR: Can't identify data: "<<std::string(buf, str_size)<<" as boolean!"<<std::endl;

    releaseTuple(tup);
    return err;
  }

  int DBFolder::GetNamedChannelData(DBChannelID_t channel, const std::string& name, long& data) {

    Tuple tup;
    size_t col = this->GetTupleColumn(channel, name, tup);
    int err=0;

    //first handle special case that the db data is boolean, but user mistakenly used long version of this function
    char buf[kBUFFER_SIZE];
    int str_size = getStringValue(tup, col, buf, kBUFFER_SIZE, &err);
    if (std::string(buf, str_size)=="True") {
      data = 1;
    }
    else if (std::string(buf, str_size)=="False") {
      data = 0;
    }
    else { //ok, we really have a long (hopefully)
      data = getLongValue(tup, col, &err);
    }
    releaseTuple(tup);
    return err;
  }

  int DBFolder::GetNamedChannelData(DBChannelID_t channel, const std::string& name, double& data) {

    Tuple tup;
    size_t col = this->GetTupleColumn(channel, name, tup);
    int err=0;
    data = getDoubleValue(tup, col, &err);
    releaseTuple(tup);
    return err;
  }

  int DBFolder::GetNamedChannelData(DBChannelID_t channel, const std::string& name, std::string& data) {

    Tuple tup;
    size_t col = this->GetTupleColumn(channel, name, tup);
    int err=0;
    char buf[kBUFFER_SIZE];
    int str_size = getStringValue(tup, col, buf, kBUFFER_SIZE, &err);
    data = std::string(buf, str_size);
    releaseTuple(tup);
    return err;
  }

  int DBFolder::GetNamedChannelData(DBChannelID_t channel, const std::string& name, std::vector<double>& data) {

    data.clear();

    Tuple tup;
    size_t col = this->GetTupleColumn(channel, name, tup);
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

  int DBFolder::GetChannelList( std::vector<DBChannelID_t>& channels ) const {

    channels.clear();
    if (!fCachedDataset) return 1;

    Tuple tup;
    int err=0;
    for ( int row = 0; row != fNRows; ++row) {
      tup = getTuple(fCachedDataset, row + kNUMBER_HEADER_ROWS);
      channels.push_back( (DBChannelID_t)getLongValue(tup,0,&err) );
      releaseTuple(tup);
    }
    return err;
  }


  size_t DBFolder::GetTupleColumn(DBChannelID_t channel, const std::string& name, Tuple& tup ) {

    //check if cached row is still valid
    int err;
    int row = -1;
    if (fCachedRow != -1 && fCachedChannel == channel) {
      tup = getTuple(fCachedDataset, fCachedRow + kNUMBER_HEADER_ROWS);
      if ( channel == (DBChannelID_t)getLongValue(tup,0,&err) ) {
	row = fCachedRow;
      }
      else releaseTuple(tup);
    }

    //if cached row is not valid, find the new row
    if (row == -1) {
//std::cout<<"Channel "<<channel<<" not cached"<<std::endl;
      //binary search for channel
      DBChannelID_t val;
      int l = 0, h = fNRows - 1;
      row = (l + h )/2;
      while ( l <= h ) {
//std::cout<<"  "<<l<<"  "<<h<<"  "<<row<<std::endl;
	tup = getTuple(fCachedDataset, row + kNUMBER_HEADER_ROWS);
	val = getLongValue(tup, 0, &err);
	releaseTuple(tup);

	if (val == channel ) break;

	if (val > channel) h = row - 1;
	else            l = row + 1;

	row = (l + h)/2;
      }

      //get the tuple to be returned, check that the found row matches the requested channel
      tup = getTuple(fCachedDataset, row + kNUMBER_HEADER_ROWS);
      if ( channel != (DBChannelID_t)getLongValue(tup, 0, &err) ) {
        releaseTuple(tup);
	std::string msg = "Channel " + std::to_string(channel) + " is not found in database!";
	throw WebError(msg);
      }


      //update caching info
      fCachedChannel = channel;
      fCachedRow = row;

    }

    //get the column corresponding to input string name and return
    for (size_t c=1; c < fColumns.size(); ++c ) {
      if (name == fColumns[c]) return c;
    }

    std::string msg = "Column named " + name + " is not found in the database!";
    throw WebError(msg);
    return 0;
  }

  //returns true if an Update is performed, false if not
  bool DBFolder::UpdateData( DBTimeStamp_t raw_time) {

    //convert to IOVTimeStamp
    IOVTimeStamp ts = TimeStampDecoder::DecodeTimeStamp(raw_time);

    //check if cache is updated
    if (this->IsValid(ts)) return false;

    int err=0;

    //release old dataset
    if (fCachedDataset) releaseDataset(fCachedDataset);

    //get full url string
    std::stringstream fullurl;
    fullurl << fURL << "/data?f=" << fFolderName
            << "&t=" << ts.DBStamp();
    if (fTag.length() > 0) fullurl << "&tag=" << fTag;

    //std::cout << "In DBFolder::UpdateData" << std::endl;
    //std::cout << "t=" << raw_time/1000000000 << std::endl;
    //std::cout << "Full url = " << fullurl.str() << std::endl;

    //get new dataset
    int status = -1;
    if(fSQLitePath != "" && !fTestMode) {
      fCachedDataset  = GetSQLiteData(raw_time/1000000000);
    }
    else {
      if(fTestMode) {
	std::cout << "Accessing primary calibration data from http conditions database server." << std::endl;
	std::cout << "Folder = " << fFolderName << std::endl;
      }
      fCachedDataset = getDataWithTimeout(fullurl.str().c_str(), NULL, fMaximumTimeout, &err);
    }
    status = getHTTPstatus(fCachedDataset);
    //DumpDataset(fCachedDataset);

    //Can add some more queries here if we get http error 504
    /*if (status == 504) {
      //try again
    }*/

    if (status != 200) {
      std::string msg = "HTTP error from " + fullurl.str()+": status: " + std::to_string(status) + ": " + std::string(getHTTPmessage(fCachedDataset));
      throw WebError(msg);
    }

    // If test mode is selected, get comparison data.

    if(fTestMode) {
      if(fSQLitePath != "") {
	std::cout << "Accessing comparison data from sqlite database " << fSQLitePath << std::endl;
	Dataset compare1 = GetSQLiteData(raw_time/1000000000);
	CompareDataset(fCachedDataset, compare1);
      }
      if(fURL2 != "") {
	std::cout <<"Accessing comparison data from second database url." << std::endl;
	std::stringstream fullurl2;
	fullurl2 << fURL2 << "/data?f=" << fFolderName
		<< "&t=" << ts.DBStamp();
	if (fTag.length() > 0) fullurl2 << "&tag=" << fTag;
	std::cout << "Full url = " << fullurl2.str() << std::endl;
	Dataset compare2 = getDataWithTimeout(fullurl2.str().c_str(), NULL, fMaximumTimeout, &err);
	status = getHTTPstatus(compare2);
	if (status != 200) {
	  std::string msg = "HTTP error from " + fullurl2.str()+": status: " + std::to_string(status) + ": " + std::string(getHTTPmessage(compare2));
	  throw WebError(msg);
	}
	CompareDataset(fCachedDataset, compare2);
      }
    }

    
    //update info about cached data
    fNRows = getNtuples(fCachedDataset) - kNUMBER_HEADER_ROWS;
    //std::cout<<"Retrieved "<<fNRows<<" rows from "<<fullurl.str()<<std::endl;
    if (fNRows < 1) {
      std::stringstream msg;
      msg << "Time " << ts.DBStamp() << ": Data not found in database.";
      throw WebError(msg.str());
      fCachedStart = fCachedEnd = ts;
    }

    //start and end times
    Tuple tup;
    tup = getTuple(fCachedDataset, 0);
    char buf[kBUFFER_SIZE];
    getStringValue(tup,0, buf, kBUFFER_SIZE, &err);
    //std::cout << "IOV start time = " << buf << std::endl;
    fCachedStart = IOVTimeStamp::GetFromString(std::string(buf));
    //std::cout << "IOV start time converted = "
    //      << fCachedStart.Stamp() << "." << fCachedStart.SubStamp() << std::endl;
    releaseTuple(tup);

    tup = getTuple(fCachedDataset, 1);
    getStringValue(tup,0, buf, kBUFFER_SIZE, &err);
    if ( 0 == strcmp(buf,"-") ) {
      //std::cout << "IOV end time = max" << std::endl;
      fCachedEnd = IOVTimeStamp::MaxTimeStamp();
    }
    else {
      //std::cout << "IOV end time = " << buf << std::endl;
      fCachedEnd = IOVTimeStamp::GetFromString(std::string(buf));
      //std::cout << "IOV end time converted = "
      //	<< fCachedEnd.Stamp() << "." << fCachedEnd.SubStamp() << std::endl;
    }
    releaseTuple(tup);

    //column names
    tup = getTuple(fCachedDataset, 2);
    fColumns.clear();
    for (int c=0; c < getNfields(tup); ++c) {
      getStringValue(tup, c, buf, kBUFFER_SIZE, &err);
      //std::cout << "Column name = " << buf << std::endl;
      fColumns.push_back(buf);
    }
    releaseTuple(tup);

    //column types
    tup = getTuple(fCachedDataset, 3);
    fTypes.clear();
    for (int c=0; c < getNfields(tup); ++c) {
      getStringValue(tup, c, buf, kBUFFER_SIZE, &err);
      //std:: cout << "Type = " << buf << std::endl;
      fTypes.push_back(buf);
    }
    releaseTuple(tup);

    return true;
  }

  // Query data from sqlite database.
  // The return value of type Dataset (aka void*), is partially opaque type HttpResponse*
  // (defined in wda.c and copied above).

  Dataset DBFolder::GetSQLiteData(int t) const
  {
    if(fSQLitePath == "")
      return 0;

    // Result struct (allocate and initialize later).

    HttpResponse* result = 0;

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
	<< " AND " << table_iovs << ".active=1"
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
	<< " AND " << table_iovs << ".active=1"
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
    // We do this so that we know how much memory to allocate for fake http response.

    std::string table_data = fFolderName + "_data";
    sql.str("");
    sql << "SELECT COUNT(DISTINCT channel)"
	<< " FROM " << table_data << "," << table_iovs << "," << table_tag_iovs
	<< " WHERE " << table_tag_iovs << ".tag='" << fTag << "'"
	<< " AND " << table_iovs << ".iov_id=" << table_tag_iovs << ".iov_id"
	<< " AND " << table_data << ".__iov_id=" << table_tag_iovs << ".iov_id"
	<< " AND " << table_iovs << ".active=1"
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

    // Initialize fake HttpResponse.
    // Allocate space for data rows and four header rows.

    result = (HttpResponse*)malloc(sizeof(HttpResponse) + (nrows+3)*sizeof(DataRec *));
    if(result == 0) {
      std::cout << "Out of memory." << std::endl;
      throw cet::exception("DBFolder") << "Out of memory.";
    }
    result->memory = 0;
    result->size = 0;
    result->allocsize = 0;
    result->rows = 0;
    result->nrows = nrows + 4;
    result->idx = 0;
    result->http_code = 200;

    // Stash begin time in fake http response (row 0).

    DataRec *dataRec = (DataRec *)malloc(sizeof (DataRec));
    if(dataRec == 0) {
      std::cout << "Out of memory." << std::endl;
      throw cet::exception("DBFolder") << "Out of memory.";
    }
    memset(dataRec, 0, sizeof (DataRec));
    dataRec->ncolumns = 1;
    dataRec->columns = (char**)calloc(1, sizeof(char*));
    if(dataRec->columns == 0) {
      std::cout << "Out of memory." << std::endl;
      throw cet::exception("DBFolder") << "Out of memory.";
    }
    std::ostringstream ostr;
    ostr << begin_time << ".0";
    dataRec->columns[0] = strdup(ostr.str().c_str());
    if(dataRec->columns[0] == 0) {
      std::cout << "Out of memory." << std::endl;
      throw cet::exception("DBFolder") << "Out of memory.";
    }
    result->dataRecs[0] = dataRec;

    // Stash end time in fake http response (row 1).

    dataRec = (DataRec *)malloc(sizeof (DataRec));
    if(dataRec == 0) {
      std::cout << "Out of memory." << std::endl;
      throw cet::exception("DBFolder") << "Out of memory.";
    }
    memset(dataRec, 0, sizeof (DataRec));
    dataRec->ncolumns = 1;
    dataRec->columns = (char**)calloc(1, sizeof(char*));
    if(dataRec->columns == 0) {
      std::cout << "Out of memory." << std::endl;
      throw cet::exception("DBFolder") << "Out of memory.";
    }
    ostr.str("");
    if(end_time == 0)
      ostr << "-";
    else
      ostr << end_time << ".0";
    dataRec->columns[0] = strdup(ostr.str().c_str());
    if(dataRec->columns[0] == 0) {
      std::cout << "Out of memory." << std::endl;
      throw cet::exception("DBFolder") << "Out of memory.";
    }
    result->dataRecs[1] = dataRec;

    // Main data query.

    sql.str("");
    sql << "SELECT " << table_data << ".*,MAX(begin_time)"
	<< " FROM " << table_data << "," << table_iovs << "," << table_tag_iovs
	<< " WHERE " << table_tag_iovs << ".tag='" << fTag << "'"
	<< " AND " << table_iovs << ".iov_id=" << table_tag_iovs << ".iov_id"
	<< " AND " << table_data << ".__iov_id=" << table_tag_iovs << ".iov_id"
	<< " AND " << table_iovs << ".active=1"
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
    std::vector<std::string> column_names;
    std::vector<std::string> column_types;
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

    // Get the number of relevant columns.

    unsigned int nrcols = column_names.size();

    // Stash the names of columns in fake http response.

    dataRec = (DataRec *)malloc(sizeof (DataRec));
    if(dataRec == 0) {
      std::cout << "Out of memory." << std::endl;
      throw cet::exception("DBFolder") << "Out of memory.";
    }
    memset(dataRec, 0, sizeof (DataRec));
    dataRec->ncolumns = nrcols;
    dataRec->columns = (char**)calloc(nrcols, sizeof(char*));
    if(dataRec->columns == 0) {
      std::cout << "Out of memory." << std::endl;
      throw cet::exception("DBFolder") << "Out of memory.";
    }
    for(unsigned int icol=0; icol<nrcols; ++icol) {
      dataRec->columns[icol] = strdup(column_names[icol].c_str());
      if(dataRec->columns[icol] == 0) {
	std::cout << "Out of memory." << std::endl;
	throw cet::exception("DBFolder") << "Out of memory.";
      }
    }	  
    result->dataRecs[2] = dataRec;

    // Stash the types of columns in fake http response.

    dataRec = (DataRec *)malloc(sizeof (DataRec));
    if(dataRec == 0) {
      std::cout << "Out of memory." << std::endl;
      throw cet::exception("DBFolder") << "Out of memory.";
    }
    memset(dataRec, 0, sizeof (DataRec));
    dataRec->ncolumns = nrcols;
    dataRec->columns = (char**)calloc(nrcols, sizeof(char*));
    if(dataRec->columns == 0) {
      std::cout << "Out of memory." << std::endl;
      throw cet::exception("DBFolder") << "Out of memory.";
    }
    for(unsigned int icol=0; icol<nrcols; ++icol) {
      dataRec->columns[icol] = strdup(column_types[icol].c_str());
      if(dataRec->columns[icol] == 0) {
	std::cout << "Out of memory." << std::endl;
	throw cet::exception("DBFolder") << "Out of memory.";
      }
    }	  
    result->dataRecs[3] = dataRec;

    // Re-execute query.
    // Retrieve all data rows and stash in fake http response.

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

	// Add row to fake http response.

	if(nrows+4 > result->nrows) {
	  std::cout << "Too many data rows " << result->nrows << std::endl;
	  throw cet::exception("DBFolder") << "Too many data rows " << result->nrows;
	}
	dataRec = (DataRec *)malloc(sizeof (DataRec));
	if(dataRec == 0) {
	  std::cout << "Out of memory." << std::endl;
	  throw cet::exception("DBFolder") << "Out of memory.";
	}
	memset(dataRec, 0, sizeof (DataRec));
	dataRec->ncolumns = nrcols;
	dataRec->columns = (char**)calloc(nrcols, sizeof(char*));
	if(dataRec->columns == 0) {
	  std::cout << "Out of memory." << std::endl;
	  throw cet::exception("DBFolder") << "Out of memory.";
	}
	result->dataRecs[nrows+3] = dataRec;

	// Loop over columns.

	int rcol = 0;
	for(int col = 0; col < ncols; ++col) {
	  std::string colname = sqlite3_column_name(stmt, col);

	  // Ignore columns that begin with "_".
	  // Also ignore utility column MAX(begin_time).

	  if(colname[0] != '_' && colname.substr(0,3) != "MAX") {
	    int dtype = sqlite3_column_type(stmt, col);

	    if(dtype == SQLITE_INTEGER) {
	      int value = sqlite3_column_int(stmt, col);
	      //std::cout << "Value = " << value << std::endl;
	      std::ostringstream ostr;
	      ostr << value;
	      result->dataRecs[nrows+3]->columns[rcol] = strdup(ostr.str().c_str());
	      if(result->dataRecs[nrows+3]->columns[rcol] == 0) {
		std::cout << "Out of memory." << std::endl;
		throw cet::exception("DBFolder") << "Out of memory.";
	      }
	    }
	    else if(dtype == SQLITE_FLOAT) {
	      double value = sqlite3_column_double(stmt, col);
	      //std::cout << "Value = " << value << std::endl;	    
	      std::ostringstream ostr;
	      ostr << value;
	      result->dataRecs[nrows+3]->columns[rcol] = strdup(ostr.str().c_str());
	      if(result->dataRecs[nrows+3]->columns[rcol] == 0) {
		std::cout << "Out of memory." << std::endl;
		throw cet::exception("DBFolder") << "Out of memory.";
	      }
	    }
	    else if(dtype == SQLITE_TEXT) {
	      const char* s = (const char*)sqlite3_column_text(stmt, col);
	      //std::cout << "Value = " << s << std::endl;	    
	      result->dataRecs[nrows+3]->columns[rcol] = strdup(s);
	      if(result->dataRecs[nrows+3]->columns[rcol] == 0) {
		std::cout << "Out of memory." << std::endl;
		throw cet::exception("DBFolder") << "Out of memory.";
	      }
	    }
	    else if(dtype == SQLITE_NULL) {
	      //std::cout << "Value = NULL" << std::endl;	    
	    }
	    ++rcol;
	  }
	}
      }
      else if(rc != SQLITE_DONE) {
	std::cout << "sqlite3_step returned error result = " << rc << std::endl;
	throw cet::exception("DBFolder") << "sqlite3_step error.";
      }
    }
    if(nrows+4 != result->nrows) {
      std::cout << "Wrong number of data rows " << result->nrows << std::endl;
      throw cet::exception("DBFolder") << "Wrong number of data rows " << result->nrows;
    }

    // Delete statement.

    sqlite3_finalize(stmt);

    // Close database.

    sqlite3_close(db);

    // Done.

    return result;
  }

  // Dump dataset by rows.
  // Row 0 - IOV start time.
  // Row 1 - IOV end time.
  // Row 2 - Data column names.
  // Row 3 - Data column types.
  // Rows 4-N - Data.

  void DBFolder::DumpDataset(Dataset data) const
  {
    std::vector<std::string> names;
    std::vector<std::string> types;

    int nrows = getNtuples(data) - kNUMBER_HEADER_ROWS;
    std::cout << "Dataset contains " << nrows << " rows." << std::endl;

    // Row 0 - start time.
    int err = 0;
    Tuple tup;
    tup = getTuple(data, 0);
    char buf[kBUFFER_SIZE];
    getStringValue(tup, 0, buf, kBUFFER_SIZE, &err);
    std::cout << "IOV start time = " << buf << std::endl;
    releaseTuple(tup);

    // Row 1 - end time.

    tup = getTuple(data, 1);
    getStringValue(tup, 0, buf, kBUFFER_SIZE, &err);
    if ( 0 == strcmp(buf,"-") )
      std::cout << "IOV end time = max" << std::endl;
    else
      std::cout << "IOV end time = " << buf << std::endl;
    releaseTuple(tup);

    // Row 2 - column names.
    tup = getTuple(data, 2);
    for (int c=0; c < getNfields(tup); ++c) {
      getStringValue(tup, c, buf, kBUFFER_SIZE, &err);
      std::cout << "Column " << c << ", name = " << buf << std::endl;
      names.push_back(buf);
    }
    releaseTuple(tup);

    // Row 3 - column types.
    tup = getTuple(data, 3);
    for (int c=0; c < getNfields(tup); ++c) {
      getStringValue(tup, c, buf, kBUFFER_SIZE, &err);
      std::cout << "Column " << c << ", type = " << buf << std::endl;
      types.push_back(buf);
    }
    releaseTuple(tup);

    // Data rows.
    for(int row = 0; row < nrows; ++row) {
      std::cout << "\nRow " << row << std::endl;
      tup = getTuple(data, row + kNUMBER_HEADER_ROWS);

      // Loop over columns.

      for(unsigned int col = 0; col < names.size(); ++col) {
	if(types[col] == "bigint" || types[col] == "integer") {
	  long value = getLongValue(tup, col, &err);
	  std::cout << names[col] << " = " << value << std::endl;
	}
	else if(types[col] == "real") {
	  double value = getDoubleValue(tup, col, &err);
	  std::cout << names[col] << " = " << value << std::endl;
	}
	else if(types[col] == "text" or types[col] == "boolean") {
	  char buf[kBUFFER_SIZE];
	  getStringValue(tup, col, buf, kBUFFER_SIZE, &err);
	  std::cout << names[col] << " = " << buf << std::endl;
	}
	else {
	  std::cout << "Unknown type " << types[col] << std::endl;
	  throw cet::exception("DBFolder") << "Unknown type.";
	}
      }
      releaseTuple(tup);
    }
  }

  void DBFolder::CompareDataset(Dataset data1, Dataset data2) const
  {
    bool compare_ok = true;
    std::cout << "\nComparing datasets." << std::endl;
    std::vector<std::string> names1;
    std::vector<std::string> types1;
    std::vector<std::string> names2;
    std::vector<std::string> types2;

    int nrows1 = getNtuples(data1) - kNUMBER_HEADER_ROWS;
    int nrows2 = getNtuples(data2) - kNUMBER_HEADER_ROWS;
    //std::cout << "Dataset 1 contains " << nrows1 << " rows." << std::endl;
    //std::cout << "Dataset 2 contains " << nrows2 << " rows." << std::endl;
    if(nrows1 != nrows2)
      compare_ok = false;

    // Row 0 - start time.
    int err = 0;
    Tuple tup1, tup2;
    tup1 = getTuple(data1, 0);
    tup2 = getTuple(data2, 0);
    char buf1[kBUFFER_SIZE];
    char buf2[kBUFFER_SIZE];
    getStringValue(tup1, 0, buf1, kBUFFER_SIZE, &err);
    getStringValue(tup1, 0, buf2, kBUFFER_SIZE, &err);
    //std::cout << "IOV start time 1 = " << buf1 << std::endl;
    //std::cout << "IOV start time 2 = " << buf2 << std::endl;
    if(strcmp(buf1, buf2) != 0)
      compare_ok = false;
    releaseTuple(tup1);
    releaseTuple(tup2);

    // Row 1 - end time.

    tup1 = getTuple(data1, 1);
    tup2 = getTuple(data2, 1);
    getStringValue(tup1, 0, buf1, kBUFFER_SIZE, &err);
    getStringValue(tup2, 0, buf2, kBUFFER_SIZE, &err);
    //if ( 0 == strcmp(buf1,"-") )
    //std::cout << "IOV end time 1 = max" << std::endl;
    //else
    //std::cout << "IOV end time 1 = " << buf1 << std::endl;
    //if ( 0 == strcmp(buf2,"-") )
    //std::cout << "IOV end time 2 = max" << std::endl;
    //else
    //std::cout << "IOV end time 2 = " << buf2 << std::endl;
    if(strcmp(buf1, buf2) != 0)
      compare_ok = false;
    releaseTuple(tup1);
    releaseTuple(tup2);

    // Row 2 - column names.
    tup1 = getTuple(data1, 2);
    tup2 = getTuple(data2, 2);
    for (int c=0; c < getNfields(tup1); ++c) {
      getStringValue(tup1, c, buf1, kBUFFER_SIZE, &err);
      //std::cout << "Column " << c << ", name 1 = " << buf1 << std::endl;
      names1.push_back(buf1);
    }
    for (int c=0; c < getNfields(tup2); ++c) {
      getStringValue(tup2, c, buf2, kBUFFER_SIZE, &err);
      //std::cout << "Column " << c << ", name 2 = " << buf2 << std::endl;
      names2.push_back(buf2);
      if(c >= int(names1.size()) || names1[c] != names2.back()) {
	std::cout << "Name mismatch " << names1[c] << " vs. " << names2.back() << std::endl;
	compare_ok = false;
      }
    }
    if(names1.size() != names2.size()) {
      std::cout << "Size mismatch " << names1.size() << " vs. " << names2.size() << std::endl;
      compare_ok = false;
    }
    releaseTuple(tup1);
    releaseTuple(tup2);

    // Row 3 - column types.
    tup1 = getTuple(data1, 3);
    tup2 = getTuple(data2, 3);
    for (int c=0; c < getNfields(tup1); ++c) {
      getStringValue(tup1, c, buf1, kBUFFER_SIZE, &err);
      //std::cout << "Column " << c << ", type 1 = " << buf1 << std::endl;
      types1.push_back(buf1);
      if(types1.back() == "bigint")
	types1.back() = "integer";
    }
    for (int c=0; c < getNfields(tup2); ++c) {
      getStringValue(tup2, c, buf2, kBUFFER_SIZE, &err);
      //std::cout << "Column " << c << ", type 2 = " << buf2 << std::endl;
      types2.push_back(buf2);
      if(types2.back() == "bigint")
	types2.back() = "integer";
      if(c >= int(types1.size()) || types1[c] != types2.back()) {
	if(types1[c] != "boolean" || types2.back() != "integer") {
	  std::cout << "Type mismatch " << types1[c] << " vs. " << types2.back() << std::endl;
	  compare_ok = false;
	}
      }
    }
    if(types1.size() != types2.size()) {
      std::cout << "Size mismatch " << types1.size() << " vs. " << types2.size() << std::endl;
      compare_ok = false;
    }
    releaseTuple(tup1);
    releaseTuple(tup2);

    // Data rows.
    for(int row = 0; row < nrows1; ++row) {
      //std::cout << "\nRow " << row << std::endl;
      tup1 = getTuple(data1, row + kNUMBER_HEADER_ROWS);
      tup2 = getTuple(data2, row + kNUMBER_HEADER_ROWS);

      // Loop over columns.

      for(unsigned int col = 0; col < names1.size(); ++col) {
	if(types1[col] == "integer") {
	  long value1 = getLongValue(tup1, col, &err);
	  long value2 = getLongValue(tup2, col, &err);
	  //std::cout << names1[col] << " 1 = " << value1 << std::endl;
	  //std::cout << names2[col] << " 2 = " << value2 << std::endl;
	  if(value1 != value2) {
	    std::cout << "Value mismatch " << value1 << " vs. " << value2 << std::endl;
	    compare_ok = false;
	  }
	}
	else if(types1[col] == "real") {
	  double value1 = getDoubleValue(tup1, col, &err);
	  double value2 = getDoubleValue(tup2, col, &err);
	  //std::cout << names1[col] << " 1 = " << value1 << std::endl;
	  //std::cout << names2[col] << " 2 = " << value2 << std::endl;
	  if(value1 != value2) {
	    std::cout << "Value mismatch " << value1 << " vs. " << value2 << std::endl;
	    compare_ok = false;
	  }
	}
	else if(types1[col] == "text" or types1[col] == "boolean") {
	  char buf1[kBUFFER_SIZE];
	  char buf2[kBUFFER_SIZE];
	  getStringValue(tup1, col, buf1, kBUFFER_SIZE, &err);
	  getStringValue(tup2, col, buf2, kBUFFER_SIZE, &err);
	  //std::cout << names1[col] << " 1 = " << buf1 << std::endl;
	  //std::cout << names2[col] << " 2 = " << buf2 << std::endl;
	  if(strcmp(buf1, buf2) != 0) {
	    bool never_mind = false;
	    if(strcmp(buf1, "True") == 0 && strcmp(buf2, "1") == 0)
	      never_mind = true;
	    if(strcmp(buf1, "False") == 0 && strcmp(buf2, "0") == 0)
	      never_mind = true;
	    if(!never_mind) {
	      std::cout << "Value mismatch " << buf1 << " vs. " << buf2 << std::endl;
	      compare_ok = false;
	    }
	  }
	}
	else {
	  std::cout << "Unknown type " << types1[col] << std::endl;
	  throw cet::exception("DBFolder") << "Unknown type.";
	}
      }
      releaseTuple(tup1);
      releaseTuple(tup2);
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
