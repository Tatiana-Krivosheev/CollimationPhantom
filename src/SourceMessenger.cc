#include "SourceMessenger.hh"
#include "Source.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

SourceMessenger::SourceMessenger(Source* source):
    _source{source},
    _src_directory{nullptr},
    _iso_radius_cmd{nullptr},
    _src_angle_cmd{nullptr},
    _src_fname_cmd{nullptr}
{
    _src_directory = new G4UIdirectory("/GP/source/");
    _src_directory->SetGuidance("Source construction control");

    _iso_radius_cmd = new G4UIcmdWithADoubleAndUnit("/GP/source/iso_radius", this);
    _iso_radius_cmd->SetGuidance("Set ISOsource radius");
    _iso_radius_cmd->SetParameterName("iso_radius", false);
    _iso_radius_cmd->SetUnitCategory("Length");
    _iso_radius_cmd->SetRange("iso_radius>0.0");
    _iso_radius_cmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    _src_angle_cmd = new G4UIcmdWithADoubleAndUnit("/GP/source/src_angle", this);
    _src_angle_cmd->SetGuidance("Set source angle");
    _src_angle_cmd->SetParameterName("src_angle", false);
    _src_angle_cmd->SetUnitCategory("Length");
    _src_angle_cmd->SetRange("src_angle>0.0");
    _src_angle_cmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    // _src_angle_cmd->SetDefaultUnit("degree");

    _src_fname_cmd = new G4UIcmdWithAString("/GP/source/src_fname", this);
    _src_fname_cmd->SetGuidance("Set file name");
    _src_angle_cmd->SetParameterName("src_fname", false);
    _src_angle_cmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

SourceMessenger::~SourceMessenger()
{
	delete _iso_radius_cmd;
	delete _src_angle_cmd;
	delete _src_fname_cmd;

	delete _src_directory;
}

void SourceMessenger::SetNewValue(G4UIcommand* cmd, G4String value)
{
	if (cmd == _iso_radius_cmd)
	{
		_source->set_iso_radius(_iso_radius_cmd->GetNewDoubleValue(value));
		return;
	}

	if (cmd == _src_angle_cmd)
	{
	    _source->set_src_angle(_src_angle_cmd->GetNewDoubleValue(value));
		return;
	}

	if (cmd == _src_fname_cmd)
    {
	    _source->set_sources(value);
		return;
    }

	return;
}
