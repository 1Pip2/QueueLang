# QueueLang
QueueLang is a language that is:
  - Interpreted and compiles to bytecode AheadOfTime
  - 100% queue based at runtime
  - Is self-compiled whilst the VirtualMachine is written in c
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
### Functions
A function has the following layout:<br />

>fun \<identifier>: \<identifier>, \<identifier>, ... [<br />
&emsp;\<statement> \<statement> ...<br />
]

It is called using the 'call' operation:<br />
> call \<identifier> do

'call' will dequeue the function arguments one by one.<br />
For example:
>call min 5 4 do

Will result in 4 being enqueued.

### Statements
A statement is either an operation or a litteral.

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

Queue:
- ';' countes all enqueued operations and executes the front of the queue that many times 
- 'do' executes the front of the queue
- 'req' moves the front element of the queue to the back
- 'dup' will enqueue a copy of the reference to the front element in the queue
- 'cpy' will enqueue a copy of the front element in the queue
- 'rm' removes the element at the front of the queue

### If-statements
> if [<br />
  &emsp;\<code>  
] <br />
  &emsp;\<code><br />
endif

The if keyword expects a Boolean at the front of the queue. If it is True is executes the code within the brackets and jumps to the endif. If it is False it executes the code after the closing bracket. Note that a endif can be used by multiple If-statements.

> if [] <br />
if [] <br />
if [] <br />
endif

This is how you can handle else if in QueueLang.

### While-loops
> while [] []

The while keyword executes the first body. This must result in a bool a the front of the queue. If it is True, the second body gets executed and the process repeats. If it False the second body gets skiped and execution of the program continues.


### Litterals
- Int: 0, 5, -4, 100, ...
- Bool: True, False
- Array: {Litteral Litteral ...}
