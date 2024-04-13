const char *MirrorJob::SetRecursionMode(const char *m)
{
   struct { const char name[8]; recursion_mode_t mode; } map[]={
      {"always", RECURSION_ALWAYS},
      {"never",  RECURSION_NEVER},
      {"missing",RECURSION_MISSING},
      {"newer",  RECURSION_NEWER},
   };
   unsigned i;
   for(i=0; i<sizeof(map)/sizeof(map[0]); i++) {
      if(!strcasecmp(m,map[i].name)) {
	 recursion_mode=map[i].mode;
	 return 0;
      }
   }
   xstring list(map[0].name);
   for(i=1; i<sizeof(map)/sizeof(map[0]); i++)
      list.append(", ").append(map[i].name);
   return xstring::format(_("%s must be one of: %s"),"--recursion",list.get());
}