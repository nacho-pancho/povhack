#/usr/bin/bash
make -C src
mkdir -p demo
/src/povhack data/game1.vt demo/game
for i in demo/*.pov
do
	scripts/render_hd.sh $i
done
cd ..
