#ifndef FUN_UTIL_CONTEXT_H
#define FUN_UTIL_CONTEXT_H

#include <assert.h>
#include <stack>
#include <map>
#include <iostream>

namespace fun {

template <typename Key, typename Value>
class Context;

// Helper class for Context
template <typename Key, typename Value>
class ContextBindCmd {
public:
  ContextBindCmd(Context<Key, Value> *ctxt, Key key, Value value)
    : ctxt(ctxt), key(key), value(value), prevValue(NULL) {}

  void execute();
  void undo();

private:
  Context<Key, Value> *ctxt;
  Key key;
  Value value;
  Value prevValue;
};

// Program context (environment) - stores bindings of "key - value"
// Usually key will be a variable or paramete rname, and value will be a type
// or value bound to the variable.
template <typename Key, typename Value>
class Context {
public:
  Context() : timeLine(0) {}

  // Binds a key with a 
  void bind(Key key, Value value);

  // Checks if key is bound in this context
  bool has(Key key) const;
  // Returns a value bound with this key. The key should exist in the context.
  Value get(Key key) const;

  // Undo the last action
  void undoOne();

  // Makes a checkpoint
  int checkPoint() const { return timeLine; }
  // Restore this context to a pre-created checkpoint state
  void restoreCheckPoint(int checkPoint);

  // Clears all bindings and resets the context to the initial state
  void clear();

  // Print current binding lists for debugging
  // Caution: Use this class only with 'MyValue' class.
  // This doesn't compile if the 'Value' class does not have 'toString' method.
  void printBindings() const;

private:
  friend class ContextBindCmd<Key, Value>;

  // These are not meant to be used from outside.
  // use undoOne() or restoreCheckPoint() instead.
  void addMapping(Key key, Value value) { ctxtMap[key] = value; }
  void removeMapping(Key key) { ctxtMap.erase(key); }

  int timeLine;
  std::map<Key, Value> ctxtMap;
  std::stack<ContextBindCmd<Key, Value>*> cmds;
};

template <typename Key, typename Value>
void ContextBindCmd<Key, Value>::execute() {
  if (ctxt->has(key)) prevValue = ctxt->get(key);
  ctxt->addMapping(key, value);
}

template <typename Key, typename Value>
void ContextBindCmd<Key, Value>::undo() {
  if (prevValue) ctxt->addMapping(key, prevValue);
  else ctxt->removeMapping(key);
}

template <typename Key, typename Value>
void Context<Key, Value>::bind(Key key, Value value) {
  ContextBindCmd<Key, Value> *cmd =
    new ContextBindCmd<Key, Value>(this, key, value);
  cmd->execute();
  cmds.push(cmd);
  timeLine++;
}

template <typename Key, typename Value>
bool Context<Key, Value>::has(Key key) const {
  typename std::map<Key, Value>::const_iterator it = ctxtMap.find(key);
  return it != ctxtMap.end();
}

template <typename Key, typename Value>
Value Context<Key, Value>::get(Key key) const {
  typename std::map<Key, Value>::const_iterator it = ctxtMap.find(key);
  assert(it != ctxtMap.end());
  return it->second;
}

template <typename Key, typename Value>
void Context<Key, Value>::undoOne() {
  ContextBindCmd<Key, Value> *cmd = cmds.top();
  cmds.pop();
  cmd->undo();
  delete cmd;
  timeLine--;
}

template <typename Key, typename Value>
void Context<Key, Value>::restoreCheckPoint(int checkPoint) {
  while (timeLine > checkPoint)
    undoOne();
}

template <typename Key, typename Value>
void Context<Key, Value>::clear() {
  ctxtMap.clear();
  while (!cmds.empty()) {
    ContextBindCmd<Key, Value> *cmd = cmds.top();
    cmds.pop();
    delete cmd;
  }
}

template <typename Key, typename Value>
void Context<Key, Value>::printBindings() const {
  typename std::map<Key, Value>::const_iterator it;
  for (it = ctxtMap.begin(); it != ctxtMap.end(); ++it)
    std::cout << it->first << ": " << it->second->toString() << "\n";
}

}

#endif
