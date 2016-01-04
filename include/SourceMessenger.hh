// -*- C++ -*-

#pragma once

#include "globals.hh"
#include "G4UImessenger.hh"

class Source;
class G4UImessenger;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;

class SourceMessenger : public G4UImessenger
{
#pragma region Data
    private: Source* _source;

    private: G4UIdirectory*           _srcDirectory;

	private: G4UIcmdWithADoubleAndUnit* _radiusCmd;
	private: G4UIcmdWithADoubleAndUnit* _halfzCmd;
#pragma endregion

#pragma region Ctor/Dtor/ops
    public:	SourceMessenger(Source* source);
	public: ~SourceMessenger();
#pragma endregion

#pragma region Interfaces
	public: virtual void SetNewValue(G4UIcommand* cmd, G4String newValue) override;
#pragma endregion
};
