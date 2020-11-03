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
{}

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

      // Convert string value to DBDataset::value_type (std::variant).

      if(fColTypes[col] == "integer" || fColTypes[col] == "bigint") {
	long value = strtol(buf, 0, 10);
	fData.push_back(value_type(value));
	//std::cout << "DBDataset: row=" << row << ", column=" << col << ", value=" << fData.back() 
	//	<< std::endl;
	if(col == 0) {
	  fChannels.push_back(value);
	  //std::cout << "DBDataset: channel=" << fChannels.back() << std::endl;
	}
      }
      else if(fColTypes[col] == "real") {
	double value = strtod(buf, 0);
	fData.push_back(value_type(value));
	//std::cout << "DBDataset: row=" << row << ", column=" << col << ", value=" << fData.back() 
	//	<< std::endl;
	if(col == 0) {
	  std::cout << "First column has wrong type real." << std::endl;
	  throw cet::exception("DBDataset") << "First column has wrong type real.";
	}	
      }
      else if(fColTypes[col] == "text") {
	fData.emplace_back(std::make_unique<std::string>(buf));
	//std::cout << "DBDataset: row=" << row << ", column=" << col << ", value=" << fData.back() 
	//	<< std::endl;
	if(col == 0) {
	  std::cout << "First column has wrong type text." << std::endl;
	  throw cet::exception("DBDataset") << "First column has wrong type text.";
	}	
      }
      else if(fColTypes[col] == "boolean") {
	long value = 0;
	std::string s = std::string(buf);
	if(s == "true" || s == "True" || s == "TRUE" || s == "1")
	  value = 1;
	else if(s == "false" || s == "False" || s == "FALSE" || s == "0")
	  value = 0;
	else {
	  std::cout << "Unknown string representation of boolean " << s << std::endl;
	  throw cet::exception("DBDataset") << "Unknown string representation of boolean " << s
					    << std::endl;
	}
	fData.push_back(value_type(value));
	//std::cout << "DBDataset: row=" << row << ", column=" << col << ", value=" << fData.back() 
	//	<< std::endl;
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
