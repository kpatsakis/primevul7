distance_add(OnigDistance d1, OnigDistance d2)
{
  if (d1 == ONIG_INFINITE_DISTANCE || d2 == ONIG_INFINITE_DISTANCE)
    return ONIG_INFINITE_DISTANCE;
  else {
    if (d1 <= ONIG_INFINITE_DISTANCE - d2) return d1 + d2;
    else return ONIG_INFINITE_DISTANCE;
  }
}