propname2ctype(ScanEnv* env, const char* propname)
{
  UChar* name = (UChar* )propname;
  int ctype = env->enc->property_name_to_ctype(ONIG_ENCODING_ASCII,
      name, name + strlen(propname));
  return ctype;
}