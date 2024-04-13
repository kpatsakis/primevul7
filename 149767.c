char_to_option(int c)
{
    int val;

    switch (c) {
      case 'i':
	val = ONIG_OPTION_IGNORECASE;
	break;
      case 'x':
	val = ONIG_OPTION_EXTEND;
	break;
      case 'm':
	val = ONIG_OPTION_MULTILINE;
	break;
      default:
	val = 0;
	break;
    }
    return val;
}