#pragma once

#include <vector>

#include "globals.hh"
#include "G4UserRunAction.hh"

class G4Run;
class Run;

class RunAction : public G4UserRunAction
{
#pragma region Data
    private: static RunAction* _instance;

    private: Run* _run;

    private: std::vector<std::tring> fSDName; // - vector of MultiFunctionalDetecor names.
    private: int _field_value;
#pragma endregion

#pragma region Ctor/Dtor/ops
    public: RunAction();
    public: virtual ~RunAction();
#pragma endregion

#pragma region Singleton
    static RunAction* Instance();
#pragma endregion

#pragma region Interfaces
    public: virtual G4Run* GenerateRun() override;
    public: virtual void BeginOfRunAction(const G4Run*) override;
    public: virtual void EndOfRunAction(const G4Run*) override;
#pragma endregion

    public: Run* run() const
    {
        return _run;
    }

    public: void PrintHeader(std::ostream *out);
    public: std::string FillString(const std::string &name, char c, int n, bool back=true);
};
