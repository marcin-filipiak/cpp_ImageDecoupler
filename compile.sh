#!/bin/sh
rm decoupler
clear
echo "----Kompilacja programu----"
g++ -o decoupler decoupler.cpp -lcrypto
echo "---Kompilacja ukonczona----"
echo "---Uruchamianie programu---"
chmod +x decoupler
./decoupler
