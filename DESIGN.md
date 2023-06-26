# DESIGN.md - Design spec for FoldPropPass - A Constant Folding and Constant Propogation LLVM Pass
#
# William Dinauer, Dartmouth CS57 Spring 2022

### Design Overview

This LLVM Pass is designed to perform constant folding and constant propagation
on generated IR files. 
First, in the *runOnFunction* method, we loop over every basic block
and every instruction in the basic block to check each individual instruction for
constant propagation/folding.

We check if every instruction is either of the following:

1. Binary instruction
    (ex. adding/subtracting/multiplying/dividing two integers)
2. Comparison instruction
    (ex. comparing if an integer is (greater than)/(less than)/(equal to) another)

For either type of instruction, we first check if both the right and left operands are constants.
If so, any future use of that variable in the IR can be folded into the resulting value.
For binary instructions, we store the result of the operation as an integer, then use the
LLVM-defined function *ReplaceInstWithValue* to replace any future uses of the variable with
the calculated integer. *ReplaceInstWithValue* will also delete the now obselete instruction
that has been folded.
For comparison instructions, we store the result of the comparison as a boolean, then use
the same LLVM function *ReplaceInstWithValue* to replace the comparison with either 'true'
or 'false'.

As we run the pass, we also have a boolean variable called "changes" which we turn to true if
*ReplaceInstWithValue* is ever called during the pass of the function. If "changes" is true by
the end of our pass, meaning the IR instruction set was changed, we recursively call the
*runOnFunction* method on the function to perform another pass. Once we complete a pass without making
any changes, our pass concludes and prints out the optimized IR.

### Example Usage

The pass can be called by using 'opt' and referring to the pass as 'foldproppass'.
For example, to run the pass on a generated ir file named 'test.ll', first
run 'make' to generate the FoldPropPass.so library. Then, run the command:

opt-10 -S -load ./FoldPropPass.so -foldproppass < mytest.ll > /dev/null

The resulting output is optimized IR that has been constantly folded and propagated
