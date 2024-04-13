bool ConstantFolding::MoveConstantsPastEnter(GraphDef* optimized_graph,
                                             NodeDef* node) {
  if (!IsEnter(*node) || node->input_size() == 0 ||
      node->attr().count("is_constant") == 0 ||
      !node->attr().at("is_constant").b()) {
    return false;
  }
  const string& node_name = node->name();
  const NodeDef* input = node_map_->GetNode(node->input(0));
  if (input == nullptr || !IsReallyConstant(*input) ||
      OptimizedNodeExists(*input, "_enter")) {
    return false;
  }
  // Find non-constant nodes that consume the output of *node.
  std::vector<NodeDef*> consumers;
  for (const NodeDef* fanout : node_map_->GetOutputs(node_name)) {
    if (!IsConstant(*fanout)) {
      for (int i = 0; i < fanout->input_size(); ++i) {
        if (fanout->input(i) == node_name) {
          consumers.push_back(const_cast<NodeDef*>(fanout));
          break;
        }
      }
    }
  }
  if (consumers.empty()) {
    return false;
  }
  graph_modified_ = true;
  NodeDef* new_node = optimized_graph->add_node();
  *new_node = *input;
  new_node->set_name(OptimizedNodeName(*input, "_enter"));
  new_node->set_device(node->device());
  new_node->clear_input();
  new_node->add_input(AsControlDependency(node_name));
  node_map_->AddNode(new_node->name(), new_node);
  node_map_->AddOutput(node_name, new_node->name());
  for (NodeDef* consumer : consumers) {
    for (int i = 0; i < consumer->input_size(); ++i) {
      if (NodeName(consumer->input(i)) == node_name) {
        node_map_->UpdateInput(consumer->name(), node_name, new_node->name());
        consumer->set_input(i, new_node->name());
      }
    }
  }
  return true;
}