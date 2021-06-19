set -e

LLVM_INSTALL_DIR=""
PATH_LIB="build/lib/libCfgPrinter.so"
OPT="opt"
CLANG="clang"

# Create the build
if [ ! -e "build" ]; then
    echo "Creating directory build"
    mkdir build
fi

echo "Building the cfgPrinter pass"
cmake -DLLVM_INSTALL_DIR=$LLVM_INSTALL_DIR -G "Unix Makefiles" -B build/ .
cd build
cmake --build .
cd ..

echo "Executing the pass"
EXAMPLE=examples/file

# analysis pass
$CLANG -fno-discard-value-names -Xclang -disable-O0-optnone -S -emit-llvm $EXAMPLE".c" -o $EXAMPLE".ll"
$OPT -S -mem2reg $EXAMPLE".ll" -o $EXAMPLE"_opt.ll"
$OPT -instnamer -load-pass-plugin $PATH_LIB -passes="cfgPrinter" $EXAMPLE"_opt.ll"
