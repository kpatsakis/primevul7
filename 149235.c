i_names(UChar* key ARG_UNUSED, NameEntry* e, INamesArg* arg)
{
  int r = (*(arg->func))(e->name,
                         e->name + e->name_len,
                         e->back_num,
                         (e->back_num > 1 ? e->back_refs : &(e->back_ref1)),
                         arg->reg, arg->arg);
  if (r != 0) {
    arg->ret = r;
    return ST_STOP;
  }
  return ST_CONTINUE;
}