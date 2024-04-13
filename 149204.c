save_entry(ScanEnv* env, enum SaveType type, int* id)
{
  int nid = env->save_num;

#if 0
  if (IS_NULL(env->saves)) {
    int n = 10;
    env->saves = (SaveItem* )xmalloc(sizeof(SaveItem) * n);
    CHECK_NULL_RETURN_MEMERR(env->saves);
    env->save_alloc_num = n;
  }
  else if (env->save_alloc_num <= nid) {
    int n = env->save_alloc_num * 2;
    SaveItem* p = (SaveItem* )xrealloc(env->saves, sizeof(SaveItem) * n);
    CHECK_NULL_RETURN_MEMERR(p);
    env->saves = p;
    env->save_alloc_num = n;
  }

  env->saves[nid].type = type;
#endif

  env->save_num++;
  *id = nid;
  return 0;
}