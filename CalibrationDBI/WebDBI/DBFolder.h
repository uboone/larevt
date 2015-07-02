#ifndef DBFOLDER_H
#define DBFOLDER_H

#include "CalibrationDBI/IOVData/IOVTimeStamp.h"
#include <string>
#include <vector>
#include <memory>

namespace lariov {

  typedef void *Dataset;
  typedef void *Tuple;

  class DBFolder {
  
    public:
      DBFolder(const std::string& name, const std::string& url, const std::string& tag = "");
      virtual ~DBFolder();
      
      int GetNamedChannelData(std::uint64_t channel, const std::string& name, long& data);
      int GetNamedChannelData(std::uint64_t channel, const std::string& name, double& data);
      int GetNamedChannelData(std::uint64_t channel, const std::string& name, std::string& data);
 
      const std::string& URL() const {return fURL;}
      const std::string& FolderName() const {return fFolderName;}
      const std::string& Tag() const {return fTag;}
      
      const IOVTimeStamp& CachedStart() const {return fCachedStart;}
      const IOVTimeStamp& CachedEnd() const   {return fCachedEnd;}
      
      bool UpdateData(std::uint64_t raw_time);
      
      int GetChannelList( std::vector<std::uint64_t>& channels ) const;
     
    private:  
      size_t GetTupleColumn( std::uint64_t channel, const std::string& name, Tuple& tup );
      
      bool IsValid(const IOVTimeStamp& time) const {
        if (time >= fCachedStart && time < fCachedEnd) return true;
	else return false;
      }
      
           
      std::string fURL;
      std::string fFolderName;
      std::string fTag;
      
      Dataset                  fCachedDataset;   
      int                      fNRows;         //Number of channels in cached dataset
      IOVTimeStamp               fCachedStart;
      IOVTimeStamp               fCachedEnd;
      std::vector<std::string> fColumns;       //Column names
      std::vector<std::string> fTypes;         //Column types
      int                      fCachedRow;     //Cache most recently retrieved row and channel numbers
      std::uint64_t            fCachedChannel;      
  };
}

#endif
