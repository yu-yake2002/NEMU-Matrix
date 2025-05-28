#include <memory/store_queue_wrapper.h>
#include <queue>
#include <stack>

#ifdef CONFIG_STORE_LOG
std::stack<store_log_t> store_log_stack;
void store_log_stack_reset() { store_log_stack = {};}
void store_log_stack_push(store_log_t log) { store_log_stack.push(log);}
void store_log_stack_pop() { store_log_stack.pop();}
store_log_t store_log_stack_top() {return store_log_stack.top();}
bool store_log_stack_empty() { return store_log_stack.empty();}
#ifdef CONFIG_LIGHTQS
std::stack<store_log_t> spec_store_log_stack;
void spec_store_log_stack_reset() { spec_store_log_stack = {};}
void spec_store_log_stack_push(store_log_t log) { spec_store_log_stack.push(log);}
void spec_store_log_stack_pop() { spec_store_log_stack.pop();}
store_log_t spec_store_log_stack_top() { return spec_store_log_stack.top();}
bool spec_store_log_stack_empty() { return spec_store_log_stack.empty();}
void spec_store_log_stack_copy() { store_log_stack = spec_store_log_stack;}
#endif // CONFIG_LIGHTQS
#endif // CONFIG_STORE_LOG

#ifdef CONFIG_DIFFTEST_STORE_COMMIT

std::queue<store_commit_t> cpp_store_event_queue;

void store_queue_reset() {
  cpp_store_event_queue = {};
}

void store_queue_push(store_commit_t store_commit) {
  cpp_store_event_queue.push(store_commit);
}

void store_queue_pop() {
  cpp_store_event_queue.pop();
}

store_commit_t store_queue_front() {
  auto store_commit = cpp_store_event_queue.front();
  return store_commit;
}

store_commit_t store_queue_back() {
  auto store_commit = cpp_store_event_queue.back();
  return store_commit;
}

size_t store_queue_size() {
  return cpp_store_event_queue.size();
}

bool store_queue_empty() {
  return cpp_store_event_queue.empty();
}

#ifdef CONFIG_RVMATRIX
std::queue<matrix_store_commit_t> cpp_matrix_store_event_queue;

void matrix_store_queue_reset() {
  cpp_matrix_store_event_queue = {};
}

void matrix_store_queue_push(matrix_store_commit_t store_commit) {
  cpp_matrix_store_event_queue.push(store_commit);
}

void matrix_store_queue_pop() {
  cpp_matrix_store_event_queue.pop();
}

matrix_store_commit_t matrix_store_queue_front() {
  auto store_commit = cpp_matrix_store_event_queue.front();
  return store_commit;
}

matrix_store_commit_t matrix_store_queue_back() {
  auto store_commit = cpp_matrix_store_event_queue.back();
  return store_commit;
}

size_t matrix_store_queue_size() {
  return cpp_matrix_store_event_queue.size();
}

bool matrix_store_queue_empty() {
  return cpp_matrix_store_event_queue.empty();
}
#endif // CONFIG_RVMATRIX

#endif
