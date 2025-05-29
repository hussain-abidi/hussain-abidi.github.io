#include "server.h"
#include <csignal>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <uWebSockets/HttpErrors.h>

namespace server {
  us_listen_socket_t* listener;
  Inventory* inv;

  void run(int port, Inventory* inventory) {
    inv = inventory;
    std::signal(SIGINT, cleanup);
    std::signal(SIGTERM, cleanup);

    uWS::App()
        .get("/*",
             [](uWS::HttpResponse<false>* res, uWS::HttpRequest* req) {
               std::string_view path = req->getFullUrl();
               serveFile(path, res);
             })
        .ws<SocketData>("/ws", {.open = socketOpen,
                                .message = socketMessage,
                                .close = socketClose})
        .listen(port,
                [port](auto* listenSocket) {
                  listener = listenSocket;
                  if (!listenSocket) {
                    std::cout << "Failed to bind to port " << port << '\n';
                  } else
                    std::cout << "HTTP+WebSocket server listening on port "
                              << port << '\n';
                })
        .run();
  }

  std::string getContentType(const std::string& extension) {
    if (extension == ".html")
      return "text/html";
    if (extension == ".js")
      return "text/javascript";
    if (extension == ".css")
      return "text/css";

    return "text/plain";
  }

  const std::string BASE_PATH = "../public";
  void serveFile(std::string_view path, uWS::HttpResponse<false>* res) {
    std::ostringstream buffer;
    std::ifstream ifs;

    std::filesystem::path filePath = BASE_PATH;

    if (path == "/") {
      filePath += "/index.html";
    } else {
      filePath += std::string(path);
    }

    ifs.open(filePath);
    if (!ifs) {
      res->write("404 Not Found");
      res->close();
      return;
    }

    buffer << ifs.rdbuf();

    res->writeHeader("Content-Type", getContentType(filePath.extension()));
    res->writeStatus(uWS::HTTP_200_OK);
    res->end(buffer.str());
  }

  void socketOpen(WS* ws) {
    std::vector<uint8_t> vec = inv->encode();
    std::string_view data(reinterpret_cast<const char*>(vec.data()),
                          vec.size());

    ws->send(data);
  }

  void socketMessage(WS* ws, std::string_view message, uWS::OpCode opCode) {
    if (message.size() < sizeof(uint32_t)) {
      ws->close();
      return;
    }
    auto* data = message.data();

    uint32_t nameSize;
    std::string name;
    unsigned long quantity;

    memcpy(&nameSize, data, sizeof(uint32_t));
    data += sizeof(uint32_t);

    if (message.size() < sizeof(int) + nameSize + sizeof(uint32_t)) {
      ws->close();
      return;
    };

    name = std::string(data, nameSize);
    data += nameSize;
    memcpy(&quantity, data, sizeof(uint32_t));

    inv->setQuantity(name, quantity);
  }

  void socketClose(WS* ws, int code, std::string_view message) {}

  void cleanup(int) {
    std::cout << '\n';
    us_listen_socket_close(0, listener);

    std::exit(0); // TODO: fix
  }
} // namespace server
