onigenc_get_case_fold_codes_by_str_with_map(int map_size,
    const OnigPairCaseFoldCodes map[],
    int ess_tsett_flag, OnigCaseFoldType flag ARG_UNUSED,
    const OnigUChar* p, const OnigUChar* end, OnigCaseFoldCodeItem items[])
{
  if (0x41 <= *p && *p <= 0x5a) {
    items[0].byte_len = 1;
    items[0].code_len = 1;
    items[0].code[0] = (OnigCodePoint )(*p + 0x20);
    if (*p == 0x53 && ess_tsett_flag != 0 && end > p + 1
	&& (*(p+1) == 0x53 || *(p+1) == 0x73)) {
      /* SS */
      items[1].byte_len = 2;
      items[1].code_len = 1;
      items[1].code[0] = (OnigCodePoint )0xdf;
      return 2;
    }
    else
      return 1;
  }
  else if (0x61 <= *p && *p <= 0x7a) {
    items[0].byte_len = 1;
    items[0].code_len = 1;
    items[0].code[0] = (OnigCodePoint )(*p - 0x20);
    if (*p == 0x73 && ess_tsett_flag != 0 && end > p + 1
	&& (*(p+1) == 0x73 || *(p+1) == 0x53)) {
      /* ss */
      items[1].byte_len = 2;
      items[1].code_len = 1;
      items[1].code[0] = (OnigCodePoint )0xdf;
      return 2;
    }
    else
      return 1;
  }
  else if (*p == 0xdf && ess_tsett_flag != 0) {
    items[0].byte_len = 1;
    items[0].code_len = 2;
    items[0].code[0] = (OnigCodePoint )'s';
    items[0].code[1] = (OnigCodePoint )'s';

    items[1].byte_len = 1;
    items[1].code_len = 2;
    items[1].code[0] = (OnigCodePoint )'S';
    items[1].code[1] = (OnigCodePoint )'S';

    items[2].byte_len = 1;
    items[2].code_len = 2;
    items[2].code[0] = (OnigCodePoint )'s';
    items[2].code[1] = (OnigCodePoint )'S';

    items[3].byte_len = 1;
    items[3].code_len = 2;
    items[3].code[0] = (OnigCodePoint )'S';
    items[3].code[1] = (OnigCodePoint )'s';

    return 4;
  }
  else {
    int i;

    for (i = 0; i < map_size; i++) {
      if (*p == map[i].from) {
	items[0].byte_len = 1;
	items[0].code_len = 1;
	items[0].code[0] = map[i].to;
	return 1;
      }
      else if (*p == map[i].to) {
	items[0].byte_len = 1;
	items[0].code_len = 1;
	items[0].code[0] = map[i].from;
	return 1;
      }
    }
  }

  return 0;
}