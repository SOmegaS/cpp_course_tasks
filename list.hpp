#include <iostream>

template <size_t N>
class StackStorage {
 public:
  StackStorage();
  StackStorage(const StackStorage&) = delete;
  ~StackStorage() = default;
  void operator=(const StackStorage&) = delete;

  char pool[N];
  int offset = 0;
  void* some_8_byte_variable_lol;  // NOLINT
};

template <typename T, size_t N>
class StackAllocator {
 public:
  using value_type = T;

  StackStorage<N>* stack_storage;

  StackAllocator();
  template <typename U>
  StackAllocator(const StackAllocator<U, N>& allocator);
  StackAllocator(StackStorage<N>& stack_storage);
  ~StackAllocator();

  template <typename U>
  StackAllocator<T, N>& operator=(const StackAllocator<U, N>& allocator);

  value_type* allocate(size_t n);

  void deallocate(value_type* ptr, size_t count);

  template <typename U>
  struct [[maybe_unused]] rebind {  // NOLINT
    using other [[maybe_unused]] = StackAllocator<U, N>;
  };
};

template <typename T, typename Alloc = std::allocator<T>>
class List {
 public:
  template <bool IsConst>
  class common_iterator;

  using value_type = T;
  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  List();
  explicit List(size_t count);
  List(size_t count, const T& element);
  explicit List(Alloc& alloc);
  explicit List(size_t count, Alloc& allocator);
  explicit List(size_t count, const T& element, Alloc& allocator);
  List(const List& list);
  List(const List& list, Alloc&& alloc);
  ~List();
  List<T, Alloc>& operator=(const List<T, Alloc>& list);
  Alloc get_allocator();
  value_type& front();
  value_type& back();

  iterator begin();
  const_iterator begin() const;
  const_iterator cbegin() const;
  iterator end();
  const_iterator end() const;
  const_iterator cend() const;
  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;
  const_reverse_iterator crbegin() const;
  reverse_iterator rend();
  const_reverse_iterator rend() const;
  const_reverse_iterator crend() const;

  [[nodiscard]] size_t size() const;
  void clear();
  void insert(const_iterator pos, const T& element);
  void insert(const_iterator pos, T&& element);
  template <typename... Args>
  void insert(const_iterator pos, Args&... args);
  void erase(const_iterator iter);
  void push_back(const T& element);
  void push_back(T&& element);
  void pop_back();
  void push_front(const T& element);
  void push_front(T&& element);
  void pop_front();
  // void resize();

 private:
  struct Node {
    Node* prev;
    Node* next;
    T element;
    Node(Node* prev, Node* next) : prev(prev), next(next) {}
    Node(Node* prev, Node* next, const T& element)
        : prev(prev), next(next), element(element) {}
    Node(Node* prev, Node* next, T&& element)
        : prev(prev), next(next), element(element) {}
    template <typename... Args>
    Node(Node* prev, Node* next, Args&... args)
        : prev(prev), next(next), element(T(args...)) {}
  };

  using node_alloc =
      typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
  using node_alloc_traits =
      typename std::allocator_traits<Alloc>::template rebind_traits<Node>;

  [[no_unique_address]] node_alloc alloc_;

  size_t size_;
  Node* begin_;
  Node* end_;
};

template <typename T, typename Alloc>
template <bool IsConst>
class List<T, Alloc>::common_iterator {
 public:
  using value_type = std::conditional_t<IsConst, const T, T>;
  using difference_type = long long;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator_category = std::bidirectional_iterator_tag;

  Node* node;
  bool is_end;

  common_iterator(Node* node, bool is_end) : node(node), is_end(is_end) {}

  common_iterator<IsConst>& operator++() {
    if (node->next) {
      node = node->next;
    } else {
      is_end = true;
    }
    return *this;
  }

  common_iterator<IsConst> operator++(int) {
    common_iterator<IsConst> tmp(*this);
    if (node->next) {
      node = node->next;
    } else {
      is_end = true;
    }
    return tmp;
  }

  common_iterator<IsConst>& operator--() {
    if (is_end) {
      is_end = false;
    } else {
      node = node->prev;
    }
    return *this;
  }

