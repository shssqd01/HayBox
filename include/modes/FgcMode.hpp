#ifndef _MODES_FGCMODE_HPP
#define _MODES_FGCMODE_HPP

#include "core/ControllerMode.hpp"
#include "core/socd.hpp"
#include "core/state.hpp"

class FgcMode : public ControllerMode {
  public:
    FgcMode(socd::SocdType horizontal_socd, socd::SocdType vertical_socd, bool swap_up_down);

  private:
    bool _swap_up_down;
    void UpdateDigitalOutputs(InputState &inputs, OutputState &outputs);
    void UpdateAnalogOutputs(InputState &inputs, OutputState &outputs);
};

#endif
