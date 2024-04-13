add_len_mml(MinMaxLen* to, OnigDistance len)
{
  to->min = distance_add(to->min, len);
  to->max = distance_add(to->max, len);
}