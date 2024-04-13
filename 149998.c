disable_noname_group_capture(Node** root, regex_t* reg, ScanEnv* env)
{
  int r, i, pos, counter;
  BitStatusType loc;
  GroupNumRemap* map;

  map = (GroupNumRemap* )xalloca(sizeof(GroupNumRemap) * (env->num_mem + 1));
  CHECK_NULL_RETURN_MEMERR(map);
  for (i = 1; i <= env->num_mem; i++) {
    map[i].new_val = 0;
  }
  counter = 0;
  r = noname_disable_map(root, map, &counter);
  if (r != 0) return r;

  r = renumber_by_map(*root, map);
  if (r != 0) return r;

  for (i = 1, pos = 1; i <= env->num_mem; i++) {
    if (map[i].new_val > 0) {
      SCANENV_MEM_NODES(env)[pos] = SCANENV_MEM_NODES(env)[i];
      pos++;
    }
  }

  loc = env->capture_history;
  BIT_STATUS_CLEAR(env->capture_history);
  for (i = 1; i <= ONIG_MAX_CAPTURE_HISTORY_GROUP; i++) {
    if (BIT_STATUS_AT(loc, i)) {
      BIT_STATUS_ON_AT_SIMPLE(env->capture_history, map[i].new_val);
    }
  }

  env->num_mem = env->num_named;
  reg->num_mem = env->num_named;

  return onig_renumber_name_table(reg, map);
}