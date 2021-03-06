// -------------------------------------------------------------------------
// -----                  TraRPCHit header file                -----
// -----	     Last modification 18/10/14 by H. Alvarez        -----
// -------------------------------------------------------------------------

/**  TraRPCHit.h
 **  A RPC hit is the representation of the 
 **  information obtained from each RPC.
 **/

#ifndef TraRPCHit_H
#define TraRPCHit_H

#include "TObject.h"
#include "FairMultiLinkedData.h"

class TraRPCHit : public FairMultiLinkedData 
{
public:

  /** Default constructor **/
  TraRPCHit();

  /** Constructor with arguments
   *@param fRPCId       RPC unique identifier
   *@param fCharge      Total charge deposited on the RPC ([GeV] in sim)
   *@param fTime        Time since event start [ns]
   **/
  TraRPCHit(Int_t id, Double_t charge, Double_t time);

  /** Copy constructor **/
  TraRPCHit(const TraRPCHit&);

  TraRPCHit& operator=(const TraRPCHit&) { return *this; }

  /** Destructor **/
  virtual ~TraRPCHit();

  /** Accessors **/
  Int_t    GetRPCId()         const { return fRPCId; }
  Double_t GetCharge()        const { return fCharge; }
  Double_t GetTime()          const { return fTime; }
	
  /** Modifiers **/
  void SetRPCId(Int_t id) { fRPCId = id; }
  void SetCharge(Double32_t charge){fCharge = charge;}
  void AddCharge(Double32_t charge){fCharge += charge;}
  void SetTime(Double32_t time){fTime = time;}

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

protected:
  // Hit: Fired cell with a charge value bigger than a given threshold.
  // fRPCId decodes the plane number (0 to 3, 3 bits PPP), 
  // row (X direction, 0 to 11 in 4 bits RRRR),
  // column (Y direction, 0 to 9, 4 bits TTTT) 
  // acording to 0PPP00RRRR00TTTT

  Double32_t fCharge;   //total charge in the RPC
  Double32_t fTime;    //TDC time 
  Int_t fRPCId;        //RPC unique identifier

  ClassDef(TraRPCHit,1)
};

#endif
