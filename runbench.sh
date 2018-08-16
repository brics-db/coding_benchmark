#!/bin/bash
pushd build/Release
make -j$(nproc) && (reset; ./benchmark16 1> >(tee benchmark.out) 2> >(tee benchmark.err >&2))
popd

