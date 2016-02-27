#pragma once

#include "G4UserEventAction.hh"
#include "globals.hh"

class G4Event;

class EventAction : public G4UserEventAction
{
#pragma region Data
    private: std::string _draw_flag;
#pragma endregion

#pragma region Ctor/Dtor/ops
    public:          EventAction();
    public: virtual ~EventAction();
#pragma endregion

#pragma region Interfaces
    public: virtual void BeginOfEventAction(const G4Event*) override;
    public: virtual void EndOfEventAction(const G4Event* anEvent) override;
#pragma endregion

    public: void SetDrawFlag(std::string val)
    {
        _draw_flag = val;
    }
};
