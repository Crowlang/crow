# Testing Crow version 1e3174c
## count.cro
```
=== Counting to 1000000
=== Watch memory usage for memory holes
=== While
1000000
=== do-while
1000000
=== loop
1000000
=== do-times
1000000
=== each
1000000
=== each-with-iterator
1000000
```
**Test count.cro passed**
## hello.cro
```
=== Anonymous string
Hello world
=== From variable
Hello world
=== From function
Hello world
=== From anonymous function
Hello world
```
**Test hello.cro failed**
Expected
```

```
## ndimen-array.cro
```
=== Array from variable
1
2
3

4
5
6

7
8
9

Hello
world

=== Anonymous array
1
2
3

Hello
world
```
**Test ndimen-array.cro passed**
