bool MaybeRemoveControlInput(const string& old_input, NodeDef* node,
                             GraphDef* graph, NodeMap* node_map) {
  bool removed_input = false;
  bool update_node_map = true;
  const string old_input_ctrl_dep = AsControlDependency(NodeName(old_input));
  for (int i = 0; i < node->input_size(); ++i) {
    const string& input = node->input(i);
    if (old_input_ctrl_dep == input) {
      if (IsControlInput(input)) {
        node->mutable_input()->SwapElements(i, node->input_size() - 1);
        node->mutable_input()->RemoveLast();
        removed_input = true;
      } else {
        // There is a non-control input from the same node.
        // Don't remove the output from the NodeMap.
        update_node_map = false;
      }
    }
  }
  if (update_node_map) {
    node_map->RemoveOutput(NodeName(old_input), node->name());
  }
  return removed_input;
}