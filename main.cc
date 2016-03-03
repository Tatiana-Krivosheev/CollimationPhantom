#include <iostream>
#include <string>

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4SystemOfUnits.hh"

#include "globals.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "G4GenericPhysicsList.hh"
#include "QGSP_BIC.hh"
#include "G4tgrMessenger.hh"

#include "PhantomSetup.hh"
#include "Detector.hh"
#include "Initialization.hh"

int main(int argc, char* argv[])
{
    new G4tgrMessenger; // ?

    // Choose the Random engine
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    CLHEP::HepRandom::setTheSeed(24534575684783);
    long seeds[2];
    seeds[0] = 534524575674523;
    seeds[1] = 526345623452457;
    CLHEP::HepRandom::setTheSeeds(seeds);

    // Construct the default run manager
    int nthreads = 4;
    G4MTRunManager* runManager = new G4MTRunManager;
    runManager->SetNumberOfThreads(nthreads);

    std::cout << "\n\n\tPHANTOM running in multithreaded mode with " << nthreads
              << " threads\n\n" << std::endl;

    // Treatment of patient images before creating the G4runManager
    PhantomSetup phs{"phantom.hed"};

    // Detector construction
    auto* geometry = new Detector{phs};
    runManager->SetUserInitialization(geometry);

    // Physics list
    auto* phs_vec = new std::vector<G4String>;
    phs_vec->push_back("G4EmStandardPhysics");
    G4VModularPhysicsList* phys = new G4GenericPhysicsList(phs_vec);
    runManager->SetUserInitialization(phys);

    // User action initialization
    runManager->SetUserInitialization(new Initialization());

    runManager->Initialize();

#ifdef G4VIS_USE
    // visualisation manager
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
#endif

    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    if (argc==1)
    {
#ifdef G4UI_USE
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);

#ifdef G4VIS_USE
        UImanager->ApplyCommand("/control/execute vis.mac");
#endif
        ui->SessionStart();
        delete ui;
#endif
    }
    else
    {
        std::string command   = "/control/execute ";
        std::string file_name = argv[1];
        UImanager->ApplyCommand(command + file_name);
    }

    delete runManager;

#ifdef G4VIS_USE
    delete visManager;
#endif

    return 0;
}
