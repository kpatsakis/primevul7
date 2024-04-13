static void alloc_stmt_fields(MYSQL_STMT *stmt)
{
  MYSQL_FIELD *fields, *field, *end;
  MEM_ROOT *fields_mem_root= &stmt->extension->fields_mem_root;
  MYSQL *mysql= stmt->mysql;

  DBUG_ASSERT(stmt->field_count);

  free_root(fields_mem_root, MYF(0));

  /*
    Get the field information for non-select statements
    like SHOW and DESCRIBE commands
  */
  if (!(stmt->fields= (MYSQL_FIELD *) alloc_root(fields_mem_root,
						 sizeof(MYSQL_FIELD) *
						 stmt->field_count)) ||
      !(stmt->bind= (MYSQL_BIND *) alloc_root(fields_mem_root,
					      sizeof(MYSQL_BIND) *
					      stmt->field_count)))
  {
    set_stmt_error(stmt, CR_OUT_OF_MEMORY, unknown_sqlstate, NULL);
    return;
  }

  for (fields= mysql->fields, end= fields+stmt->field_count,
	 field= stmt->fields;
       field && fields < end; fields++, field++)
  {
    *field= *fields; /* To copy all numeric parts. */
    field->catalog=   strmake_root(fields_mem_root,
                                   fields->catalog,
                                   fields->catalog_length);
    field->db=        strmake_root(fields_mem_root,
                                   fields->db,
                                   fields->db_length);
    field->table=     strmake_root(fields_mem_root,
                                   fields->table,
                                   fields->table_length);
    field->org_table= strmake_root(fields_mem_root,
                                   fields->org_table,
                                   fields->org_table_length);
    field->name=      strmake_root(fields_mem_root,
                                   fields->name,
                                   fields->name_length);
    field->org_name=  strmake_root(fields_mem_root,
                                   fields->org_name,
                                   fields->org_name_length);
    if (fields->def)
    {
      field->def= strmake_root(fields_mem_root,
                               fields->def,
                               fields->def_length);
      field->def_length= fields->def_length;
    }
    else
    {
      field->def= NULL;
      field->def_length= 0;
    }
    field->extension= 0; /* Avoid dangling links. */
    field->max_length= 0; /* max_length is set in mysql_stmt_store_result() */
  }
}