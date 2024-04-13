onig_node_conv_to_str_node(Node* node, int flag)
{
  NODE_SET_TYPE(node, NODE_STRING);
  STR_(node)->flag = flag;
  STR_(node)->capa = 0;
  STR_(node)->s    = STR_(node)->buf;
  STR_(node)->end  = STR_(node)->buf;
}