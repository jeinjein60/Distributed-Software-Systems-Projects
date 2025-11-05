/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#include <chrono>
#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  std::string SayHello(const std::string& user) {
    HelloRequest request;
    request.set_name(user);

    HelloReply reply;
    ClientContext context;
  
    //added code to start
    auto start = std::chrono::high_resolution_clock::now();
    Status status = stub_->SayHello(&context, request, &reply);

    //end
    auto end = std::chrono::high_resolution_clock::now();
    auto rtt = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    if (status.ok()) {
      std::cout << "RTT in microseconds: " << rtt << std::endl;
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
      return "RPC failed";
    }
    // Act upon its status.
    // if (status.ok()) {
    //   std::cout<< "RTT in microseconds: " << rtt << std::endl;
    //   return reply.message();
    // } else {
    //   std::cout << status.error_code() << ": " << status.error_message()
    //             << std::endl;
    //   return "RPC failed";
    // }

  }
  void MeasureThroughput(int num_requests) {

    HelloRequest request;
    request.set_name("throughput-test");

    HelloReply reply;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_requests; i++) {
      ClientContext context;
      Status status = stub_->SayHello(&context, request, &reply);

      if (!status.ok()) {
        std::cout << "RPC failed " << i << std::endl;
        return;
      }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    double seconds = duration_ms / 1000.0;
    double throughput = num_requests / seconds;

    std::cout << "Completed " << num_requests << " requests in " << seconds << " s." << std::endl;
    std::cout << "Throughput: " << throughput << " requests/seconds" << std::endl;
  }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

// int main(int argc, char** argv) {
//   // Instantiate the client. It requires a channel, out of which the actual RPCs
//   // are created. This channel models a connection to an endpoint specified by
//   // the argument "--target=" which is the only expected argument.
//   // We indicate that the channel isn't authenticated (use of
//   // InsecureChannelCredentials()).
//   std::string target_str;
//   std::string arg_str("--target");
//   if (argc > 1) {
//     std::string arg_val = argv[1];
//     size_t start_pos = arg_val.find(arg_str);
//     if (start_pos != std::string::npos) {
//       start_pos += arg_str.size();
//       if (arg_val[start_pos] == '=') {
//         target_str = arg_val.substr(start_pos + 1);
//       } else {
//         std::cout << "The only correct argument syntax is --target="
//                   << std::endl;
//         return 0;
//       }
//     } else {
//       std::cout << "The only acceptable argument is --target=" << std::endl;
//       return 0;
//     }
//   } else {
//     target_str = "localhost:50051";
//   }
//   // GreeterClient greeter(
//   //     grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
//   GreeterClient greeter(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
//   greeter.MeasureThroughput(1000);
//   std::string user("world");
//   std::string reply = greeter.SayHello(user);
//   std::cout << "Greeter received: " << reply << std::endl;

//   return 0;
// }

int main(int argc, char** argv) {
  std::string target_str = "localhost:50051";

  GreeterClient greeter(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

  // 🔹 First measure RTT with one request
  std::string reply = greeter.SayHello("world");
  std::cout << "Greeter received: " << reply << std::endl;

  // 🔹 Then measure throughput with 1000 requests
  greeter.MeasureThroughput(1000);

  return 0;
