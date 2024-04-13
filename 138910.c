dump_desc(const char *desc, const char *type)
{
  time_t now = time(NULL);
  tor_assert(desc);
  tor_assert(type);
  if (!last_desc_dumped || last_desc_dumped + 60 < now) {
    char *debugfile = get_datadir_fname("unparseable-desc");
    size_t filelen = 50 + strlen(type) + strlen(desc);
    char *content = tor_malloc_zero(filelen);
    tor_snprintf(content, filelen, "Unable to parse descriptor of type "
                 "%s:\n%s", type, desc);
    write_str_to_file(debugfile, content, 0);
    log_info(LD_DIR, "Unable to parse descriptor of type %s. See file "
             "unparseable-desc in data directory for details.", type);
    tor_free(content);
    tor_free(debugfile);
    last_desc_dumped = now;
  }
}