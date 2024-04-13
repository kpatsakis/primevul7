Status ConstantFolding::FoldMergeNode(NodeDef* node, GraphDef* output_graph) {
  // Merge nodes are special, in the sense that they execute as soon as one of
  // their input is ready. We can therefore fold a merge node iff it has at
  // least one constant input without control dependency.
  // We still need to ensure that the nodes in the fanin of the merge node are
  // scheduled. We'll therefore add a control dependency from the merge node
  // to the folded constant. We end up with:
  //  * the merge node and its inputs are preserved as is
  //  * a new constant node C1, driven by the merge node through a control
  //  dependency, initialized to the value of the folded input
  //  * a new constant node C2, driven by the merge node through a control
  //  dependency, initialized to the index of the folded input
  //  * the fanout of the merge nodes is rewired to be driven by either C1 or
  //  C2.
  for (int input_index = 0; input_index < node->input_size(); ++input_index) {
    const auto& input = node->input(input_index);
    if (IsControlInput(input)) {
      // Try the next input.
      continue;
    }
    NodeDef* input_node = node_map_->GetNode(input);
    if (!IsReallyConstant(*input_node)) {
      continue;
    }
    bool valid_input = true;
    for (const string& fanin_of_input : input_node->input()) {
      if (IsControlInput(fanin_of_input)) {
        valid_input = false;
        break;
      }
    }
    if (!valid_input) {
      // Try the next input
      continue;
    }

    string const_out_name = OptimizedNodeName(*node, "_const");
    string const_index_name = OptimizedNodeName(*node, "_index");
    if (node_map_->GetNode(const_out_name) ||
        node_map_->GetNode(const_index_name)) {
      // Intended name already exists.
      return errors::AlreadyExists(
          strings::StrCat(const_out_name, " or ", const_index_name,
                          " already present in the graph"));
    }

    NodeDef* const_out = output_graph->add_node();
    *const_out = *input_node;
    const_out->set_name(const_out_name);
    const_out->set_device(node->device());
    *const_out->add_input() = AsControlDependency(*node);
    node_map_->AddNode(const_out->name(), const_out);
    node_map_->AddOutput(node->name(), const_out->name());

    NodeDef* const_index = output_graph->add_node();
    const_index->set_op("Const");
    Tensor index(DT_INT32, TensorShape({}));
    index.flat<int32>()(0) = input_index;
    (*const_index->mutable_attr())["dtype"].set_type(DT_INT32);
    index.AsProtoTensorContent(
        (*const_index->mutable_attr())["value"].mutable_tensor());
    const_index->set_name(const_index_name);
    const_index->set_device(node->device());
    *const_index->add_input() = AsControlDependency(*node);
    node_map_->AddNode(const_index->name(), const_index);
    node_map_->AddOutput(node->name(), const_index->name());

    // We make a copy because we mutate the nodes.
    auto outputs = node_map_->GetOutputs(node->name());
    for (NodeDef* output : outputs) {
      for (int i = 0; i < output->input_size(); i++) {
        int port;
        string node_name = ParseNodeName(output->input(i), &port);
        if (node_name == node->name()) {
          if (port == 0) {
            *output->mutable_input(i) = const_out->name();
            node_map_->AddOutput(const_out->name(), output->name());
          } else if (port == 1) {
            *output->mutable_input(i) = const_index->name();
            node_map_->AddOutput(const_index->name(), output->name());
          } else {
            // This is a control dependency (or an invalid edge since the
            // merge node has only 2 outputs): preserve them.
          }
        }
      }
    }
    return Status::OK();
  }
  return Status::OK();
}