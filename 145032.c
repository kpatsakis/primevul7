  static const ConfigModule * find(ParmStr key, 
				   const ConfigModule * i, 
				   const ConfigModule * end) 
  {
    while (i != end) {
      if (strcmp(key, i->name) == 0)
	return i;
      ++i;
    }
    return i;
  }