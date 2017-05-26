file_input: (NEWLINE | stmt)* ENDMARKER

stmt: simple_stmt | compound_stmt
simple_stmt: small_stmt (';' small_stmt)* [';'] NEWLINE
small_stmt: (expr_stmt | flow_stmt | func_stmt)

func_stmt: print_stmt | exec_stmt | range_stmt | input_stmt | len_stmt
print_stmt: 'print' ('(' [ testlist ] ')') | testlist
exec_stmt: 'exec' expr ['in' test [',' test]]
range_stmt: 'range' '(' test [',' test] ')'
input_stmt: 'input' '(' [test]')'
len_stmt: 'len' '(' test ')'

expr_stmt: testlist ('=' (testlist))*
flow_stmt: break_stmt | continue_stmt | return_stmt | raise_stmt
raise_stmt: 'raise' [test [',' test [',' test]]]
break_stmt: 'break'
continue_stmt: 'continue'
return_stmt: 'return' [testlist]

compound_stmt: if_stmt | while_stmt | for_stmt | with_stmt | funcdef
if_stmt: 'if' test ':' stmt+ ('elif' test ':' stmt+)* ['else' ':' stmt+]
while_stmt: 'while' test ':' stmt+ ['else' ':' stmt+]
for_stmt: 'for' exprlist 'in' testlist ':' stmt+ ['else' ':' stmt+]
with_stmt: 'with' with_item (',' with_item)*  ':' stmt+
with_item: test ['as' expr]

funcdef: 'def' NAME parameters ':' stmt+
parameters: '(' [NAME (',' NAME)* [',']] ')'

test: or_test
or_test: and_test ('or' and_test)*
and_test: not_test ('and' not_test)*
not_test: 'not' not_test | comparison
comparison: expr (comp_op expr)*
comp_op: '<'|'>'|'=='|'>='|'<='|'<>'|'!='|'in'|'not' 'in'|'is'|'is' 'not'
expr: term (('+'|'-') term)*
term: factor (('*'|'/'|'%'|'//') factor)*
factor: (('+'|'-'|'~') factor) | power
power: atom [('**'|'^') factor]
atom: ('(' [testlist] ')' |
       '[' [testlist] ']' |
       NAME [ '[' test ']' ]| NUMBER | STRING+ | boolean | func_stmt)
boolean: 'True' | 'False'
exprlist: expr (',' expr)* [',']
testlist: test (',' test)* [',']