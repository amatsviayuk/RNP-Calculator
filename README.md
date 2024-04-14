# RNP-Calculator
Basically, that's a simple integer calculator that uses postfix notation to handle precedence of operators, functions and parentheses.

The following operations are available, in the ascending order of priority:

1)
   a + b, a - b ;
2)
   a * b, a / b (You cannot divide by 0);
4)
   IF(a, b, c) -if a > 0 return b, otherwise c,
   N a -unary negation, i.e. -a,
   MIN( a1, a2, ... ), MAX(a1, a2, ...) -functions MIN and MAX do not have a restriction on the number of parameters;
5) ( ... ) -parentheses.
