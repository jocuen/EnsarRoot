// -------------------------------------------------------------------------
// -----                E105AsciiGenerator source file                 -----
// -------------------------------------------------------------------------
#include "E105AsciiGenerator.h"

#include "FairPrimaryGenerator.h"
#include "FairIon.h"
#include "FairRunSim.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TRandom.h"

#include <iostream>

using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   ------------------------------------------
E105AsciiGenerator::E105AsciiGenerator() {}
// ------------------------------------------------------------------------



// -----   Standard constructor   -----------------------------------------
E105AsciiGenerator::E105AsciiGenerator(const char* fileName) {

  fPDG=TDatabasePDG::Instance();
  fFileName  = fileName;
  cout << "-I- E105AsciiGenerator: Opening input file " << fileName << endl;

  // Open first the file to register all new ions.
  fInputFile = new ifstream(fFileName);
  if ( ! fInputFile->is_open() ) 
    Fatal("E105AsciiGenerator","Cannot open input file.");
  cout << "-I- E105AsciiGenerator: Looking for ions..." << endl;

  Int_t nIons = RegisterIons();
  cout << "-I- E105AsciiGenerator: " << nIons << " ions registered." 
       << endl;
  CloseInput();

  // Re-Open the file for standard streaming ...
  fInputFile = new ifstream(fFileName);

  fX=fY=fZ=0.0;
  fPointVtxIsSet=kFALSE;
  fDX=fDY=fDZ=0.0;
  fBoxVtxIsSet=kFALSE;
 
}
// ------------------------------------------------------------------------



// -----   Destructor   ---------------------------------------------------
E105AsciiGenerator::~E105AsciiGenerator() {
  CloseInput();
}
// ------------------------------------------------------------------------



// -----   Public method ReadEvent   --------------------------------------
Bool_t E105AsciiGenerator::ReadEvent(FairPrimaryGenerator* primGen) {

  // Check for input file
  if ( ! fInputFile->is_open() ) {
    cout << "-E- E105AsciiGenerator: Input file not open!" << endl;
    return kFALSE;
  }

  // Define event variable to be read from file
  Int_t    eventId = 0;
  Int_t    nTracks = 0;
  Double_t pBeam   = 0.;
  Double_t b       = 0.;


  // Define track variables to be read from file
  Int_t    iPid   = -1;
  Int_t    iA      = 0;
  Int_t    iZ      = 0;
  Double_t px      = 0.;
  Double_t py      = 0.;
  Double_t pz      = 0.;
  Double_t vx      = 0.;
  Double_t vy      = 0.;
  Double_t vz      = 0.;
  Double_t iMass      = 0.;


  // Read event header line from input file
  *fInputFile >> eventId >> nTracks >> pBeam >> b;

  if ( fInputFile->eof() ) {
      cout << "-I- E105AsciiGenerator: End of input file reached " << endl;
      CloseInput();
      return kFALSE;
  }

  cout << "-I- E105AsciiGenerator: Reading Event: " << eventId << ",  pBeam = "
      << pBeam << "GeV, b = " << b << " fm, multiplicity " << nTracks
      << endl;

  // Loop over tracks in the current event
  for (Int_t itrack=0; itrack<nTracks; itrack++) {

      *fInputFile >> iPid  >> iZ >> iA >> px >> py >> pz >> vx >> vy >> vz >> iMass;
     /*
      cout << "-I- E105AsciiGenerator: iPid: " << iPid <<
	  ",   A = " << iA << " Z = " << iZ <<
	  " px = "  << px <<
	  " py = "  << py <<
	  " pz = "  << pz <<
	  " vx = "  << vx <<
	  " vy = "  << vy <<
	  " vz = " << vz << endl;
      */

      Int_t pdgType=0;

      // Ion case ( iPid = -1 )
      if ( iPid < 0 ) {
	  char ionName[20];
	  sprintf(ionName, "Ion_%d_%d", iA, iZ);
	  TParticlePDG* part = fPDG->GetParticle(ionName);
	  if ( ! part ) {
	      cout << "-W- E105AsciiGenerator::ReadEvent: Cannot find "
		  << ionName << " in database!" << endl;
	      continue;
	  }
	  pdgType = part->PdgCode();
      }
      else pdgType = iA;  // "normal" particle

      // Give track to PrimaryGenerator
      //cout << "PDG : " << pdgType << endl;

      if (fPointVtxIsSet){ 
	vx = fX;
	vy = fY;
	vz = fZ;
	if (fBoxVtxIsSet) {
	  vx = gRandom->Gaus(fX,fDX);
	  vy = gRandom->Gaus(fY,fDY); 
	  vz = gRandom->Gaus(fZ,fDZ); 
	}         	
      }

      primGen->AddTrack(pdgType, px, py, pz, vx, vy, vz);

  }//! tracks

  return kTRUE;
}
// ------------------------------------------------------------------------
// -----   Private method CloseInput   ------------------------------------
void E105AsciiGenerator::CloseInput() {
  if ( fInputFile ) {
    if ( fInputFile->is_open() ) {
       cout << "-I- E105AsciiGenerator: Closing input file " 
	    << fFileName << endl;
       fInputFile->close();
    }
    delete fInputFile;
    fInputFile = NULL;
  }
}
// ------------------------------------------------------------------------

// -----   Private method RegisterIons   ----------------------------------
Int_t E105AsciiGenerator::RegisterIons() {

  Int_t nIons = 0;
  Int_t eventId, nTracks;
  Double_t pBeam,b;

  // Define track variables to be read from file
  Int_t    iPid   = -1;
  Int_t    iA      = 0;
  Int_t    iZ      = 0;
  Double_t px      = 0.;
  Double_t py      = 0.;
  Double_t pz      = 0.;
  Double_t vx      = 0.;
  Double_t vy      = 0.;
  Double_t vz      = 0.;
  Double_t iMass      = 0.;

  fIonMap.clear();

  while ( ! fInputFile->eof()) {
    *fInputFile >> eventId >> nTracks >> pBeam >> b;

    if ( fInputFile->eof() ) continue;

    for (Int_t iTrack=0; iTrack<nTracks; iTrack++) {
      *fInputFile >> iPid >> iZ >> iA >> px >> py >> pz >> vx >> vy >> vz >> iMass;

      // Ion Case
      if ( iPid < 0 ) {
	char buffer[20];
	sprintf(buffer, "Ion_%d_%d", iA, iZ);
	TString ionName(buffer);
	if ( fIonMap.find(ionName) == fIonMap.end() ) {
	  //FairIon* ion = new FairIon(ionName, iZ, iA, iZ);
	  FairIon* ion = new FairIon(ionName, iZ, iA, iZ, 0., iMass);
	  fIonMap[ionName] = ion;
	  nIons++;
	}  // new ion
      } // ion
    }// !tracks

  }//!events

  FairRunSim* run = FairRunSim::Instance();
  map<TString, FairIon*>::iterator mapIt;
  for (mapIt=fIonMap.begin(); mapIt!=fIonMap.end(); mapIt++) {
    FairIon* ion = (*mapIt).second;
    run->AddNewIon(ion);
  }

  return nIons;
}
// ------------------------------------------------------------------------

ClassImp(E105AsciiGenerator)

