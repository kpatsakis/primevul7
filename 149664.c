unset_addr_list_fix(UnsetAddrList* uslist, regex_t* reg)
{
  int i, offset;
  EncloseNode* en;
  AbsAddrType addr;

  for (i = 0; i < uslist->num; i++) {
    en = NENCLOSE(uslist->us[i].target);
    if (! IS_ENCLOSE_ADDR_FIXED(en)) return ONIGERR_PARSER_BUG;
    addr = en->call_addr;
    offset = uslist->us[i].offset;

    BBUF_WRITE(reg, offset, &addr, SIZE_ABSADDR);
  }
  return 0;
}