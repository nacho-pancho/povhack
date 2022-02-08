#!/usr/bin/bash
povray +i$1 +o${1/pov/png} +w1920 +h1080 +a0.1
