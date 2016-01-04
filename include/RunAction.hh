#pragma once

#include <vector>

#include "globals.hh"
#include "G4UserRunAction.hh"

class G4Run;
class Run;

class RunAction : public G4UserRunAction
{
#pragma region Singleton
    private: static RunAction* _instance;
#pragma endregion

#pragma region Data
    private: Run*                     _run;

    private: std::vector<std::string> _SDName; // - vector of MultiFunctionalDetecor names.
    private: int                      _field_width;
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

    public: void print_header(std::ostream *out);
    public: std::string fill_string(const std::string &name, char c, int n, bool back=true);
};
