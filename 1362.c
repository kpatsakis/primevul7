bool ConstantFolding::ReplaceOperationWithBroadcastTo(
    int input_to_broadcast, const GraphProperties& properties, NodeDef* node,
    GraphDef* graph) {
  const DataType dtype = GetDataTypeFromNodeOrProps(*node, properties);
  if (dtype == DT_INVALID) {
    return false;
  }
  const PartialTensorShape shape(
      properties.GetOutputProperties(node->name())[0].shape());
  if (!shape.IsFullyDefined()) {
    return false;
  }
  // Create constant node with shape.
  const string const_name = OptimizedNodeName(
      *node, strings::StrCat("-broadcastto_shape-", input_to_broadcast));
  if (node_map_->GetNode(const_name) != nullptr) {
    return false;
  }

  Tensor shape_t;
  if (!ConvertShapeToConstant("Shape", DT_INT32, shape, &shape_t).ok()) {
    return false;
  }
  NodeDef tmp;
  if (!CreateNodeDef(const_name, TensorValue(&shape_t), &tmp).ok()) {
    return false;
  }
  NodeDef* const_node = graph->add_node();
  const_node->Swap(&tmp);
  const_node->set_device(node->device());
  node_map_->AddNode(const_name, const_node);
  for (int i = 0; i < node->input_size(); ++i) {
    if (i != input_to_broadcast) {
      // Add a control input on the unused input.
      string ctrl_dep = AddControlDependency(NodeName(node->input(i)), graph,
                                             node_map_.get());
      *const_node->add_input() = ctrl_dep;
      node_map_->AddOutput(NodeName(ctrl_dep), const_name);
    }
  }

  // Rewrite `node` in-place to BroadcastTo.
  node->set_op("BroadcastTo");
  EraseRegularNodeAttributes(node);
  (*node->mutable_attr())["T"].set_type(dtype);
  (*node->mutable_attr())["Tidx"].set_type(DT_INT32);
  // Set the designated input to BroadcastTo.
  node->mutable_input()->SwapElements(0, input_to_broadcast);
  // Keep all other inputs as control dependencies.
  for (int i = 1; i < node->input_size(); ++i) {
    if (IsControlInput(node->input(i))) {
      break;
    }
    const string ctrl_dep =
        AddControlDependency(node->input(i), graph, node_map_.get());
    node_map_->UpdateInput(node->name(), node->input(i), ctrl_dep);
    node->set_input(i, ctrl_dep);
  }
  // Add the shape argument.
  *node->add_input() = const_node->name();
  node_map_->AddOutput(const_name, node->name());
  node->mutable_input()->SwapElements(1, node->input_size() - 1);
  return true;
}