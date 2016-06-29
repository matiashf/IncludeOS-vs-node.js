// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2015-2016 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <os>
#include <net/inet4>

using Connection_ptr = net::TCP::Connection_ptr;
using Disconnect = net::TCP::Connection::Disconnect;

// An IP-stack object
std::unique_ptr<net::Inet4<VirtioNet> > inet;

void Service::start() {
  // Assign a driver (VirtioNet) to a network interface (eth0)
  hw::Nic<VirtioNet>& eth0 = hw::Dev::eth<0,VirtioNet>();

  // Bring up a network stack, attached to the nic
  inet = std::make_unique<net::Inet4<VirtioNet> >(eth0);

  // FIXME: Remove static config?
  // Static IP configuration, until we (possibly) get DHCP
  inet->network_config( { 10,0,0,42 },      // IP
                        { 255,255,255,0 },  // Netmask
                        { 10,0,0,1 },       // Gateway
                        { 8,8,8,8 } );      // DNS

  // Set up a TCP server on port 80
  auto& server = inet->tcp().bind(80);
  inet->dhclient()->on_config([&server](bool timeout) {
    if(!timeout)
      printf("Server IP updated: %s\n", server.local().to_string().c_str());
  });
  printf("Server listening: %s \n", server.local().to_string().c_str());
  // When someone connects to our server
  server
    .onDisconnect([](net::TCP::Connection_ptr client, Disconnect reason) {
        printf("Disconnect: %s: %s\n",
               client->to_string().c_str(), reason.to_string().c_str());
        client->close();
      })
    .onConnect([](net::TCP::Connection_ptr client) {
      printf("Connect: %s\n", client->to_string().c_str());
      client->read(1024, [client](auto buf, size_t size) {
          std::string data{(char *) buf.get(), size};
          printf("[%s] %s%s", client->to_string().c_str(),
                 data.c_str(), (data.back() == '\n') ? "" : "\n");
          client->write(data.data(), data.size());
        });
      client->onError([](Connection_ptr client, net::TCP::TCPException exc) {
          printf("Error: %s: %s\n", client->to_string().c_str(), exc.what());
        });
      });
}
