#!/bin/sh

sh update.sh
swig -outdir csharp/csharp -o csharp/native/csharpdriver_wrap.cpp -c++ -csharp -namespace djondb driver.i

cd csharp
