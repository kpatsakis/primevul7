void ConstantFolding::ReplaceOperationWithSnapshot(
    int input_to_forward, const GraphProperties& properties, NodeDef* node,
    GraphDef* graph) {
  // If the graph contains no ops that mutate their inputs, we can
  // use Identity instead of Snapshot.
  if (!graph_contains_assign_or_inplace_op_) {
    ReplaceOperationWithIdentity(input_to_forward, properties, node, graph);
    return;
  }

  const DataType dtype = GetDataTypeFromNodeOrProps(*node, properties);
  if (dtype == DT_INVALID) return;

  node->set_op("Snapshot");
  EraseRegularNodeAttributes(node);
  (*node->mutable_attr())["T"].set_type(dtype);
  // Propagate the designated input through the Snapshot.
  node->mutable_input()->SwapElements(0, input_to_forward);
  // Add all other inputs as control dependencies.
  for (int i = 1; i < node->input_size(); ++i) {
    if (IsControlInput(node->input(i))) {
      break;
    }
    const string ctrl_dep =
        AddControlDependency(node->input(i), graph, node_map_.get());
    node_map_->UpdateInput(node->name(), node->input(i), ctrl_dep);
    node->set_input(i, ctrl_dep);
  }
  graph_modified_ = true;
}