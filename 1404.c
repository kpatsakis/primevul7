Status ConstantFolding::ReplaceOperationWithConstantTensor(DataType dtype,
                                                           TensorProto* value,
                                                           NodeDef* node,
                                                           GraphDef* graph) {
  if (dtype == DT_VARIANT) return Status::OK();
  node->set_op("Const");
  EraseRegularNodeAttributes(node);
  (*node->mutable_attr())["dtype"].set_type(dtype);
  (*node->mutable_attr())["value"].mutable_tensor()->Swap(value);
  // Convert all inputs to control dependencies.
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
  return Status::OK();
}