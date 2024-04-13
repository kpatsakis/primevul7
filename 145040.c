  Config::possible_elements(bool include_extra, bool include_modules) const
  {
    return new PossibleElementsEmul(this, include_extra, include_modules);
  }