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
	_rot_start_cmd{nullptr},
	_rot_stop_cmd{nullptr},
    _shift_x_cmd{nullptr},
    _shift_y_cmd{nullptr},
    _shift_z_cmd{nullptr},
    _src_fname_cmd{nullptr}
{
    _src_directory = new G4UIdirectory("/GP/source/");
    _src_directory->SetGuidance("Source construction control");

    _iso_radius_cmd = new G4UIcmdWithADoubleAndUnit("/GP/source/iso_radius", this);
    _iso_radius_cmd->SetGuidance("Set ISOsource radius");
    _iso_radius_cmd->SetParameterName("isoRadius", false);
    //_iso_radius_cmd->SetUnitCategory("Length");
    _iso_radius_cmd->SetDefaultUnit("mm");
    _iso_radius_cmd->SetUnitCandidates("mm cm m");
    _iso_radius_cmd->SetRange("isoRadius>0.0");
    _iso_radius_cmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    _src_angle_cmd = new G4UIcmdWithADoubleAndUnit("/GP/source/src_angle", this);
    _src_angle_cmd->SetGuidance("Set source angle");
    _src_angle_cmd->SetParameterName("src_angle", true);
    _src_angle_cmd->SetDefaultUnit("degree");
    _src_angle_cmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    _rot_start_cmd = new G4UIcmdWithADoubleAndUnit("/GP/source/rot_start", this);
    _rot_start_cmd->SetGuidance("Set rotation start angle");
    _rot_start_cmd->SetParameterName("rot_start", true);
    _rot_start_cmd->SetDefaultUnit("degree");
    _rot_start_cmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    _rot_stop_cmd = new G4UIcmdWithADoubleAndUnit("/GP/source/rot_stop", this);
    _rot_stop_cmd->SetGuidance("Set rotation stop angle");
    _rot_stop_cmd->SetParameterName("rot_stop", true);
    _rot_stop_cmd->SetDefaultUnit("degree");
    _rot_stop_cmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    _shift_x_cmd = new G4UIcmdWithADoubleAndUnit("/GP/source/shift_x", this);
    _shift_x_cmd->SetGuidance("Set Source2Phantom X shift");
    _shift_x_cmd->SetParameterName("shift_x", false);
    _shift_x_cmd->SetDefaultUnit("mm");
    _shift_x_cmd->SetUnitCandidates("mm cm m");
    _shift_x_cmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    _shift_y_cmd = new G4UIcmdWithADoubleAndUnit("/GP/source/shift_y", this);
    _shift_y_cmd->SetGuidance("Set Source2Phantom Y shift");
    _shift_y_cmd->SetParameterName("shift_y", false);
    _shift_y_cmd->SetDefaultUnit("mm");
    _shift_y_cmd->SetUnitCandidates("mm cm m");
    _shift_y_cmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    _shift_z_cmd = new G4UIcmdWithADoubleAndUnit("/GP/source/shift_z", this);
    _shift_z_cmd->SetGuidance("Set Source2Phantom Z shift");
    _shift_z_cmd->SetParameterName("shift_z", false);
    _shift_z_cmd->SetDefaultUnit("mm");
    _shift_z_cmd->SetUnitCandidates("mm cm m");
    _shift_z_cmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    _src_fname_cmd = new G4UIcmdWithAString("/GP/source/src_fname", this);
    _src_fname_cmd->SetGuidance("Set file name");
    _src_angle_cmd->SetParameterName("srcFname", false);
    _src_angle_cmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

SourceMessenger::~SourceMessenger()
{
	delete _iso_radius_cmd;
	delete _src_angle_cmd;

    delete _rot_start_cmd;
    delete _rot_stop_cmd;

    delete _shift_x_cmd;
    delete _shift_y_cmd;
    delete _shift_z_cmd;

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

	if (cmd == _rot_start_cmd)
	{
	    _source->set_rot_start(_rot_start_cmd->GetNewDoubleValue(value));
		return;
	}

	if (cmd == _rot_stop_cmd)
	{
	    _source->set_rot_stop(_rot_stop_cmd->GetNewDoubleValue(value));
		return;
	}

	if (cmd == _shift_x_cmd)
	{
		_source->set_shift_x(_shift_x_cmd->GetNewDoubleValue(value));
		return;
	}

	if (cmd == _shift_y_cmd)
	{
		_source->set_shift_y(_shift_y_cmd->GetNewDoubleValue(value));
		return;
	}

	if (cmd == _shift_z_cmd)
	{
		_source->set_shift_z(_shift_z_cmd->GetNewDoubleValue(value));
		return;
	}

	if (cmd == _src_fname_cmd)
    {
	    _source->set_sources(value);
		return;
    }

	return;
}
