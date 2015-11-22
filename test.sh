#!/bin/bash

make all && cat 180sec_2chan_5kHz_pm_4kHz_FM_4Hz_Trigger1Hz.mdat | ./qmread | less
