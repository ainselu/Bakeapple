# Bakeapple Bytecode ISA
Hello! This file goes over all instructions in each version of the Bakeapple VM and their use, syntax and etc. If you find any issues with any of these, please feel free to open an issue on our GitHub.

## Syntax
The syntax of Bakeapple bytecode is inspired by ARM assembly, and follows a similar syntax, being [instruction] [destination] [source], without commas. Anything enclosed in double quotes "" or curly braces {} are treated as one argument. Some instructions may break apart the source into multiple arguments following the rules of tokenization for every other instruction, and these are usually enclosed in curly braces, but double quotes are fine as well. Unlike ARM assembly, when referencing the address of a register, it is not enclosed in square brackets, and you will typically just simply reference the register's name to get the value it contains. It is case sensitive, all instructions must be capitalized, and all registers will be lowercase. All instructions must have at least two trailing arguments seperated by a space, and all instructions must be on a new line.

An example is as follows:

```
MOV x0 1
```

which moves the value 1 into register x0, you could also do:

```
MOV x0 x1
```

and whatever value you had stored in x1 will be copied into x0.

Although in simple scripting it will not be permitted, when loading a file into memory, you must use "GLOBAL .(label)" to denote the starting point of your program. In scripting labels are not permitted and it will simply start from the beginning of your script. A list of all registers included by default are as follows:

General purpose registers: x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14
Special registers: sy, rv

Registers are the only way to store data, and memory is typically only accessed through a register pointing to a memory address. The sy register stores whether or not the last instruction executed was successful, and the rv register only stores the return value of a label.

Also, all labels must be prefixed with a period, and end with a colon. They can be used for functions, branching, loops, or etc. They're case sensitive; a label ".ex:" is not the same as ".EX:". In libraries, it is noteworthy that unline in regular programs, each label defined as global simply means a label that can be referenced by another program requiring that library, other labels not marked as global can still be accessed internally by other labels in the library itself. The file extension for bytecode is .bkl, while for libraries it is .blib.

So, a simple hello world program would look like this:

```
GLOBAL .main

.main:
WRT "hello world" 1
END
```

The types currently valid are as follows:

* nil
* integer
* string
* boolean
* character
* 
More are intended to be added soon; the vm is still in its early stages of development.

End denotes the end of a label, the loader will load all instructions as part of the given label from the start of the label definition to the end instrucion. In terms of syntax, this is all for now.

## bake-vm v0.1.0
This goes over all instructions introduced in bake-vm v0.1.0.

* GLOBAL [label] : Defines a label as the starting point for program execution. For libraries, it denotes all labels that can be accessed externally
* END : Denotes the end of a label
  
* MOV [dest] [src] : Replaces the value of dest with the value of val
* ADD [dest] [src] : Adds the value of src to the value of dest
* SUB [dest] [src] : Subtracts the value of src from the value of dest
* MUL [dest] [src] : Multiplies the value of dest by the value of src
* DIV [dest] [src] : Divides the value of dest by the value of src
  
* WRT [src] [paramm. 1] : Prints the value of src with the formatting specified by the parameter, all options for formatting are as follows:
  * 0: Print to stdout with no formatting
  * 1: Print to stdout with a newline
  * 2: Print to stderr with no formatting
  * 3: Print to stderr with a newline
* NOP [0] [0] : Does nothing
* INP [register] [0] : Get user input and store it as a string pointer in the given register
  
* RET [src] [type] : Returns out of the label to the one which called it with the specified value and type, stored in rv
* CALL [label] [{args}] : Calls the label with the specified arguments
* JMP [label] [0] : Unconditional jump to the specified label
* ACP [register] [parameter number] : Makes a pointer to the given label parameter in the given register
* SRT [type] [0] : Sets the return type of the current label
* CHT [type] [register] : Sets the type of the given register to the given type, use with caution

* CMP [val. 1] [val. 2] : If dest == src, sy is set to 1, else, 0
* CMG [val. 1] [val. 2] : If dest > src, sy is set to 1, else, 0
* CML [val. 1] [val. 2] : If dest < src, sy is set to 1, else, 0
* CGE [val. 1] [val. 2] : If dest >= src, sy is set to 1, else, 0
* CLE [val. 1] [val. 2] : If dest <= src, sy is set to 1, else, 0
  
* JE [label] [0] : If sy == 1, jump to the specified label
* JNE [label] [0] : If sy == 0, jump to the specified label

* FOR [{start, condition, increment}] [label name] : For loop

* REQ [file name] [library reference] : Loads a library file into memory; the library reference is the name by which the library can be referenced as in LIBC calls
* LIBC [library reference] [label name] : Calls a label from a library
  
* STR [register] [{type, value}] : Stores the given value in memmory, then makes a pointer to it of the given type in the given register
* DEL [register] [0] : Frees the memory the given register is pointed to; this is necessary to perform when you're done with memory allocated with STR to avoid memory leaks as it is not done automatically

* RAC [new register] [0] : Allocates memory for a new register; should be deleted after use with DRG
* DRG [register] [0] : Deallocates a register from memory
