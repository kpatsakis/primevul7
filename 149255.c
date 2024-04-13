node_new_enclosure(enum EnclosureType type)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_ENCLOSURE);
  ENCLOSURE_(node)->type = type;

  switch (type) {
  case ENCLOSURE_MEMORY:
    ENCLOSURE_(node)->m.regnum       =  0;
    ENCLOSURE_(node)->m.called_addr  = -1;
    ENCLOSURE_(node)->m.entry_count  =  1;
    ENCLOSURE_(node)->m.called_state =  0;
    break;

  case ENCLOSURE_OPTION:
    ENCLOSURE_(node)->o.options =  0;
    break;

  case ENCLOSURE_STOP_BACKTRACK:
    break;

  case ENCLOSURE_IF_ELSE:
    ENCLOSURE_(node)->te.Then = 0;
    ENCLOSURE_(node)->te.Else = 0;
    break;
  }

  ENCLOSURE_(node)->opt_count = 0;
  return node;
}