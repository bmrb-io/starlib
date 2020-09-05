#!/bin/bash

# Tar up results we want sent to us and logged.

# NOTE: RESULT MUST BE NAMED 'results.tar.gz' for NMI to pick it up:
#
# (maybe should be pruned down to just the relevant files later:)
tar cvf - . | gzip - --stdout > results.tar.gz

