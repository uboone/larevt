//=================================================================================
//
// Name: DBDataset.cpp
//
// Purpose: Implementation for class DBDataset.
//
// Created: 26-Oct-2020 - H. Greenlee
//
//=================================================================================

#include <iostream>
#include <cstring>
#include "WebDBIConstants.h"
#include "DBDataset.h"
#include "wda.h"
#include "cetlib_except/exception.h"

// Default constructor.

lariov::DBDataset::DBDataset() :
  fBeginTime(0, 0),
  fEndTime(0, 0),
  fNRows(0),
  fNCols(0)
{}


// Initializing constructor.

lariov::DBDataset::DBDataset(void* dataset, bool release) :
  fBeginTime(0, 0),
  fEndTime(0, 0)
{
  update(dataset, release);
}

// Destructor.

lariov::DBDataset::~DBDataset()
{

  // Loop over rows and columns and delete text strings.

  for(size_t row = 0; row < fNRows; ++row) {
    for(size_t col = 0; col < fNCols; ++col) {
      if(fColTypes[col] == "text") {
	delete[] fData[row*fNCols + col].charValue;
      }
    }
  }
}

// Get row number by channel number.
// Return -1 if not found.

int lariov::DBDataset::getRowNumber(DBChannelID_t ch) const
{
  int result = -1;

  // Do a binary search on channel numbers.

  int low = 0;
  int high = fNRows - 1;
  while(high-low > 1) {
    int mid = (low + high) / 2;
    if(fChannels[mid] < ch)
      low = mid;
    else if(fChannels[mid] > ch)
      high = mid;
    else {

      // Found an exact match.  Break out of loop.

      result = mid;
      break;
    }
  }

  // If we fell out of loop without finding a match...

  if(result < 0) {
    if(fChannels[low] == ch)
      result = low;
    else if(fChannels[high] == ch)
      result = high;
  }

  // Done.

  return result;
}

// Get column number by column name.
// Return -1 if not found.

int lariov::DBDataset::getColNumber(const std::string& name) const
{
  int result = -1;

  // Scan column names.

  for(size_t i=0; i<fNCols; ++i) {
    if(fColNames[i] == name) {
      result = i;
      break;
    }
  }

  return result;
}

// Deinitialize.

void lariov::DBDataset::clear()
{
  fBeginTime = IOVTimeStamp(0, 0);
  fEndTime = IOVTimeStamp(0, 0);
  fNRows = 0;
  fNCols = 0;
  fColNames.clear();
  fColTypes.clear();
  fChannels.clear();
  fData.clear();
}

// Reinitialize.

