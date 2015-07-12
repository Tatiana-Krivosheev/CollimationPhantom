#include "SourceMessenger.hh"
#include "Source.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

SourceMessenger::SourceMessenger(Source* source):
    _source{source},
    _srcDirectory{nullptr},
    _nof_rows_cmd{nullptr},
    _nof_cols_cmd{nullptr},
    _iso_radius_cmd{nullptr}
{
    _srcDirectory = new G4UIdirectory("/PH/source/");
    _srcDirectory->SetGuidance("Source construction control");

    _iso_radius_cmd = new G4UIcmdWithADoubleAndUnit("/PH/source/radius", this);
    _iso_radius_cmd->SetGuidance("Set iso radius");
    _iso_radius_cmd->SetParameterName("radius",false);
    _iso_radius_cmd->SetUnitCategory("Length");
    _iso_radius_cmd->SetRange("radius>0.0");
    _iso_radius_cmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    
    _nof_rows_cmd = new G4UIcmdWithAnInteger("/PH/source/nof_rows", this);
    _nof_rows_cmd->SetGuidance("Set number of rows");
    _nof_rows_cmd->SetParameterName("nof_rows",false);
    _nof_rows_cmd->SetRange("nof_rows>0");
    _nof_rows_cmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    
    _nof_cols_cmd = new G4UIcmdWithAnInteger("/PH/source/nof_cols", this);
    _nof_cols_cmd->SetGuidance("Set number of collimators per row");
    _nof_cols_cmd->SetParameterName("nof_cols",false);
    _nof_cols_cmd->SetRange("nof_cols>0");
    _nof_cols_cmd->AvailableForStates(G4State_PreInit, G4State_Idle);    
}

SourceMessenger::~SourceMessenger()
{
	delete _iso_radius_cmd;

	delete _nof_rows_cmd;
	delete _nof_cols_cmd;
	
	delete _srcDirectory;
}

void SourceMessenger::SetNewValue(G4UIcommand* cmd, G4String value)
{
	if (cmd == _iso_radius_cmd)
	{
		_source->set_iso_radius(_iso_radius_cmd->GetNewDoubleValue(value));
		return;
	}

	if (cmd == _nof_rows_cmd)
	{
		_source->set_nof_rows(_nof_rows_cmd->GetNewIntegerValue(value));
		return;
	}
	
	if (cmd == _nof_cols_cmd)
	{
		_source->set_nof_cols(_nof_cols_cmd->GetNewIntegerValue(value));
		return;
	}

	return;
}

