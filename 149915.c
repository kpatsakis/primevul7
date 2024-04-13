add_char_opt_map_info(OptMapInfo* map, UChar c, OnigEncoding enc)
{
  if (map->map[c] == 0) {
    map->map[c] = 1;
    map->value += map_position_value(enc, c);
  }
}