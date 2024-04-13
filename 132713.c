static LEX_CSTRING get_token(const char **str)
{
  LEX_CSTRING ret = { NULL, 0 };

  if (*str != NULL)
  {
    ret.str= *str;

    if (*ret.str != '\0')
    {
      /* Find a param delimiter. */
      while (**str && **str != ';') (*str)++;

      ret.length= *str - ret.str;

      /* Skip a delimiter. */
      if (**str == ';') (*str)++;
    }
  }

  return ret;
}