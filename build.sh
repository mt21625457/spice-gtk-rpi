#!/bin/bash
SPICE_HOME=/home/pi/spice/usr

meson _build --prefix=${SPICE_HOME}
ninja -C _build
ninja -C _build install
