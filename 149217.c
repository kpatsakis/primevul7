static int backref_rel_to_abs(int rel_no, ScanEnv* env)
{
  if (rel_no > 0) {
    return env->num_mem + rel_no;
  }
  else {
    return env->num_mem + 1 + rel_no;
  }
}