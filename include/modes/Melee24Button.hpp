#ifndef _MODES_MELEE24BUTTON_HPP
#define _MODES_MELEE24BUTTON_HPP

#include "core/ControllerMode.hpp"
#include "core/socd.hpp"
#include "core/state.hpp"
#include "MeleeLimits.hpp"

typedef struct {
    bool crouch_walk_os = false;
    bool teleport_coords = false;
} Melee24ButtonOptions;

class Melee24Button : public ControllerMode {
  public:
    Melee24Button(socd::SocdType socd_type, Melee24ButtonOptions options = {});

  protected:
    void UpdateDigitalOutputs(InputState &inputs, OutputState &outputs);
    void UpdateAnalogOutputs(InputState &inputs, OutputState &outputs);

  private:
    Melee24ButtonOptions _options;
    bool _horizontal_socd;

    void HandleSocd(InputState &inputs);
};

#endif
