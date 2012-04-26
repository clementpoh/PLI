break ins_func
break pretty_stmts
break pretty_expr

run -p tests/bad3.pli12 out
display *parsed_prog->f_first
display *parsed_prog->f->rest->f_first
