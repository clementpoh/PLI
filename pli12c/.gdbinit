set history save on
set history expansion on

break translate_binop

run tests/hello.pli12  ../out.t12
