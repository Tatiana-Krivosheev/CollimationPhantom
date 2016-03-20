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
    private: Source*                    _source;

    private: G4UIdirectory*             _src_directory;

	private: G4UIcmdWithADoubleAndUnit* _iso_radius_cmd;

	private: G4UIcmdWithADoubleAndUnit* _src_angle_cmd;

	private: G4UIcmdWithADoubleAndUnit* _rot_start_cmd;
	private: G4UIcmdWithADoubleAndUnit* _rot_stop_cmd;

	private: G4UIcmdWithADoubleAndUnit* _shift_x_cmd;
	private: G4UIcmdWithADoubleAndUnit* _shift_y_cmd;
	private: G4UIcmdWithADoubleAndUnit* _shift_z_cmd;

	private: G4UIcmdWithAString*        _src_fname_cmd;
#pragma endregion

#pragma region Ctor/Dtor/ops
    public:	SourceMessenger(Source* source);
	public: ~SourceMessenger();
#pragma endregion

#pragma region Interfaces
	public: virtual void SetNewValue(G4UIcommand* cmd, G4String value) override;
#pragma endregion
};
