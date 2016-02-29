#pragma once

#include "G4VUserActionInitialization.hh"

class Initialization : public G4VUserActionInitialization
{
#pragma region Ctor/Dtor/ops
    public:          Initialization();
    public: virtual ~Initialization();
#pragma endregion

#pragma region Initialization
    public: virtual void BuildForMaster() const override;
    public: virtual void Build() const override;
#pragma endregion
};
