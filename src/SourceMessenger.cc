#include "SourceMessenger.hh"
#include "Source.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

SourceMessenger::SourceMessenger(Source* source):
    _source{source},
    _srcDirectory{nullptr},
    _radiusCmd{nullptr},
    _halfzCmd{nullptr}
{
    _srcDirectory = new G4UIdirectory("/GP/source/");
    _srcDirectory->SetGuidance("Source construction control");

    _radiusCmd = new G4UIcmdWithADoubleAndUnit("/GP/source/radius", this);
    _radiusCmd->SetGuidance("Set source radius");
    _radiusCmd->SetParameterName("radius",false);
    _radiusCmd->SetUnitCategory("Length");
    _radiusCmd->SetRange("radius>0.0");
    _radiusCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    _halfzCmd = new G4UIcmdWithADoubleAndUnit("/GP/source/halfz", this);
    _halfzCmd->SetGuidance("Set source halfz");
    _halfzCmd->SetParameterName("halfz",false);
    _halfzCmd->SetUnitCategory("Length");
    _halfzCmd->SetRange("halfz>0.0");
    _halfzCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

SourceMessenger::~SourceMessenger()
{
	delete _radiusCmd;
	delete _halfzCmd;

	delete _srcDirectory;
}

void SourceMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue)
{
	if (cmd == _radiusCmd)
	{
		// _source->set_radius(_radiusCmd->GetNewDoubleValue(newValue));
		return;
	}

	if (cmd == _halfzCmd)
	{
		// _source->set_halfz(_halfzCmd->GetNewDoubleValue(newValue));
		return;
	}

	return;
}
