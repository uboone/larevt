#include "DBFolder.h"
#include "WebDBIConstants.h"
#include "larevt/CalibrationDBI/IOVData/TimeStampDecoder.h"
#include "WebError.h"

#include <sstream>
#include <stdlib.h>
#include <cstring>
#include "wda.h"

namespace lariov {

  typedef struct {
      size_t ncolumns;    // Number of columns in CSV row
      size_t nelements;   // Number of elements in data array
      char **columns;     // Pointers to columns
  } DataRec;

  DBFolder::DBFolder(const std::string& name, const std::string& url, const std::string& tag /*= ""*/) :
    fCachedStart(0,0), fCachedEnd(0,0) {

    fFolderName = name;
    fURL = url;
    fTag = tag;
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
    data = false;
    if (std::string(buf, str_size)=="True") {
      data = true;
    }
    else if (std::string(buf, str_size)=="False") {
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

    //get new dataset
    int status = -1;
    fCachedDataset = getDataWithTimeout(fullurl.str().c_str(), NULL, fMaximumTimeout, &err);
    status = getHTTPstatus(fCachedDataset);

    //Can add some more queries here if we get http error 504
    /*if (status == 504) {
      //try again
    }*/

    if (status != 200) {
      std::string msg = "HTTP error from " + fullurl.str()+": status: " + std::to_string(status) + ": " + std::string(getHTTPmessage(fCachedDataset));
      throw WebError(msg);
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
    fCachedStart = IOVTimeStamp::GetFromString(std::string(buf));
    releaseTuple(tup);

    tup = getTuple(fCachedDataset, 1);
    getStringValue(tup,0, buf, kBUFFER_SIZE, &err);
    if ( 0 == strcmp(buf,"-") ) {
      fCachedEnd = IOVTimeStamp::MaxTimeStamp();
    }
    else {
      fCachedEnd = IOVTimeStamp::GetFromString(std::string(buf));
    }
    releaseTuple(tup);

    //column names
    tup = getTuple(fCachedDataset, 2);
    fColumns.clear();
    for (int c=0; c < getNfields(tup); ++c) {
      getStringValue(tup, c, buf, kBUFFER_SIZE, &err);
      fColumns.push_back(buf);
    }
    releaseTuple(tup);

    //column types
    tup = getTuple(fCachedDataset, 3);
    fTypes.clear();
    for (int c=0; c < getNfields(tup); ++c) {
      getStringValue(tup, c, buf, kBUFFER_SIZE, &err);
      fTypes.push_back(buf);
    }
    releaseTuple(tup);

    return true;
  }

}//end namespace lariov
