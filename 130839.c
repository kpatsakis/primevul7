static void build_completion_hash(bool rehash, bool write_info)
{
  COMMANDS *cmd=commands;
  MYSQL_RES *databases=0,*tables=0;
  MYSQL_RES *fields;
  static char ***field_names= 0;
  MYSQL_ROW database_row,table_row;
  MYSQL_FIELD *sql_field;
  char buf[NAME_LEN*2+2];		 // table name plus field name plus 2
  int i,j,num_fields;
  DBUG_ENTER("build_completion_hash");

  if (status.batch || quick || !current_db)
    DBUG_VOID_RETURN;			// We don't need completion in batches
  if (!rehash)
    DBUG_VOID_RETURN;

  /* Free old used memory */
  if (field_names)
    field_names=0;
  completion_hash_clean(&ht);
  free_root(&hash_mem_root,MYF(0));

  /* hash this file's known subset of SQL commands */
  while (cmd->name) {
    add_word(&ht,(char*) cmd->name);
    cmd++;
  }

  /* hash MySQL functions (to be implemented) */

  /* hash all database names */
  if (mysql_query(&mysql,"show databases") == 0)
  {
    if (!(databases = mysql_store_result(&mysql)))
      put_info(mysql_error(&mysql),INFO_INFO);
    else
    {
      while ((database_row=mysql_fetch_row(databases)))
      {
	char *str=strdup_root(&hash_mem_root, (char*) database_row[0]);
	if (str)
	  add_word(&ht,(char*) str);
      }
      mysql_free_result(databases);
    }
  }
  /* hash all table names */
  if (mysql_query(&mysql,"show tables")==0)
  {
    if (!(tables = mysql_store_result(&mysql)))
      put_info(mysql_error(&mysql),INFO_INFO);
    else
    {
      if (mysql_num_rows(tables) > 0 && !opt_silent && write_info)
      {
	tee_fprintf(stdout, "\
Reading table information for completion of table and column names\n\
You can turn off this feature to get a quicker startup with -A\n\n");
      }
      while ((table_row=mysql_fetch_row(tables)))
      {
	char *str=strdup_root(&hash_mem_root, (char*) table_row[0]);
	if (str &&
	    !completion_hash_exists(&ht,(char*) str, (uint) strlen(str)))
	  add_word(&ht,str);
      }
    }
  }

  /* hash all field names, both with the table prefix and without it */
  if (!tables)					/* no tables */
  {
    DBUG_VOID_RETURN;
  }
  mysql_data_seek(tables,0);
  if (!(field_names= (char ***) alloc_root(&hash_mem_root,sizeof(char **) *
					   (uint) (mysql_num_rows(tables)+1))))
  {
    mysql_free_result(tables);
    DBUG_VOID_RETURN;
  }
  i=0;
  while ((table_row=mysql_fetch_row(tables)))
  {
    if ((fields=mysql_list_fields(&mysql,(const char*) table_row[0],NullS)))
    {
      num_fields=mysql_num_fields(fields);
      if (!(field_names[i] = (char **) alloc_root(&hash_mem_root,
						  sizeof(char *) *
						  (num_fields*2+1))))
      {
        mysql_free_result(fields);
        break;
      }
      field_names[i][num_fields*2]= '\0';
      j=0;
      while ((sql_field=mysql_fetch_field(fields)))
      {
	sprintf(buf,"%.64s.%.64s",table_row[0],sql_field->name);
	field_names[i][j] = strdup_root(&hash_mem_root,buf);
	add_word(&ht,field_names[i][j]);
	field_names[i][num_fields+j] = strdup_root(&hash_mem_root,
						   sql_field->name);
	if (!completion_hash_exists(&ht,field_names[i][num_fields+j],
				    (uint) strlen(field_names[i][num_fields+j])))
	  add_word(&ht,field_names[i][num_fields+j]);
	j++;
      }
      mysql_free_result(fields);
    }
    else
      field_names[i]= 0;

    i++;
  }
  mysql_free_result(tables);
  field_names[i]=0;				// End pointer
  DBUG_VOID_RETURN;
}