cmake -B build \
  -DCMAKE_MAKE_PROGRAM="/usr/bin/make" \
  -DCMAKE_C_COMPILER="/usr/bin/gcc" \
  -DCMAKE_CXX_COMPILER="/usr/bin/g++" \
  --preset=default .