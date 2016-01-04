#include "Initialization.hh"

#include "Source.hh"
#include "RunAction.hh"
#include "EventAction.hh"

Initialization::Initialization():
    G4VUserActionInitialization()
{
}

Initialization::~Initialization()
{
}

void Initialization::BuildForMaster() const
{
    SetUserAction(new RunAction);
}

void Initialization::Build() const
{
    SetUserAction(new Source);
    SetUserAction(new RunAction);
    SetUserAction(new EventAction);
}

