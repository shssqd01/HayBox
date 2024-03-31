#ifndef _MODES_GENERIC_HPP
#define _MODES_GENERIC_HPP

#include "core/ControllerMode.hpp"
#include "core/socd.hpp"
#include "core/state.hpp"

class Generic : public ControllerMode {
  public:
    Generic(socd::SocdType socd_type, bool is_switch);

  private:
    bool _is_switch;
    void UpdateDigitalOutputs(InputState &inputs, OutputState &outputs);
    void UpdateAnalogOutputs(InputState &inputs, OutputState &outputs);
};

#endif
