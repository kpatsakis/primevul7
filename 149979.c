onig_free_body(regex_t* reg)
{
  if (IS_NOT_NULL(reg)) {
    if (IS_NOT_NULL(reg->p))                xfree(reg->p);
    if (IS_NOT_NULL(reg->exact))            xfree(reg->exact);
    if (IS_NOT_NULL(reg->int_map))          xfree(reg->int_map);
    if (IS_NOT_NULL(reg->int_map_backward)) xfree(reg->int_map_backward);
    if (IS_NOT_NULL(reg->repeat_range))     xfree(reg->repeat_range);
    if (IS_NOT_NULL(reg->chain))            onig_free(reg->chain);

#ifdef USE_NAMED_GROUP
    onig_names_free(reg);
#endif
  }
}