//=====================================================================
///
///  (Description)
///  Run Class is for accumulating scored quantities which is
///  scored using G4MutiFunctionalDetector and G4VPrimitiveScorer.
///  Accumulation is done using G4THitsMap object.
///
///  The constructor DicomRun(const std::vector<std::string> mfdName)
///  needs a vector filled with MultiFunctionalDetector names which
///  was assigned at instantiation of MultiFunctionalDetector(MFD).
///  Then DicomRun constructor automatically scans primitive scorers
///  in the MFD, and obtains collectionIDs of all collections associated
///  to those primitive scorers. Futhermore, the G4THitsMap objects
///  for accumulating during a RUN are automatically created too.
///  (*) Collection Name is same as primitive scorer name.
///
///  The resultant information is kept inside DicomRun objects as data members.
///  std::vector<std::string> fCollName;            // Collection Name,
///  std::vector<int> fCollID;                 // Collection ID,
///  std::vector<G4THitsMap<G4double>*> fRunMap; // HitsMap for RUN.
///
///  The resualtant HitsMap objects are obtain using access method,
///  GetHitsMap(..).
///
//=====================================================================

#include "Run.hh"
#include "G4SDManager.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"

Run::Run():
    G4Run()
{
}

Run::Run(const std::vector<std::string> mfdName):
    G4Run()
{
    ConstructMFD(mfdName);
}

// Destructor
//    clear all data members.
Run::~Run()
{
    //--- Clear HitsMap for RUN
    int Nmap = _RunMap.size();
    for ( int i = 0; i != Nmap; ++i)
    {
        if( _RunMap[i] )
            _RunMap[i]->clear();
    }

    _CollName.clear();
    _CollID.clear();
    _RunMap.clear();
}

void Run::ConstructMFD(const std::vector<std::string>& mfdName)
{
    G4SDManager* SDman = G4SDManager::GetSDMpointer();

    //=================================================
    //  Initalize RunMaps for accumulation.
    //  Get CollectionIDs for HitCollections.
    //=================================================
    int Nmfd = mfdName.size();

    for ( int idet = 0; idet != Nmfd ; ++idet )  // Loop for all MFD.
    {
        std::string detName = mfdName[idet];
        //--- Seek and Obtain MFD objects from SDmanager.
        G4MultiFunctionalDetector* mfd = (G4MultiFunctionalDetector*)(SDman->FindSensitiveDetector(detName));

        if ( mfd )
        {
            //--- Loop over the registered primitive scorers.
            for (int icol = 0; icol != mfd->GetNumberOfPrimitives(); ++icol)
            {
                // Get Primitive Scorer object.
                G4VPrimitiveScorer* scorer = mfd->GetPrimitive(icol);

                // collection name and collectionID for HitsCollection,
                // where type of HitsCollection is G4THitsMap in case of a primitive scorer.
                // The collection name is given by <MFD name>/<Primitive Scorer name>.
                std::string collectionName = scorer->GetName();
                std::string fullCollectionName = detName+"/"+collectionName;
                int collectionID = SDman->GetCollectionID(fullCollectionName);

                if ( collectionID >= 0 )
                {
                    std::cout << "++ "<<fullCollectionName<< " id " << collectionID << std::endl;

                    // Store obtained HitsCollection information into data members.
                    // And, creates new G4THitsMap for accumulating quantities during RUN.
                    _CollName.push_back(fullCollectionName);
                    _CollID.push_back(collectionID);
                    _RunMap.push_back(new G4THitsMap<double>(detName,collectionName));
                }
                else
                {
                    std::cout << "** collection " << fullCollectionName << " not found. " << std::endl;
                }
            }
        }
    }
}

//  RecordEvent is called at end of event.
//  For scoring purpose, the resultant quantity in a event,
//  is accumulated during a Run.
void Run::RecordEvent(const G4Event* aEvent)
{
    numberOfEvent++;  // This is an original line.

    //=============================
    // HitsCollection of This Event
    //============================
    G4HCofThisEvent* HCE = aEvent->GetHCofThisEvent();
    if (!HCE)
        return;

    //=======================================================
    // Sum up HitsMap of this Event  into HitsMap of this RUN
    //=======================================================
    int Ncol = fCollID.size();
    for ( int i = 0; i != Ncol ; ++i )  // Loop over HitsCollection
    {
        G4THitsMap<double>* EvtMap = 0;
        if ( _CollID[i] >= 0 )           // Collection is attached to HCE
        {
            EvtMap = static_cast<G4THitsMap<double>*>(HCE->GetHC(_CollID[i]));
        }
        else
        {
            std::cout <<" Error EvtMap Not Found "<< i << std::endl;
        }
        if ( EvtMap )
        {
            //=== Sum up HitsMap of this event to HitsMap of RUN.===
            *_RunMap[i] += *EvtMap;
        }
    }

    G4Run::RecordEvent(aEvent);
}

// Merge hits map from threads
void Run::Merge(const G4Run* aRun)
{
    const Run* localRun = static_cast<const DicomRun*>(aRun);
    copy(_CollName, localRun->_CollName);
    copy(_CollID, localRun->_CollID);

    unsigned ncopies = copy(_RunMap, localRun->_RunMap);

    // copy function returns the fRunMap size if all data is copied
    // so this loop isn't executed the first time around
    std::cout << "Run :: Num copies = " << ncopies << std::endl;

    for(unsigned i = ncopies; i != _RunMap.size(); ++i)
    {
        *_RunMap[i] += *localRun->_RunMap[i];
    }
    Run::Merge(aRun);
}


//  Access method for HitsMap of the RUN
//-----
// Access HitsMap.
//  By  MultiFunctionalDetector name and Collection Name.
G4THitsMap<double>* Run::GetHitsMap(const std::string& detName,
                                    const std::string& colName) const
{
    std::string fullName = detName+"/"+colName;
    return GetHitsMap(fullName);
}

// Access HitsMap.
//  By full description of collection name, that is
//    <MultiFunctional Detector Name>/<Primitive Scorer Name>
G4THitsMap<double>* Run::GetHitsMap(const std::string& fullName) const
{
    int Ncol = fCollName.size();
    for ( int i = 0; i < Ncol; i++)
    {
        if ( fCollName[i] == fullName )
        {
            return fRunMap[i];
        }
    }

    G4Exception("Run", fullName.c_str(), JustWarning,
                "GetHitsMap failed to locate the requested HitsMap");

    return nullptr;
}
