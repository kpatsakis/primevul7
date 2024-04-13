bool ConstantFolding::PartialConcatConstFolding(GraphDef* optimized_graph,
                                                GraphProperties* properties,
                                                NodeDef* node) {
  // Partial constant folding for Concat which is not commutative, so
  // we have to preserve order and can only push consecutive runs of constant
  // inputs into sub-nodes.
  if (!IsConcat(*node) ||
      node->name().rfind("_partial_split_") != string::npos) {
    return false;
  }
  const int num_non_control_inputs = NumNonControlInputs(*node);
  if (num_non_control_inputs <= 3) return false;
  int axis_arg = -1;
  int begin = 0;
  int end = num_non_control_inputs;
  if (node->op() == "Concat") {
    begin = 1;
    axis_arg = 0;
  } else if (node->op() == "ConcatV2") {
    end = num_non_control_inputs - 1;
    axis_arg = num_non_control_inputs - 1;
  } else {
    return false;
  }

  // We search for consecutive runs of constant inputs in the range
  // [begin:end[ and push then down into child nodes.
  std::vector<std::pair<int, int>> constant_input_runs;
  int first = begin;
  int last = begin;
  while (last < end) {
    while (first < end && !IsReallyConstant(*node_map_->GetNode(
                              NodeName(node->input(first))))) {
      ++first;
    }
    // Invariant: node[first] is constant || first >= end.
    last = first + 1;
    while (last < end &&
           IsReallyConstant(*node_map_->GetNode(NodeName(node->input(last))))) {
      ++last;
    }
    // Invariant: node[last] is not constant || last >= end
    // Discard intervals shorter than 2 elements.
    if (first < end && (last - first) > 1) {
      constant_input_runs.emplace_back(first, last);
    }
    first = last;
  }

  // Skip if all inputs are constant, and let constant folding take over.
  if (constant_input_runs.empty() || (constant_input_runs.size() == 1 &&
                                      constant_input_runs[0].first == begin &&
                                      constant_input_runs[0].second == end)) {
    return false;
  }
  std::set<int> inputs_to_delete;
  for (auto interval : constant_input_runs) {
    // Push the constant inputs in the interval to a child node than can be
    // constant folded.
    string new_node_name = OptimizedNodeName(*node, "_partial_split");
    do {
      new_node_name += strings::StrCat("_", interval.first);
    } while (node_map_->NodeExists(new_node_name));

    NodeDef* added_node = optimized_graph->add_node();
    *added_node = *node;
    added_node->set_op("ConcatV2");
    added_node->set_name(new_node_name);
    node_map_->AddNode(added_node->name(), added_node);
    added_node->clear_input();
    for (int i = interval.first; i < interval.second; ++i) {
      added_node->add_input(node->input(i));
      node_map_->UpdateInput(node->name(), node->input(i), added_node->name());
      if (i != interval.first) {
        inputs_to_delete.insert(i);
      }
    }
    added_node->add_input(node->input(axis_arg));
    (*added_node->mutable_attr())["N"].set_i(interval.second - interval.first);
    node_map_->AddOutput(NodeName(node->input(axis_arg)), added_node->name());

    // Overwrite the first constant input with the result of the added
    // child node.
    node->set_input(interval.first, added_node->name());
  }
  if (!inputs_to_delete.empty()) {
    // Fix up the inputs to the original node.
    protobuf::RepeatedPtrField<string> tmp;
    tmp.Swap(node->mutable_input());
    for (int i = 0; i < tmp.size(); ++i) {
      if (inputs_to_delete.find(i) == inputs_to_delete.end()) {
        node->add_input(tmp.Get(i));
      }
    }
    (*node->mutable_attr())["N"].set_i(node->input_size() - 1);
    properties->ClearInputProperties(node->name());
  }
  return true;
}