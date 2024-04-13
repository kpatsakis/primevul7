onig_region_copy(OnigRegion* to, const OnigRegion* from)
{
#define RREGC_SIZE   (sizeof(int) * from->num_regs)
  int i, r;

  if (to == from) return;

  r = onig_region_resize(to, from->num_regs);
  if (r) return;

  for (i = 0; i < from->num_regs; i++) {
    to->beg[i] = from->beg[i];
    to->end[i] = from->end[i];
  }
  to->num_regs = from->num_regs;

#ifdef USE_CAPTURE_HISTORY
  history_root_free(to);

  if (IS_NOT_NULL(from->history_root)) {
    to->history_root = history_tree_clone(from->history_root);
  }
#endif
}