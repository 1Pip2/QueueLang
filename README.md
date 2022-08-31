# QueueLang
QueueLang is a language that is:
  - Interpreted and compiles to bytecode AheadOfTime
  - 100% queue based at runtime
  - It is self-compiled whilst the VirtualMachine is written in c
## The Concept
All instructions(+, -, if, else, ..), data(Numbers, Strings, ..) and results of instructions get enqueued.
For example, "(7 - 4) * (8 % 3)" would be written as: "- 7 4 % 8 3 *;"
First instructions and data will be enqueued.
\[-, 7, 4, %, 8, 3, *]

Then the ';' will count the number of instructions(3 in this case) and will execute the that many times:
\[-, 7, 4, %, 8, 3, *] -> - 7, 4 = 3
\[%, 8, 3, *, 3] -> % 8, 3 = 2
\[*, 3, 2] -> * 3, 2 = 6
\[6]
As you can see, the operands where dequeued and the result enqueued. Note that executing data will result in a runtime error.
