#/usr/bin/bash
make -C src
mkdir -p demo
./src/povhack -o demo/game data/nh.vt100
