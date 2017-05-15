#include <fstream>
#include <iomanip>
#include <string>

#include "RunAction.hh"
#include "Run.hh"

#include "G4THitsMap.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4RunManager.hh"

RunAction* RunAction::_instance = nullptr;

RunAction* RunAction::Instance()
{
    return _instance;
}

RunAction::RunAction():
    G4UserRunAction{},
    _run{nullptr},
    _field_width{14}
{
    _SDName.push_back(std::string{"phantomSD"});
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
    // return new Run(_SDName);
    return _run = new Run{_SDName};
}

void RunAction::BeginOfRunAction(const G4Run* aRun)
{
    G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

    //inform the runManager to save random number seed
    G4RunManager::GetRunManager()->SetRandomNumberStore(false);
}

void RunAction::EndOfRunAction(const G4Run* aRun)
{
    int nofEvents = aRun->GetNumberOfEvent();

    if (nofEvents == 0)
        return;

    static double local_total_dose = 0.0;
    double total_dose              = 0.0;

    const Run* run = static_cast<const Run*>(aRun);

    //--- Dump all scored quantities involved in Run.
    for ( size_t i = 0; i != _SDName.size(); ++i )
    {
        //---------------------------------------------
        // Dump accumulated quantities for this RUN.
        //  (Display only central region of x-y plane)
        //      0       ConcreteSD/DoseDeposit
        //---------------------------------------------
        G4THitsMap<G4double>* DoseDeposit = run->GetHitsMap(_SDName[i]+"/DoseDeposit");

        if( DoseDeposit && DoseDeposit->GetMap()->size() != 0 )
        {
            auto itr = DoseDeposit->GetMap()->cbegin();
            for(; itr != DoseDeposit->GetMap()->cend(); ++itr)
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
        G4cout << "\n--------------------End of Global Run-----------------------"
               << " \n The run was " << nofEvents << " events " << G4endl;
        G4cout << "LOCAL TOTAL DOSE : \t" << local_total_dose/gray << " Gy" << G4endl;
        G4cout << "      TOTAL DOSE : \t" << total_dose/gray << " Gy" << G4endl;
    }
    else
    {
        G4cout << "\n--------------------End of Local Run------------------------"
               << " \n The run was " << nofEvents << G4endl;
        G4cout << "LOCAL TOTAL DOSE : \t" << local_total_dose/gray << " Gy" << G4endl;
        G4cout << "      TOTAL DOSE : \t" << total_dose/gray << " Gy" << G4endl;
    }

    if(IsMaster())
    {
        G4cout << " ###### EndOfRunAction ###### " << G4endl;

        const Run* re02Run = static_cast<const Run*>(aRun);
        //--- Dump all scored quantities involved in the Run.

        for ( size_t i = 0; i != _SDName.size(); ++i )
        {
        //---------------------------------------------
        // Dump accumulated quantities for this RUN.
        //  (Display only central region of x-y plane)
        //      0       ConcreteSD/DoseDeposit
        //---------------------------------------------
            G4THitsMap<G4double>* DoseDeposit = re02Run->GetHitsMap(_SDName[i]+"/DoseDeposit");

            G4cout << "=============================================================" << G4endl;
            G4cout << " Number of event processed : " << aRun->GetNumberOfEvent()     << G4endl;
            G4cout << "=============================================================" << G4endl;

            std::ofstream fileout;
            std::string fname = "dose.out";
            fileout.open(fname);

            G4cout << " opened file " << fname << " for dose output" << G4endl;

            if( DoseDeposit && DoseDeposit->GetMap()->size() != 0 )
            {
                std::ostream *myout = &G4cout;
                print_header(myout);

                auto itr = DoseDeposit->GetMap()->cbegin();
                for(; itr != DoseDeposit->GetMap()->cend(); ++itr)
                {
                    fileout <<  itr->first
                            << "     "  << *(itr->second)/CLHEP::gray
                            << G4endl;
                    G4cout << "    " << itr->first
                              << "     " << std::setprecision(6)
                              << *(itr->second)/CLHEP::gray << " Gy"
                              << G4endl;
                }
                G4cout << "=============================================" << G4endl;
            }
            else
            {
                G4Exception("RunAction", "000", JustWarning,
                            "DoseDeposit HitsMap is either a null pointer of the HitsMap was empty");
            }

            fileout.close();
            G4cout << " closed file " << fname << " for dose output" << G4endl;
        }
    }

    G4cout << "Finished : End of Run Action " << aRun->GetRunID() << G4endl;
}

void RunAction::print_header(std::ostream *out)
{
    std::vector<std::string> vecScoreName;
    vecScoreName.push_back("DoseDeposit");

    // head line
    std::string vname;
    *out << std::setw(10) << "Voxel" << " |";

    for (auto it = vecScoreName.cbegin(); it != vecScoreName.cend(); ++it)
    {
        //vname = FillString((*it),
        //                       ' ',
        //                       FieldValue+1,
        //                       false);
        //    *out << vname << '|';
        *out << std::setw(_field_width) << (*it) << "  |";
    }
    *out << G4endl;
}

std::string RunAction::fill_string(const std::string &name,
                                   char c, int n, bool back)
{
    std::string fname("");
    int k = n - name.size();
    if (k > 0)
    {
        if (back)
        {
            fname = name;
            fname += std::string(k, c);
        }
        else
        {
            fname = std::string(k, c);
            fname += name;
        }
    }
    else
    {
        fname = name;
    }

    return fname;
}
