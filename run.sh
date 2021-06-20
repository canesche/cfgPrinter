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

# Create the results
if [ ! -e "results" ]; then
    mkdir results
fi

echo "Building the cfgPrinter pass"
cmake -DLLVM_INSTALL_DIR=$LLVM_INSTALL_DIR -G "Unix Makefiles" -B build/ .
cd build
cmake --build .
cd ..

echo ""
echo "Benchmarks toys"

EXAMPLE=benchmarks/test

if [ ! -e "results/test" ]; then
    mkdir "results/test"
fi

echo "Executing the pass for bench: "$EXAMPLE
# analysis pass
$CLANG -Wno-everything -fno-discard-value-names -Xclang -disable-O0-optnone -S -emit-llvm $EXAMPLE".c" -o $EXAMPLE".ll"
$OPT -S -mem2reg $EXAMPLE".ll" -o $EXAMPLE"_opt.ll"
$OPT -load-pass-plugin $PATH_LIB -passes="cfgPrinter" $EXAMPLE"_opt.ll" -disable-output

mv *dot "results/test"

echo ""
echo "Benchmarks Stanford"

BENCH=(
    Bubblesort
    FloatMM
    IntMM
    Oscar
    Perm
    Puzzle
    Queens
    Quicksort
    RealMM
    Towers
    Treesort
)

for ((i = 0; i < ${#BENCH[@]}; i++)); do
	
    if [ ! -e "results/Stanford" ]; then
	    mkdir "results/Stanford"
	fi

	if [ ! -e "results/Stanford/${BENCH[i]}" ]; then
	    mkdir "results/Stanford/${BENCH[i]}"
	fi

    echo "Executing the pass for bench: ${BENCH[i]}"

    EXAMPLE=benchmarks/${BENCH[i]}
    $CLANG -Wno-everything -fno-discard-value-names -Xclang -disable-O0-optnone -S -emit-llvm benchmarks/Stanford/${BENCH[i]}".c" -o $EXAMPLE".ll"
    $OPT -S -instnamer -mem2reg $EXAMPLE".ll" -o $EXAMPLE"_opt.ll"
    
    #$OPT --dot-cfg $EXAMPLE"_opt.ll"
    
    $OPT -load-pass-plugin $PATH_LIB -passes="cfgPrinter" $EXAMPLE"_opt.ll" -disable-output
    
    mv *.dot "results/Stanford/${BENCH[i]}"
done

rm benchmarks/*.ll
