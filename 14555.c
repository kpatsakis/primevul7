static int isinstack (CallInfo *ci, const TValue *o) {
  StkId pos;
  for (pos = ci->func + 1; pos < ci->top; pos++) {
    if (o == s2v(pos))
      return 1;
  }
  return 0;  /* not found */
}