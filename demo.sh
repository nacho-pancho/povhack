#/usr/bin/bash
make -C src
mkdir -p demo
./src/hack2frames data/game.vt100 demo/game
cd demo
for i in game*.tty
do
	../src/frame2pov $i ${i/tty/pov}
	#povray +i${i/tty/pov} +o${i/tty/png} +a0.1 +w960 +h540
	povray +i${i/tty/pov} +o${i/tty/png} +a0.1 +w480 +h270 -D 
done
cd ..
