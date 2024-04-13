bool MaybeAddControlInput(const string& ctrl_input, NodeDef* node,
                          GraphDef* graph, NodeMap* node_map) {
  bool already_exists = false;
  for (const string& input : node->input()) {
    if (input == ctrl_input || AsControlDependency(input) == ctrl_input) {
      already_exists = true;
      break;
    }
  }
  if (!already_exists) {
    const string ctrl_dep =
        ConstantFolding::AddControlDependency(ctrl_input, graph, node_map);
    node->add_input(ctrl_dep);
    node_map->AddOutput(NodeName(ctrl_input), node->name());
  }
  return !already_exists;
}