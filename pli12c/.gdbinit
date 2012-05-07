set history save on
set history expansion on

break verify_binop

run tests/bad1.pli12 
display *e
