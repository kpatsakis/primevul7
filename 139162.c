void STDCALL mysql_get_character_set_info(MYSQL *mysql, MY_CHARSET_INFO *csinfo)
{
  csinfo->number   = mysql->charset->number;
  csinfo->state    = mysql->charset->state;
  csinfo->csname   = mysql->charset->csname;
  csinfo->name     = mysql->charset->name;
  csinfo->comment  = mysql->charset->comment;
  csinfo->mbminlen = mysql->charset->mbminlen;
  csinfo->mbmaxlen = mysql->charset->mbmaxlen;

  if (mysql->options.charset_dir)
    csinfo->dir = mysql->options.charset_dir;
  else
    csinfo->dir = charsets_dir;
}