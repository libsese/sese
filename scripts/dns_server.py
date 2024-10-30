#  Copyright 2024 libsese
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

import signal
import socket
import sys

import dns.message
import dns.name
import dns.query
import dns.rrset
import dns.rdatatype
import dns.rdataclass


mapping = {
    "example.com.": "192.168.1.1",
    "www.example.com.": "192.168.1.2",
    "mail.example.com": "192.168.1.2",
}


def handle_query(data, addr, sock):
    query = dns.message.from_wire(data)
    response = dns.message.make_response(query)

    question = query.question[0].name.to_text()
    if question in mapping:
        response.answer.append(
            dns.rrset.from_text(
                question,
                3600,
                dns.rdataclass.IN,
                dns.rdatatype.A,
                mapping[question],
            )
        )
    else:
        response.set_rcode(dns.rcode.NXDOMAIN)

    sock.sendto(response.to_wire(), addr)


is_running = True


def start_dns_server(host="127.0.0.1", port=53535):
    global is_running
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((host, port))
    sock.settimeout(1)
    print(f"Starting DNS server on {port}...")

    def signal_handler(sig, frame):
        print("\nShutting down DNS server...")
        global is_running
        is_running = False
        sock.close()
        sys.exit(0)

    signal.signal(signal.SIGINT, signal_handler)

    while is_running:
        try:
            data, addr = sock.recvfrom(512)
            handle_query(data, addr, sock)
        except TimeoutError:
            print("Waiting")


if __name__ == "__main__":
    port = int(sys.argv[1])
    start_dns_server(port=port)
