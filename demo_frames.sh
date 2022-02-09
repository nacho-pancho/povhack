#/usr/bin/bash
make -C src
mkdir -p demo
./src/hack2frames data/game.vt100 demo/game
cd demo
for i in game*.tty
do
	../src/frame2pov $i ${i/tty/pov}
done
cd ..
