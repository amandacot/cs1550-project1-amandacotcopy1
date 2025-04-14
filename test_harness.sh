#!/bin/bash

make
echo "======================================"
echo "Interleaved activation: S1 S2 T0 S3 T1"
echo "======================================"
ACTIVATE_SEQUENCE=S1,S2,T0,S3,T1 ./tutor_sim

echo "======================================"
echo "Interleaved activation: T1 S10 S11 T2 S12"
echo "======================================"
ACTIVATE_SEQUENCE=T1,S10,S11,T2,S12 ./tutor_sim

echo "======================================"
echo "Interleaved activation: S0 T0 S1 T1"
echo "======================================"
ACTIVATE_SEQUENCE=S0,T0,S1,T1 ./tutor_sim

echo "======================================"
echo "Only tutors: T0,T1,T2"
echo "======================================"
ACTIVATE_SEQUENCE=T0,T1,T2 ./tutor_sim
