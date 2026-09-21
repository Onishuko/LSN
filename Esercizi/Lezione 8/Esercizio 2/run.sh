#!/bin/bash

source config.sh

./main.exe \
    $M \
    $N \
    $R0 \
    $SIGMA0 \
    $MU0 \
    $T0 \
    $DSIGMA \
    $DMU \
    $NSTEPS \
    $NEQUIL \
    $COOLING \
    $DELTA