void lariov::DBDataset::update(void* dataset, bool release)
{

  // Parse dataset and get number of rows.

  clear();
  fNRows = getNtuples(dataset) - kNUMBER_HEADER_ROWS;
  //std::cout << "DBDataset: Number of rows = " << fNRows << std::endl;
  fChannels.reserve(fNRows);

  // Process header rows.

  int err = 0;
  char buf[kBUFFER_SIZE];
  Tuple tup;

  // Extract IOV begin time.

  tup = getTuple(dataset, 0);
  getStringValue(tup, 0, buf, kBUFFER_SIZE, &err);
  fBeginTime = IOVTimeStamp::GetFromString(std::string(buf));
  //std::cout << "DBDataset: Begin time stamp input = " << buf << std::endl;
  //std::cout << "DBDataset: Begin time stamp = " << fBeginTime.DBStamp() << std::endl;
  releaseTuple(tup);

  // Extract IOV end time.
  
  tup = getTuple(dataset, 1);
  getStringValue(tup, 0, buf, kBUFFER_SIZE, &err);
  if ( 0 == strcmp(buf,"-") )
    fEndTime = IOVTimeStamp::MaxTimeStamp();
  else
    fEndTime = IOVTimeStamp::GetFromString(std::string(buf));
  //std::cout << "DBDataset: End time stamp input = " << buf << std::endl;
  //std::cout << "DBDataset: End time stamp = " << fEndTime.DBStamp() << std::endl;
  releaseTuple(tup);

  // Extract column names.

  tup = getTuple(dataset, 2);
  fNCols = getNfields(tup);
  //std::cout << "DBDataset: Number of columns = " << fNCols << std::endl;
  fColNames.reserve(fNCols);
  for (size_t col=0; col < fNCols; ++col) {
    getStringValue(tup, col, buf, kBUFFER_SIZE, &err);
    //std::cout << "DBDataset: Column name = " << buf << std::endl;
    fColNames.push_back(std::string(buf));
  }
  releaseTuple(tup);

  // Extract column types.

  tup = getTuple(dataset, 3);
  fColTypes.reserve(fNCols);
  for (size_t col=0; col < fNCols; ++col) {
    getStringValue(tup, col, buf, kBUFFER_SIZE, &err);
    //std:: cout << "DBDataset: Column type = " << buf << std::endl;
    fColTypes.push_back(std::string(buf));
  }
  releaseTuple(tup);

  // Extract data.  Loop over rows.

  fData.reserve(fNRows * fNCols);
  for(size_t row = 0; row < fNRows; ++row) {
    //std::cout << "\nRow " << row << std::endl;
    tup = getTuple(dataset, row + kNUMBER_HEADER_ROWS);

    // Loop over columns.

    for(size_t col = 0; col < fNCols; ++col) {
      getStringValue(tup, col, buf, kBUFFER_SIZE, &err);
      fData.emplace_back(DBDataset::value_type());

      // Convert string value to DBDataset::value_type (std::variant).

      if(fColTypes[col] == "integer" || fColTypes[col] == "bigint") {
	fData.back().longValue = strtol(buf, 0, 10);
	//std::cout << "DBDataset: row=" << row << ", column=" << col << ", value="
	// << fData.back().longValue << std::endl;
	if(col == 0) {
	  fChannels.push_back(fData.back().longValue);
	  //std::cout << "DBDataset: channel=" << fChannels.back() << std::endl;
	}
      }
      else if(fColTypes[col] == "real") {
	fData.back().doubleValue = strtod(buf, 0);
	//std::cout << "DBDataset: row=" << row << ", column=" << col << ", value="
	// << fData.back().doubleValue << std::endl;
	if(col == 0) {
	  std::cout << "First column has wrong type real." << std::endl;
	  throw cet::exception("DBDataset") << "First column has wrong type real.";
	}	
      }
      else if(fColTypes[col] == "text") {
	size_t nch = strlen(buf) + 1;
	fData.back().charValue = new char[nch];
	memcpy(fData.back().charValue, buf, nch);
	//std::cout << "DBDataset: row=" << row << ", column=" << col << ", value="
	// << fData.back().charValue << std::endl;
	if(col == 0) {
	  std::cout << "First column has wrong type text." << std::endl;
	  throw cet::exception("DBDataset") << "First column has wrong type text.";
	}	
      }
      else if(fColTypes[col] == "boolean") {
	std::string s = std::string(buf);
	if(s == "true" || s == "True" || s == "TRUE" || s == "1")
	  fData.back().longValue = 1;
	else if(s == "false" || s == "False" || s == "FALSE" || s == "0")
	  fData.back().longValue = 0;
	else {
	  std::cout << "Unknown string representation of boolean " << s << std::endl;
	  throw cet::exception("DBDataset") << "Unknown string representation of boolean " << s
					    << std::endl;
	}
	//std::cout << "DBDataset: row=" << row << ", column=" << col << ", value="
	// << fData.back().longValue << std::endl;
	if(col == 0) {
	  std::cout << "First column has wrong type boolean." << std::endl;
	  throw cet::exception("DBDataset") << "First column has wrong type boolean.";
	}	
      }
      else {
	std::cout << "Unknown datatype = " << fColTypes[col] << std::endl;
	throw cet::exception("DBDataset") << "Unknown datatype = " << fColTypes[col] 
					  << ": " << buf << std::endl;
      }
    }
    releaseTuple(tup);
  }

  // Maybe release dataset memory.

  if(release)
    releaseDataset(dataset);
}
