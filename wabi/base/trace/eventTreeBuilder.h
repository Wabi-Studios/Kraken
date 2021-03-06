/*
 * Copyright 2021 Pixar. All Rights Reserved.
 *
 * Portions of this file are derived from original work by Pixar
 * distributed with Universal Scene Description, a project of the
 * Academy Software Foundation (ASWF). https://www.aswf.io/
 *
 * Licensed under the Apache License, Version 2.0 (the "Apache License")
 * with the following modification; you may not use this file except in
 * compliance with the Apache License and the following modification:
 * Section 6. Trademarks. is deleted and replaced with:
 *
 * 6. Trademarks. This License does not grant permission to use the trade
 *    names, trademarks, service marks, or product names of the Licensor
 *    and its affiliates, except as required to comply with Section 4(c)
 *    of the License and to reproduce the content of the NOTICE file.
 *
 * You may obtain a copy of the Apache License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the Apache License with the above modification is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
 * ANY KIND, either express or implied. See the Apache License for the
 * specific language governing permissions and limitations under the
 * Apache License.
 *
 * Modifications copyright (C) 2020-2021 Wabi.
 */

#ifndef WABI_BASE_TRACE_EVENT_TREE_BUILDER_H
#define WABI_BASE_TRACE_EVENT_TREE_BUILDER_H

#include "wabi/wabi.h"

#include "wabi/base/trace/collection.h"
#include "wabi/base/trace/counterAccumulator.h"
#include "wabi/base/trace/eventNode.h"
#include "wabi/base/trace/eventTree.h"

WABI_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///
/// \class Trace_EventTreeBuilder
///
/// This class creates a tree of TraceEventTree instances from
/// TraceCollection instances.
///
class Trace_EventTreeBuilder : protected TraceCollection::Visitor
{
 public:

  /// Constructor.
  Trace_EventTreeBuilder();

  /// Returns the created tree.
  TraceEventTreeRefPtr GetTree()
  {
    return _tree;
  }

  /// Creates a TraceEventTree from the data in /p collection.
  TRACE_API void CreateTree(const TraceCollection &collection);

  /// Set the value of the counters.
  void SetCounterValues(const TraceEventTree::CounterMap &counterValues)
  {
    _counterAccum.SetCurrentValues(counterValues);
  }

 protected:

  /// \name TraceCollection::Visitor Interface
  /// @{
  virtual void OnBeginCollection() override;
  virtual void OnEndCollection() override;
  virtual bool AcceptsCategory(TraceCategoryId) override;
  virtual void OnBeginThread(const TraceThreadId &) override;
  virtual void OnEndThread(const TraceThreadId &) override;
  virtual void OnEvent(const TraceThreadId &, const TfToken &, const TraceEvent &) override;
  /// @}

 private:

  // Helper class for event graph creation.
  struct _PendingEventNode
  {
    using TimeStamp = TraceEvent::TimeStamp;

    struct AttributeData
    {
      TimeStamp time;
      TfToken key;
      TraceEventNode::AttributeData data;
    };

    _PendingEventNode(const TfToken &key,
                      TraceCategoryId category,
                      TimeStamp start,
                      TimeStamp end,
                      bool separateEvents,
                      bool isComplete);
    TraceEventNodeRefPtr Close();

    // Can move this, but not copy it
    _PendingEventNode(const _PendingEventNode &) = delete;
    _PendingEventNode &operator=(const _PendingEventNode &) = delete;

    _PendingEventNode(_PendingEventNode &&) = default;
    _PendingEventNode &operator=(_PendingEventNode &&) = default;

    TfToken key;
    TraceCategoryId category;
    TimeStamp start;
    TimeStamp end;
    bool separateEvents;
    bool isComplete;
    std::vector<TraceEventNodeRefPtr> children;
    std::vector<AttributeData> attributes;
  };

  void _OnBegin(const TraceThreadId &, const TfToken &, const TraceEvent &);
  void _OnEnd(const TraceThreadId &, const TfToken &, const TraceEvent &);
  void _OnData(const TraceThreadId &, const TfToken &, const TraceEvent &);
  void _OnTimespan(const TraceThreadId &, const TfToken &, const TraceEvent &);
  void _OnMarker(const TraceThreadId &, const TfToken &, const TraceEvent &);

  using _PendingNodeStack = std::vector<_PendingEventNode>;
  using _ThreadStackMap = std::map<TraceThreadId, _PendingNodeStack>;

  void _PopAndClose(_PendingNodeStack &stack);

  TraceEventNodeRefPtr _root;
  _ThreadStackMap _threadStacks;
  TraceEventTreeRefPtr _tree;

  class _CounterAccumulator : public TraceCounterAccumulator
  {
   protected:

    bool _AcceptsCategory(TraceCategoryId) override;
  };
  _CounterAccumulator _counterAccum;

  TraceEventTree::MarkerValuesMap _markersMap;
};

WABI_NAMESPACE_END

#endif  // WABI_BASE_TRACE_EVENT_TREE_BUILDER_H