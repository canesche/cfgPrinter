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
$OPT -load-pass-plugin $PATH_LIB -passes="cfgPrinter" $EXAMPLE"_opt.ll"

#$OPT --dot-cfg $EXAMPLE"_opt.ll"

BENCH=(
    Bubblesort
    #FloatMM
    #IntMM
    #Oscar
    #Perm
    #Puzzle
    #Queens
    #Quicksort
    #ealMM
    #Towers
    #Treesort
)

for ((i = 0; i < ${#BENCH[@]}; i++)); do
    EXAMPLE=examples/${BENCH[i]}
    $CLANG -fno-discard-value-names -Xclang -disable-O0-optnone -S -emit-llvm Stanford/${BENCH[i]}".c" -o $EXAMPLE".ll"
    $OPT -S -mem2reg $EXAMPLE".ll" -o $EXAMPLE"_opt.ll"
    
    $OPT --dot-cfg $EXAMPLE"_opt.ll"
    
    $OPT -load-pass-plugin $PATH_LIB -passes="cfgPrinter" $EXAMPLE"_opt.ll"
done
