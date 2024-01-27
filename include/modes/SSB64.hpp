#ifndef _MODES_SSB64_HPP
#define _MODES_SSB64_HPP

#include "core/ControllerMode.hpp"
#include "core/socd.hpp"
#include "core/state.hpp"

class SSB64 : public ControllerMode {
  public:
    SSB64(socd::SocdType socd_type, bool enable_cbutton_aerials = false);

  protected:
    void UpdateDigitalOutputs(InputState &inputs, OutputState &outputs);
    void UpdateAnalogOutputs(InputState &inputs, OutputState &outputs);

  private:
    bool _horizontal_socd;
    bool _cbutton_aerials;

    void HandleSocd(InputState &inputs);
};

#endif
