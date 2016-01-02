#include "RunAction.hh"
#include "Run.hh"

#include <fstream>
#include <iomanip>

#include "G4THitsMap.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4RunManager.hh"

RunAction* RunAction::_instance = nullptr;

DicomRunAction* DicomRunAction::Instance()
{
    return _instance;
}

RunAction::RunAction():
    G4UserRunAction(),
    _run{nullptr},
    fFieldValue{14}
{
    _SDName.push_back(std::string("phantomSD"));
    _instance = this;
}

RunAction::~RunAction()
{
    _SDName.clear();
    _instance = nullptr;
}

G4Run* RunAction::GenerateRun()
{
    // Generate new RUN object, which is specially
    // dedicated for MultiFunctionalDetector scheme.
    // Detail description can be found in the Run.hh/cc.
    // return new Run(fSDName);
    if (_run)
        delete _run;

    _run = new Run{_SDName};
    return _run;
}

void RunAction::BeginOfRunAction(const G4Run* aRun)
{
    std::cout << "### Run " << aRun->GetRunID() << " start." << std::endl;

    //inform the runManager to save random number seed
    G4RunManager::GetRunManager()->SetRandomNumberStore(false);
}

void DicomRunAction::EndOfRunAction(const G4Run* aRun)
{
    int nofEvents = aRun->GetNumberOfEvent();

    if (nofEvents == 0)
        return;

    static double local_total_dose = 0.0;
    double total_dose              = 0.0;

    const Run* reRun = static_cast<const Run*>(aRun);

    //--- Dump all scored quantities involved in Run.
    for ( int i = 0; i < (int)_SDName.size(); i++ )
    {
        //
        //---------------------------------------------
        // Dump accumulated quantities for this RUN.
        //  (Display only central region of x-y plane)
        //      0       ConcreteSD/DoseDeposit
        //---------------------------------------------
        G4THitsMap<G4double>* DoseDeposit = reRun->GetHitsMap(_SDName[i]+"/DoseDeposit");

        if( DoseDeposit && DoseDeposit->GetMap()->size() != 0 )
        {
            std::map<G4int,G4double*>::iterator itr = DoseDeposit->GetMap()->begin();
            for(; itr != DoseDeposit->GetMap()->end(); itr++)
            {
                if(!IsMaster())
                {
                    local_total_dose += *(itr->second);
                }
                total_dose += *(itr->second);
            }
        }
    }

    if (IsMaster())
    {
      G4cout
        << "\n--------------------End of Global Run-----------------------"
        << " \n The run was " << nofEvents << " events " << G4endl;
      G4cout << "LOCAL TOTAL DOSE : \t" << local_total_dose/gray
             << " Gy" << std::endl;
      G4cout << "      TOTAL DOSE : \t" << total_dose/gray
             << " Gy" << std::endl;

    }
  else
    {
      G4cout
        << "\n--------------------End of Local Run------------------------"
        << " \n The run was " << nofEvents << G4endl;
      G4cout << "LOCAL TOTAL DOSE : \t" << local_total_dose/gray
             << " Gy" << std::endl;
      G4cout << "      TOTAL DOSE : \t" << total_dose/gray
             << " Gy" << std::endl;

    }

  if(IsMaster()) {
    G4cout << " ###### EndOfRunAction ###### " << G4endl;
    //- DicomRun object.
    const DicomRun* re02Run = static_cast<const DicomRun*>(aRun);
    //--- Dump all scored quantities involved in DicomRun.

    for ( G4int i = 0; i < (G4int)fSDName.size(); i++ ){
      //
      //---------------------------------------------
      // Dump accumulated quantities for this RUN.
      //  (Display only central region of x-y plane)
      //      0       ConcreteSD/DoseDeposit
      //---------------------------------------------
      G4THitsMap<G4double>* DoseDeposit =
        re02Run->GetHitsMap(fSDName[i]+"/DoseDeposit");

      G4cout << "============================================================="
             <<G4endl;
      G4cout << " Number of event processed : "
             << aRun->GetNumberOfEvent() << G4endl;
      G4cout << "============================================================="
             <<G4endl;

      std::ofstream fileout;
      G4String fname = "dicom.out";
      fileout.open(fname);
      G4cout << " opened file " << fname << " for dose output" << G4endl;


      if( DoseDeposit && DoseDeposit->GetMap()->size() != 0 ) {
        std::ostream *myout = &G4cout;
        PrintHeader(myout);
        std::map<G4int,G4double*>::iterator itr =DoseDeposit->GetMap()->begin();
        for(; itr != DoseDeposit->GetMap()->end(); itr++) {
          fileout <<  itr->first
                  << "     "  << *(itr->second)/CLHEP::gray
                  << G4endl;
          G4cout << "    " << itr->first
                 << "     " << std::setprecision(6)
                 << *(itr->second)/CLHEP::gray << " Gy"
                 << G4endl;
        }
        G4cout << "============================================="<<G4endl;
            } else {
        G4Exception("DicomRunAction", "000", JustWarning,
      "DoseDeposit HitsMap is either a null pointer of the HitsMap was empty");
      }
      fileout.close();
      G4cout << " closed file " << fname << " for dose output" << G4endl;

    }
  }

  G4cout << "Finished : End of Run Action " << aRun->GetRunID() << G4endl;

}

void DicomRunAction::PrintHeader(std::ostream *out)
{
  std::vector<G4String> vecScoreName;
  vecScoreName.push_back("DoseDeposit");

  // head line
  //
  std::string vname;
  *out << std::setw(10) << "Voxel" << " |";
  for (std::vector<G4String>::iterator it = vecScoreName.begin();
       it != vecScoreName.end(); it++) {
    //vname = FillString((*it),
    //                       ' ',
    //                       FieldValue+1,
    //                       false);
    //    *out << vname << '|';
    *out << std::setw(fFieldValue) << (*it) << "  |";
  }
  *out << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
std::string DicomRunAction::FillString(const std::string &name,
                                       char c, G4int n, G4bool back)
{
  std::string fname("");
  G4int k = n - name.size();
  if (k > 0) {
    if (back) {
      fname = name;
      fname += std::string(k,c);
    }
    else {
      fname = std::string(k,c);
      fname += name;
    }
  }
  else {
    fname = name;
  }
  return fname;
}
