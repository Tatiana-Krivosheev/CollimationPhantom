#include <iostream>

#include "EventAction.hh"

#include "G4Event.hh"
#include "RunAction.hh"
#include "Run.hh"

EventAction::EventAction():
    G4UserEventAction{},
    _draw_flag("all")
{
}

EventAction::~EventAction()
{
}

void EventAction::BeginOfEventAction(const G4Event* evt)
{
    std::cout << "EV: " << evt->GetEventID() << std::endl;
}

void EventAction::EndOfEventAction(const G4Event*)
{
}
