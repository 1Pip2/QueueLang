# QueueLang
QueueLang is a language that is:
  - Interpreted and compiles to bytecode AheadOfTime
  - 100% queue based at runtime
  - Is self-compiled whilst the VirtualMachine is written in c
## The Concept
All operations(+, -, if, else, ..), data(Numbers, Strings, ..) and results of operations get enqueued. <br />
For example, "(7 - 4) * (8 % 3)" would be written as: "- 7 4 % 8 3 *;"

First operations and data will be enqueued.<br />
\[-, 7, 4, %, 8, 3, *]

Then the ';' will count the number of operations(3 in this case) and will execute the front of the queue that many times:<br />

\[-, 7, 4, %, 8, 3, *] -> - 7, 4 = 3<br />
\[%, 8, 3, *, 3] -> % 8, 3 = 2<br />
\[*, 3, 2] -> * 3, 2 = 6<br />
\[6]<br />

As you can see, the operands where dequeued and the result enqueued. Note that executing data will result in a runtime error.
