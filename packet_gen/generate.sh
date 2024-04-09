#!/bin/bash

python3 packet_gen/main.py config client >> ./mcpprotocol/packets/generated/config_client.inl
python3 packet_gen/main.py config server >> ./mcpprotocol/packets/generated/config_server.inl
python3 packet_gen/main.py handshake client >> ./mcpprotocol/packets/generated/handshake_client.inl
python3 packet_gen/main.py handshake server >> ./mcpprotocol/packets/generated/handshake_server.inl
python3 packet_gen/main.py login client >> ./mcpprotocol/packets/generated/login_client.inl
python3 packet_gen/main.py login server >> ./mcpprotocol/packets/generated/login_server.inl
python3 packet_gen/main.py play client >> ./mcpprotocol/packets/generated/play_client.inl
python3 packet_gen/main.py play server >> ./mcpprotocol/packets/generated/play_server.inl
python3 packet_gen/main.py status client >> ./mcpprotocol/packets/generated/status_client.inl
python3 packet_gen/main.py status server >> ./mcpprotocol/packets/generated/status_server.inl