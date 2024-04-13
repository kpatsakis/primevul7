void ConstantFolding::ReplaceOperationWithNoOp(NodeDef* node,
                                               GraphProperties* properties,
                                               GraphDef* graph) {
  if (HasRegularOutputs(*node, *node_map_)) return;
  node->set_op("NoOp");
  EraseRegularNodeAttributes(node);
  EraseNodeOutputAttributes(node);
  // Erase attributes that describe output properties.
  properties->ClearOutputProperties(node->name());
  // Change all inputs to control dependencies.
  for (int i = 0; i < node->input_size(); ++i) {
    if (IsControlInput(node->input(i))) {
      break;
    }
    const string ctrl_dep =
        AddControlDependency(node->input(i), graph, node_map_.get());
    node_map_->UpdateInput(node->name(), node->input(i), ctrl_dep);
    node->set_input(i, ctrl_dep);
  }
  DedupControlInputs(node);
  graph_modified_ = true;
}