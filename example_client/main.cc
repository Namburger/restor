#include <httplib/httplib.h>
#include <sys/stat.h>

#include <cxxopts/cxxopts.hpp>
#include <fstream>
#include <iostream>
#include <locale>
#include <nlohmann/json.hpp>
#include <string>

void send_get(
    httplib::Client& client, const std::string& host, const int port, const std::string& endpoint) {
  std::cout << "Sending GET to " << host << ":" << port << endpoint << "\n";
  auto res = client.Get(endpoint.c_str());
  if (res && (res->status == 200)) {
    std::cout << nlohmann::json::parse(res->body).dump(2) << "\n";
  } else {
    if (res->status) {
      std::cout << "\nError: " << res->status << "\n";
    } else {
      std::cout << "\nError: No reponse from server"
                << "\n";
    }
  }
}

void post_image(
    httplib::Client& client, const std::string& image, const std::string& host, const int port) {
  std::unique_ptr<FILE, decltype(&std::fclose)> file(std::fopen(image.c_str(), "rb"), std::fclose);
  struct stat stat;
  fstat(fileno(file.get()), &stat);
  if (!stat.st_size) {
    std::cout << "Cannot get file size for: " << image << "\n";
    std::abort();
  }

  std::string content;
  content.resize(stat.st_size);
  int64_t read = std::fread(&content[0], sizeof(char), stat.st_size, file.get());
  if (read != stat.st_size) {
    std::cout << "Cannot read image: " << image << "\n";
    std::abort();
  }

  auto encoded = httplib::detail::base64_encode(content);
  nlohmann::json j = {{"data", encoded}};

  std::cout << "Sending POST @data={\"data\": base64_encode(" << image << ")} to " << host << ":"
            << port << "/detects\n";
  auto res = client.Post("/detects", j.dump(), "application/json");
  if (res && (res->status == 200)) {
    std::cout << nlohmann::json::parse(res->body).dump(2) << "\n";
  } else {
    if (res->status) {
      std::cout << "\nError: " << res->status << "\n";
    } else {
      std::cout << "\nError: No reponse from server"
                << "\n";
    }
  }
}

int main(int argc, char* argv[]) {
  cxxopts::Options options(argv[0], "restor's example client");
  // clang-format off
  options.add_options()
      ("h,host", "HostIP", cxxopts::value<std::string>())
      ("p,port", "Port", cxxopts::value<int>())
      ("m,method", "Method", cxxopts::value<std::string>())
      ("e,endpoint", "Endpoint", cxxopts::value<std::string>())
      ("i,image", "Path to image", cxxopts::value<std::string>());
  // clang-format on

  auto result = options.parse(argc, argv);

  if (!result.count("host") || !result.count("port")) {
    std::cout << options.help({"", "Group"}) << std::endl;
    std::cout << "\nPlease provide server's ip and port\n";
    exit(1);
  }

  const std::string& host(result["host"].as<std::string>());
  const int port = result["port"].as<int>();

  httplib::Client client(host, port);

  if (!result.count("method") || !result.count("endpoint")) {
    std::cout << "No method or end point provided, sending GET /version by default\n";
    send_get(client, host, port, "/version");
    exit(1);
  }

  std::string method(result["method"].as<std::string>());
  std::transform(method.begin(), method.end(), method.begin(), ::toupper);

  if (method == "GET") {
    send_get(client, host, port, result["endpoint"].as<std::string>());
  } else if (method == "POST") {
    if (!result.count("image")) {
      std::cout << "Please specify an image to send\n";
      exit(1);
    }
    if (result["endpoint"].as<std::string>() != "/detects") {
      std::cout << "POST must be sent to /detects\n";
      exit(1);
    }
    post_image(client, result["image"].as<std::string>(), host, port);
  }

  return 0;
}
