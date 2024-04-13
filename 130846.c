static void fix_history(String *final_command) 
{
  int total_lines = 1;
  char *ptr = final_command->c_ptr();
  String fixed_buffer; 	/* Converted buffer */
  char str_char = '\0';  /* Character if we are in a string or not */
  
  /* find out how many lines we have and remove newlines */
  while (*ptr != '\0') 
  {
    switch (*ptr) {
      /* string character */
    case '"':
    case '\'':
    case '`':
      if (str_char == '\0')	/* open string */
	str_char = *ptr;
      else if (str_char == *ptr)   /* close string */
	str_char = '\0';
      fixed_buffer.append(ptr,1);
      break;
    case '\n':
      /* 
	 not in string, change to space
	 if in string, leave it alone 
      */
      fixed_buffer.append(str_char == '\0' ? " " : "\n");
      total_lines++;
      break;
    case '\\':
      fixed_buffer.append('\\');
      /* need to see if the backslash is escaping anything */
      if (str_char) 
      {
	ptr++;
	/* special characters that need escaping */
	if (*ptr == '\'' || *ptr == '"' || *ptr == '\\')
	  fixed_buffer.append(ptr,1);
	else
	  ptr--;
      }
      break;
      
    default:
      fixed_buffer.append(ptr,1);
    }
    ptr++;
  }
  if (total_lines > 1)			
    add_history(fixed_buffer.ptr());
}