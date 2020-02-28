#pragma once

#include <zmq.hpp>

#include "common/types.h"
#include "module/base/module.h"

namespace slog {

class Ticker : public Module {
public:
  const static string ENDPOINT;

  Ticker(zmq::context_t& context, uint32_t ticks_per_sec);

  void SetUp() final;
  void Loop() final;

private:
  zmq::socket_t socket_;
  uint32_t ticks_per_sec_;
  Duration sleep_us_;
};

} // slog