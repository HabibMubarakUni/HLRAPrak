#!/bin/bash

# Vc library paths
export LD_LIBRARY_PATH=~/Vc/lib:$LD_LIBRARY_PATH
export PATH=~/Vc/bin:$PATH
export CPLUS_INCLUDE_PATH=~/Vc/include:$CPLUS_INCLUDE_PATH

# TBB library paths
export TBB_HOME=~/TBB_Lib/tbb
export LD_LIBRARY_PATH=$TBB_HOME/lib64:$LD_LIBRARY_PATH
export CPLUS_INCLUDE_PATH=$TBB_HOME/include:$CPLUS_INCLUDE_PATH
