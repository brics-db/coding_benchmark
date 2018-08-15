#!/bin/bash
pushd build/Release
make -j$(nproc) && (reset; ./benchmark 1> >(tee benchmark.out) 2> >(tee benchmark.err >&2); ./benchmark_novec 1> >(tee benchmark_novec.out) 2> >(tee benchmark_novec.err >&2))
popd

