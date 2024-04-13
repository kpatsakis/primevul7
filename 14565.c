static int l_strton (const TValue *obj, TValue *result) {
  lua_assert(obj != result);
  if (!cvt2num(obj))  /* is object not a string? */
    return 0;
  else
    return (luaO_str2num(svalue(obj), result) == vslen(obj) + 1);
}