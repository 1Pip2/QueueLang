# QueueLang
QueueLang is a language that is:
  - A WORK IN PROGRESS!
  - Interpreted and compiled to bytecode AheadOfTime
  - 100% queue based at runtime
## The Concept
All operations(+, -, if, else, ..), data(Numbers, Strings, ..) and results of operations get enqueued. <br />
For example, "(7 - 4) * (8 % 3)" would be written as: "- 7 4 % 8 3 *;"

First operations and data will be enqueued.<br />
>\[-, 7, 4, %, 8, 3, *]

Then the ';' will count the number of operations(3 in this case) and will execute the front of the queue that many times:<br />

>\[-, 7, 4, %, 8, 3, *] -> - 7, 4 = 3<br />
\[%, 8, 3, *, 3] -> % 8, 3 = 2<br />
\[*, 3, 2] -> * 3, 2 = 6<br />
\[6]<br />

As you can see, the operands where dequeued and the result enqueued. Note that executing data will result in a runtime error.

# Syntax
### Prog
This is where the execution of the program starts
> prog: [ \<code> ]

### Statements
A statement is either an operation, keyword or a litteral.

### Functions
A function is called using the following syntax:
> call(\<identifier>) \<arg1> \<arg2> ... do

### Built in functions
The following functions are built into the language:
- 'get' arg1: array, arg2: int; returns arg1\[arg2]
- 'append' arg1: array, arg2: int; appends arg2 to arg1

### Operations
Arithmetic:
- '\+' will add the front two elements of the queue
- '-' will subtract the front two elements of the queue
- '*' will multiply the front two elements of the queue
- '/' will divide the front two elements of the queue
- '%' will mod the front two elements of the queue

Conditional:
- '=' will check if the front two elements of the queue are equal
- '>' will check if the front element is greater than the second
- '<' will check if the front element is less than the second
- '>=' will check if the front element is greater or equal
- '<=' will check if the front element is less or equal

- 'or' will check if one of the front two elements of the queue is True
- 'xor will check if one of the front two elements of the queue is True and one is False
- 'and' will check if both of the front two elements of the queue are True
- 'not' will check if the front element of the queue is False

Queue:
- ';' countes all enqueued operations and executes the front of the queue that many times 
- 'do' executes the front of the queue
- 'req' moves the front element of the queue to the back
- 'dup' will enqueue a copy of the reference to the front element in the queue
- 'cpy' will enqueue a copy of the front element in the queue
- 'rm' removes the element at the front of the queue

### Variables
'set(\<identifier>)' sets that identifier to the value at the front of the queue.
To modifiy the value of a varialbe simply use 'set' again.
'let' defines a constant and trying to 'set' a constant will result in an error.
To use a variable, type the name of said variable(\<identifier>). This will enqueue the variable value.
> set(a) 5 do<br />
> set(a) 4 do<br />
. a do  
This example code prints 4 to stdout.

### If-statements
> if [<br />
  &emsp;\<code>  
] <br />
  &emsp;\<code><br />
endif

The if keyword expects a Boolean at the front of the queue. If it is True is executes the code within the brackets and jumps to the endif. If it is False it executes the code after the closing bracket. Note that a endif can be used by multiple If-statements.

> if [ \<code> ] <br />
if [ \<code> ] <br />
if [ \<code> ] <br />
endif

This is how you can handle else if in QueueLang.

### While-loops
> while [ \<code> ] [ \<code> ]

The while keyword executes the first body. It then expects a bool a the front of the queue. If it is True, the second body gets executed and execution jumps back to the start of the while-statement. If it is False the second body gets skiped and execution of the program continues.


### Litterals
- Int: 0, 5, -4, 100, ...
- Bool: True, False
- Array: {Litteral Litteral ...}
