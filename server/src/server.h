#pragma once

#include "inventory.h"
#include <uWebSockets/App.h>

namespace server {
  struct SocketData {
    int id;
  };

  using WS = uWS::WebSocket<false, true, SocketData>;

  void run(int port, Inventory* inventory);

  std::string getContentType(const std::string& extension);
  void serveFile(std::string_view path, uWS::HttpResponse<false>* res);

  void socketOpen(WS* ws);
  void socketMessage(WS* ws, std::string_view message, uWS::OpCode opCode);
  void socketClose(WS* ws, int code, std::string_view message);

  void cleanup(int signal);
} // namespace server
