is_case_fold_variable_len(int item_num, OnigCaseFoldCodeItem items[],
			  int slen)
{
  int i;

  for (i = 0; i < item_num; i++) {
    if (items[i].byte_len != slen) {
      return 1;
    }
    if (items[i].code_len != 1) {
      return 1;
    }
  }
  return 0;
}