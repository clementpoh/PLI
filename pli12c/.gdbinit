set history save on
set history expansion on

break analyze.c:93

run tests/fac.pli12 ../out
# display *parsed_prog->f_first->stmts->s_first
display line
