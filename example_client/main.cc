#include <httplib/httplib.h>
#include <sys/stat.h>

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

void get_server_info(httplib::Client& client, const std::string& host,
                     const int port) {
  std::cout << "\n----------------------------------------------------\n";
  std::cout << "Sending GET to " << host << ":" << port << "/version\n";
  auto res = client.Get("/version");
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

void post_image(httplib::Client& client, const std::string& image,
                const std::string& host, const int port) {
  std::unique_ptr<FILE, decltype(&std::fclose)> file(
      std::fopen(image.c_str(), "rb"), std::fclose);
  struct stat stat;
  fstat(fileno(file.get()), &stat);
  if (!stat.st_size) {
    std::cout << "Cannot get file size for: " << image << "\n";
    std::abort();
  }

  std::string content;
  content.resize(stat.st_size);
  int64_t read =
      std::fread(&content[0], sizeof(char), stat.st_size, file.get());
  if (read != stat.st_size) {
    std::cout << "Cannot read image: " << image << "\n";
    std::abort();
  }

  auto encoded = httplib::detail::base64_encode(content);
  nlohmann::json j = {{"data", encoded}};

  std::cout << "\n----------------------------------------------------\n";
  std::cout << "Sending POST @data={\"data\": base64_encode(" << image << ")} to "
            << host << ":" << port << "/detects\n";
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
  if (argc != 4) {
    std::cout << "Usage: " << argv[0] << " <host> "
              << " <port> "
              << " <image_path>\n";
    std::cout << "Please supply all arguments\n";
    exit(1);
  }

  const std::string host(argv[1]);
  // const std::string host = "192.168.100.2";
  const int port = atoi(argv[2]);
  // const int port = 8888;
  const std::string image(argv[3]);
  // const std::string image = "cat.bmp";
  httplib::Client client(host, port);

  // GET /info
  get_server_info(client, host, port);
  // POST /detects
  post_image(client, image, host, port);

  return 0;
}
