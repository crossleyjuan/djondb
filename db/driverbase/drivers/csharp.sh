#!/bin/sh

sh update.sh
swig2.0 -outdir csharp/csharp -o csharp/native/csharpdriver_wrap.cpp -c++ -csharp -namespace djondb driver.i

cd csharp
