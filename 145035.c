  Config::Config(ParmStr name,
		 const KeyInfo * mainbegin, 
		 const KeyInfo * mainend)
    : name_(name)
    , first_(0), insert_point_(&first_), others_(0)
    , committed_(true), attached_(false)
    , md_info_list_index(-1)
    , settings_read_in_(false)
    , load_filter_hook(0)
    , filter_mode_notifier(0)
  {
    keyinfo_begin = mainbegin;
    keyinfo_end   = mainend;
    extra_begin = 0;
    extra_end   = 0;
  }