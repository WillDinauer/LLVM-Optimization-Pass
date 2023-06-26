# Optimize Test Script
# Takes in an IR file and optimizes it, printing out the optimized IR.
# William Dinauer, Dartmouth CS57, Spring 2022

opt-10 -S -load=./FoldPropPass.so --foldproppass < $1 > /dev/null
