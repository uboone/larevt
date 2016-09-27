/**
 * \file ElectronLifetimeProvider
 * 
 * \brief Class def header for a class ElectronLifetimeProvider
 *
 * @author eberly@slac.stanford.edu
 */

#ifndef ELECTRONLIFETIMEPROVIDER_H
#define ELECTRONLIFETIMEPROVIDER_H


namespace lariov {

  /**
     \class ElectronLifetimeProvider
     Pure abstract interface class for retrieving electron lifetimes.
     
  */
  class ElectronLifetimeProvider {
  
    public:
    
      virtual ~ElectronLifetimeProvider() = default;
       
      /// Retrieve pedestal information     
      virtual float Lifetime() const = 0;
      virtual float Purity() const = 0;
      virtual float LifetimeErr() const = 0;
      virtual float PurityErr() const = 0;
      
 
  };
}//end namespace lariov

#endif