  common_iterator<IsConst> operator--(int) {
    common_iterator<IsConst> tmp(*this);
    if (is_end) {
      is_end = false;
    } else {
      node = node->prev;
    }
    return tmp;
  }

  common_iterator<IsConst>& operator+=(int number) {
    for (int i = 0; i < number; ++i) {
      this->operator++();
    }
    return *this;
  }

  common_iterator<IsConst>& operator-=(int number) {
    for (int i = 0; i < number; ++i) {
      this->operator--();
    }
    return *this;
  }

  common_iterator<IsConst> operator+(int number) const {
    return (common_iterator<IsConst>(node, is_end) += number);
  }

  common_iterator<IsConst> operator-(int number) const {
    return (common_iterator<IsConst>(node, is_end) -= number);
  }

  bool operator==(const common_iterator& iter) const {
    return (node == iter.node) && (is_end == iter.is_end);
  }

  bool operator!=(const common_iterator& iter) const {
    return (node != iter.node) || (is_end != iter.is_end);
  }

  value_type& operator*() { return node->element; }

  value_type* operator->() { return &node->element; }

  operator const_iterator() const { return const_iterator(node, is_end); }
};

// StackStorage for StackAllocator

template <size_t N>
StackStorage<N>::StackStorage() {}

// StackAllocator

template <typename T, size_t N>
StackAllocator<T, N>::StackAllocator() : stack_storage(nullptr) {}

template <typename T, size_t N>
template <typename U>
StackAllocator<T, N>::StackAllocator(const StackAllocator<U, N>& allocator)
    : stack_storage(allocator.stack_storage) {}

template <typename T, size_t N>
StackAllocator<T, N>::StackAllocator(StackStorage<N>& stack_storage)
    : stack_storage(&stack_storage) {}

template <typename T, size_t N>
StackAllocator<T, N>::~StackAllocator() {}

template <typename T, size_t N>
template <typename U>
StackAllocator<T, N>& StackAllocator<T, N>::operator=(
    const StackAllocator<U, N>& allocator) {
  stack_storage = allocator.stack_storage;
  return *this;
}

template <typename T, size_t N>
typename StackAllocator<T, N>::value_type* StackAllocator<T, N>::allocate(
    size_t n) {
  if (!stack_storage) {
    throw std::bad_alloc();
  }
  int size_of_t = sizeof(T);
  int offset = size_of_t - stack_storage->offset % size_of_t;
  offset = offset == size_of_t ? 0 : offset;
  char* pos = &stack_storage->pool[0] + stack_storage->offset + offset;
  if (N - n < static_cast<size_t>(pos - &stack_storage->pool[0])) {
    throw std::bad_alloc();
  }
  T* tmp = reinterpret_cast<T*>(pos);
  stack_storage->offset += offset + n * size_of_t;
  return tmp;
}

template <typename T, size_t N>
void StackAllocator<T, N>::deallocate(value_type* /*unused*/,
                                      size_t /*unused*/) {}

// List

