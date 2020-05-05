#!/bin/sh

echo "---> Starting xvfb display server"
Xvfb :99 -ac -screen 0 "$XVFB_WHD" -nolisten tcp&