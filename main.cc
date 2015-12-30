#include "G4SystemOfUnits.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4GenericPhysicsList.hh"

#include "Randomize.hh"

#include "PhantomSetup.hh"
#include "Detector.hh"
#include "ActionInitialization.hh"

int main(int argc, char* argv[])
{
    G4UIExecutive* ui = 0;
    if ( argc == 1 )
    {
        ui = new G4UIExecutive(argc, argv);
    }

    // Choose the Random engine
    G4Random::setTheEngine(new CLHEP::RanecuEngine);

    // long long seeds[2];
    // seeds[0] = 534524575674523LL;
    // seeds[1] = 526345623452457LL;
    // CLHEP::HepRandom::setTheSeeds(seeds);

    // Construct the default run manager
#ifdef G4MULTITHREADED
    G4MTRunManager* runManager = new G4MTRunManager;
#else
    G4RunManager* runManager = new G4RunManager;
#endif

    // Treatment of patient images before creating the G4runManager
    auto* phs = new PhantomSetup("phantom.hed");

    // Set mandatory initialization classes

    // Detector construction
    auto* geometry = new Detector(phs);
    runManager->SetUserInitialization(geometry);

    // Physics list
    std::vector<string>* phs_vec = new std::vector<string>;
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
        std::string command = "/control/execute ";
        std::string fileName = argv[1];
        UImanager->ApplyCommand(command+fileName);
    }

    delete runManager;

#ifdef G4VIS_USE
    delete visManager;
#endif

    return 0;
}
