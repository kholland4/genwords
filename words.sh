#!/bin/bash
cat /usr/share/dict/words | grep -P ^[a-z]{$1}$ | sort -R
