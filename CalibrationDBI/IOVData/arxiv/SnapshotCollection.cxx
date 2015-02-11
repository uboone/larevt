#ifndef SNAPSHOTCOLLECTION_CXX
#define SNAPSHOTCOLLECTION_CXX

#include "SnapshotCollection.h"

namespace lariov {

  template <class T>
  SnapshotCollection<T>::SnapshotCollection(const std::string folder)
    : _folder (folder)
  {}

  template <class T>
  void SnapshotCollection<T>::Write(TTree& t) const
  {
    if(!_snapshot_v.size()) {
      std::cout<<"\033[93m[WARNING]\033[00m Nothing to write..."<<std::endl;
      return;
    }

    std::string tree_name(kTREE_PREFIX);
    tree_name += "_" + _folder;
    if(t.GetName() != tree_name) {
      std::cout<<"\033[93m[WARNING]\033[00m re-naming TTree to "<<tree_name<<std::endl;
    }
    t.SetName(tree_name.c_str());

    if(t.GetBranch(_folder.c_str())) {
      std::cout<<"\033[95m[ERROR]\033[00m folder "
	       <<_folder.c_str()
	       <<" already exists. Skip storing."<<std::endl;
      return;
    }

    Snapshot<T>* data_ptr = new Snapshot<T>;
    
    t.Branch(_folder.c_str(),
	     _snapshot_v.front().GetName(),
	     &data_ptr);

    for(auto const& ss : _snapshot_v) {

      *data_ptr = ss;
      
      t.Fill();

    }

    delete data_ptr;

    std::cout<<"\033[92m[INFO]\033[00m Written a folder "
	     <<_folder.c_str()
	     <<" with "
	     <<t.GetEntries()
	     <<" entries."
	     << std::endl;
  }

  template <class T>
  void SnapshotCollection<T>::Read(TFile& f)
  {
    std::string tree_name(kTREE_PREFIX);
    tree_name += "_" + _folder;
    auto t = (TTree*)(f.Get(tree_name.c_str()));
    if(!t)
      throw IOVDataError("Tree not found...");

    if(!(t->GetBranch(_folder.c_str())))
      throw IOVDataError("Folder not found...");

    _snapshot_v.clear();

    Snapshot<T>* data_ptr = new Snapshot<T>;
    t->SetBranchAddress(_folder.c_str(),&data_ptr);
    for(int i=0; i<t->GetEntries(); ++i){
      t->GetEntry(i);
      this->Append(*data_ptr);
    }

    std::cout<<"\033[92m[INFO]\033[00m Read a folder "
	     <<_folder
	     <<" with "
	     <<_snapshot_v.size()
	     <<" entries."
	     << std::endl;
  }

}
#endif