template <typename T, typename Alloc>
List<T, Alloc>::List()
    : alloc_(Alloc()), size_(0), begin_(nullptr), end_(nullptr) {}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_t count)
    : alloc_(Alloc()), size_(0), begin_(nullptr), end_(nullptr) {
  try {
    for (; count != 0; --count) {
      insert(begin());
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_t count, const T& element)
    : alloc_(Alloc()), size_(0), begin_(nullptr), end_(nullptr) {
  try {
    for (; count != 0; --count) {
      insert(begin(), element);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(Alloc& allocator)
    : alloc_(node_alloc(allocator)), size_(0), begin_(nullptr), end_(nullptr) {}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_t count, Alloc& allocator)
    : alloc_(node_alloc(allocator)), size_(0), begin_(nullptr), end_(nullptr) {
  try {
    for (; count != 0; --count) {
      insert(begin());
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_t count, const T& element, Alloc& allocator)
    : alloc_(node_alloc(allocator)), size_(0), begin_(nullptr), end_(nullptr) {
  try {
    for (; count != 0; --count) {
      insert(begin(), element);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(const List& list)
    : alloc_(
          std::allocator_traits<Alloc>::select_on_container_copy_construction(
              list.alloc_)),
      size_(0),
      begin_(nullptr),
      end_(nullptr) {
  try {
    for (auto& elem : list) {
      insert(end(), elem);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(const List& list, Alloc&& alloc)
    : alloc_(alloc), size_(0), begin_(nullptr), end_(nullptr) {
  try {
    for (auto& elem : list) {
      insert(end(), elem);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Alloc>
List<T, Alloc>::~List() {
  clear();
}

template <typename T, typename Alloc>
List<T, Alloc>& List<T, Alloc>::operator=(const List<T, Alloc>& list) {
  if (std::allocator_traits<
          Alloc>::propagate_on_container_copy_assignment::value) {
    List<T, Alloc> tmp(list, node_alloc(list.alloc_));
    std::swap(begin_, tmp.begin_);
    std::swap(end_, tmp.end_);
    std::swap(size_, tmp.size_);
    std::swap(alloc_, tmp.alloc_);
    return *this;
  }
  List<T, Alloc> tmp(list, alloc_);
  std::swap(begin_, tmp.begin_);
  std::swap(end_, tmp.end_);
  std::swap(size_, tmp.size_);
  std::swap(alloc_, tmp.alloc_);
  return *this;
}

template <typename T, typename Alloc>
Alloc List<T, Alloc>::get_allocator() {
  return alloc_;
}

template <typename T, typename Alloc>
typename List<T, Alloc>::value_type& List<T, Alloc>::front() {
  return begin_->element;
}

template <typename T, typename Alloc>
typename List<T, Alloc>::value_type& List<T, Alloc>::back() {
  return end_->element;
}

template <typename T, typename Alloc>
typename List<T, Alloc>::iterator List<T, Alloc>::begin() {
  return iterator(begin_, false);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::begin() const {
  return const_iterator(begin_, false);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::cbegin() const {
  return const_iterator(begin_, false);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::iterator List<T, Alloc>::end() {
  return iterator(end_, true);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::end() const {
  return const_iterator(end_, true);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::cend() const {
  return const_iterator(end_, true);
}

template <typename T, typename Alloc>
typename List<T, Alloc>::reverse_iterator List<T, Alloc>::rbegin() {
  return std::make_reverse_iterator(end());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::rbegin() const {
  return std::make_reverse_iterator(cend());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::crbegin()
    const {
  return std::make_reverse_iterator(cend());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::reverse_iterator List<T, Alloc>::rend() {
  return std::make_reverse_iterator(begin());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::rend() const {
  return std::make_reverse_iterator(cbegin());
}

template <typename T, typename Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::crend() const {
  return std::make_reverse_iterator(cbegin());
}

template <typename T, typename Alloc>
size_t List<T, Alloc>::size() const {
  return size_;
}

template <typename T, typename Alloc>
void List<T, Alloc>::clear() {
  for (; size_ != 0;) {
    erase(begin());
  }
}

template <typename T, typename Alloc>
void List<T, Alloc>::insert(List<T, Alloc>::const_iterator pos,
                            const T& element) {
  Node* node = node_alloc_traits::allocate(alloc_, 1);
  try {
    if (size_ == 0) {
      begin_ = node;
      node_alloc_traits::construct(alloc_, node, end_, nullptr, element);
      end_ = node;
    } else if (pos.node == end_ && pos.is_end) {
      end_->next = node;
      node_alloc_traits::construct(alloc_, node, end_, nullptr, element);
      end_ = node;
    } else if (pos.node == begin_) {
      begin_->prev = node;
      node_alloc_traits::construct(alloc_, node, nullptr, begin_, element);
      begin_ = node;
    } else {
      node_alloc_traits::construct(alloc_, node, pos.node->prev, pos.node,
                                   element);
      pos.node->prev->next = node;
      pos.node->prev = node;
    }
    ++size_;
  } catch (...) {
    node_alloc_traits::deallocate(alloc_, node, 1);
    throw;
  }
}

template <typename T, typename Alloc>
void List<T, Alloc>::insert(List<T, Alloc>::const_iterator pos, T&& element) {
  Node* node = node_alloc_traits::allocate(alloc_, 1);
  try {
    if (size_ == 0) {
      begin_ = node;
      node_alloc_traits::construct(alloc_, node, end_, nullptr,
                                   std::move(element));
      end_ = node;
    } else if (pos.node == end_ && pos.is_end) {
      end_->next = node;
      node_alloc_traits::construct(alloc_, node, end_, nullptr,
                                   std::move(element));
      end_ = node;
    } else if (pos.node == begin_) {
      begin_->prev = node;
      node_alloc_traits::construct(alloc_, node, nullptr, begin_,
                                   std::move(element));
      begin_ = node;
    } else {
      node_alloc_traits::construct(alloc_, node, pos.node->prev, pos.node,
                                   std::move(element));
      pos.node->prev->next = node;
      pos.node->prev = node;
    }
    ++size_;
  } catch (...) {
    node_alloc_traits::deallocate(alloc_, node, 1);
    throw;
  }
}

template <typename T, typename Alloc>
template <typename... Args>
void List<T, Alloc>::insert(List<T, Alloc>::const_iterator pos, Args&... args) {
  Node* node = node_alloc_traits::allocate(alloc_, 1);
  try {
    if (size_ == 0) {
      begin_ = node;
      node_alloc_traits::construct(alloc_, node, end_, nullptr, args...);
      end_ = node;
    } else if (pos.node == end_ && pos.is_end) {
      end_->next = node;
      node_alloc_traits::construct(alloc_, node, end_, nullptr, args...);
      end_ = node;
    } else if (pos.node == begin_) {
      begin_->prev = node;
      node_alloc_traits::construct(alloc_, node, nullptr, begin_, args...);
      begin_ = node;
    } else {
      node_alloc_traits::construct(alloc_, node, pos.node->prev, pos.node,
                                   args...);
      pos.node->prev->next = node;
      pos.node->prev = node;
    }
    ++size_;
  } catch (...) {
    node_alloc_traits::deallocate(alloc_, node, 1);
    throw;
  }
}

template <typename T, typename Alloc>
void List<T, Alloc>::erase(List<T, Alloc>::const_iterator iter) {
  if (size_ == 1) {  // iter.node == begin_ == end_
    begin_ = nullptr;
    end_ = nullptr;
  } else if (iter.node == begin_) {
    begin_ = begin_->next;
    begin_->prev = nullptr;
  } else if (iter.node == end_) {
    end_ = end_->prev;
    end_->next = nullptr;
  } else {
    iter.node->next->prev = iter.node->prev;
    iter.node->prev->next = iter.node->next;
  }
  node_alloc_traits::destroy(alloc_, iter.node);
  node_alloc_traits::deallocate(alloc_, iter.node, 1);
  --size_;
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_back(const T& element) {
  Node* node = node_alloc_traits::allocate(alloc_, 1);
  try {
    if (size_ == 0) {
      begin_ = node;
      node_alloc_traits::construct(alloc_, node, end_, nullptr, element);
      end_ = node;
    } else {
      end_->next = node;
      node_alloc_traits::construct(alloc_, node, end_, nullptr, element);
      end_ = node;
    }
    ++size_;
  } catch (...) {
    node_alloc_traits::deallocate(alloc_, node, 1);
    throw;
  }
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_back(T&& element) {
  Node* node = node_alloc_traits::allocate(alloc_, 1);
  try {
    if (size_ == 0) {
      begin_ = node;
      node_alloc_traits::construct(alloc_, node, end_, nullptr,
                                   std::move(element));
      end_ = node;
    } else {
      end_->next = node;
      node_alloc_traits::construct(alloc_, node, end_, nullptr,
                                   std::move(element));
      end_ = node;
    }
    ++size_;
  } catch (...) {
    node_alloc_traits::deallocate(alloc_, node, 1);
    throw;
  }
}

template <typename T, typename Alloc>
void List<T, Alloc>::pop_back() {
  auto tmp = end_;
  if (size_ == 1) {  // iter.node == begin_ == end_
    begin_ = nullptr;
    end_ = nullptr;
  } else {
    end_ = end_->prev;
    end_->next = nullptr;
  }
  node_alloc_traits::destroy(alloc_, tmp);
  node_alloc_traits::deallocate(alloc_, tmp, 1);
  --size_;
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_front(const T& element) {
  insert(begin(), element);
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_front(T&& element) {
  insert(begin(), element);
}

template <typename T, typename Alloc>
void List<T, Alloc>::pop_front() {
  erase(begin());
}
