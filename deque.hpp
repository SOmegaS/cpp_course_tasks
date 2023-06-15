#include <iostream>
#include <vector>

template <typename T>
class Deque {
 private:
  /* Массив с кластерами */
  std::vector<T*> chain_;

  // Я лЮбЛю МаЙнКрАфТ
  /* Размер кластера с элементами */
  const int kChunkSize = 1;

  /* Количество хранящихся элементов */
  size_t size_ = 0;

  /* Вместимость без реаллоцирования */
  size_t capacity_ = 0;

  /* Количество аллоцированных кластеров */
  size_t chunk_count_ = 0;

  /* Индекс кластера начала */
  int chunk_begin_ = 0;

  /* Индекс начального элемента в кластере */
  int begin_ = 0;

  /* Индекс кластера конца */
  int chunk_end_ = 0;

  /* Индекс конечного элемента в кластере */
  int end_ = 0;

 public:
  /* Класс итератора */
  template <bool IsConst>
  class common_iterator;

  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /* Конструктор по умолчанию */
  Deque();

  /* Конструктор копирования */
  Deque(const Deque<T>& deque);

  /* Конструктор по размеру */
  Deque(int size);

  /* Конструктор по размеру и элементу для заполнения */
  Deque(int size, const T& elem);

  /* Оператор присваивания */
  Deque<T>& operator=(const Deque<T>& deque);

  /* Количество элементов */
  size_t size() const;

  /* Доступ по [] */
  T& operator[](int index);

  /* Константный доступ по [] */
  const T& operator[](int index) const;

  /* Доступ по at */
  T& at(size_t index);

  /* Костантный доступ по at */
  const T& at(size_t index) const;

  /* Добавление элемента в конец */
  void push_back(const T& elem);

  /* Удаление элемента из конца */
  void pop_back();

  /* Добавление элемента в начало */
  void push_front(const T& elem);

  /* Удаление элемента из начала */
  void pop_front();

  /* Деструктор */
  ~Deque();

  /* Итератор на начало */
  iterator begin();

  /* Итератор на конец */
  iterator end();

  /* Константный итератор на начало */
  const_iterator cbegin() const;

  /* Константный итератор на конец */
  const_iterator cend() const;

  /* Итератор на начало */
  const_iterator begin() const;

  /* Итератор на конец */
  const_iterator end() const;

  /* Reverse-итератор на начало */
  reverse_iterator rbegin();

  /* Reverse-итератор на конец */
  reverse_iterator rend();

  /* Reverse-итератор на начало */
  const_reverse_iterator rbegin() const;

  /* Reverse-итератор на конец */
  const_reverse_iterator rend() const;

  /* Const reverse-итератор на начало */
  const_reverse_iterator crbegin() const;

  /* Const reverse-итератор на конец */
  const_reverse_iterator crend() const;

  /* Резервирование памяти */
  void reserve(size_t size);

  /* Вставка по итератору */
  void insert(iterator iter, const T& elem);

  /* Удаление по итератору */
  void erase(iterator iter);
};

template <typename T>
const T& Deque<T>::at(size_t index) const {
  if (index >= size_) {
    throw std::out_of_range("Deque out of range");
  }
  index += begin_;
  int chain_index = chunk_begin_ + index / kChunkSize;
  int chunk_index = index % kChunkSize;
  return chain_[chain_index][chunk_index];
}

template <typename T>
void Deque<T>::erase(Deque::iterator iter) {
  for (auto tmp_iter = iter + 1; tmp_iter != end(); ++tmp_iter, ++iter) {
    *iter = *tmp_iter;
  }
  pop_back();
}

template <typename T>
void Deque<T>::insert(Deque::iterator iter, const T& elem) {
  if (size_ + 1 > capacity_) {
    reserve(3 * capacity_);
  }

  if (iter == end()) {
    push_back(elem);
    return;
  }
  if (iter == begin() - 1) {
    push_front(elem);
    return;
  }

  T tmp_to_push_back(*(end() - 1));
  for (auto it = end() - 1; iter < it; --it) {
    *it = *(it - 1);
  }
  *iter = elem;
  push_back(tmp_to_push_back);
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::cend() const {
  return Deque::const_iterator(chain_.data(), kChunkSize,
                               chunk_begin_ + (size_ + begin_) / kChunkSize,
                               (size_ + begin_) % kChunkSize);
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::cbegin() const {
  return Deque::const_iterator(chain_.data(), kChunkSize,
                               chunk_begin_ + begin_ / kChunkSize,
                               begin_ % kChunkSize);
}

template <typename T>
typename Deque<T>::iterator Deque<T>::end() {
  return Deque::iterator(chain_.data(), kChunkSize,
                         chunk_begin_ + (size_ + begin_) / kChunkSize,
                         (size_ + begin_) % kChunkSize);
}

template <typename T>
typename Deque<T>::iterator Deque<T>::begin() {
  return Deque::iterator(chain_.data(), kChunkSize,
                         chunk_begin_ + begin_ / kChunkSize,
                         begin_ % kChunkSize);
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::end() const {
  return Deque::const_iterator(chain_.data(), kChunkSize,
                               chunk_begin_ + (size_ + begin_) / kChunkSize,
                               (size_ + begin_) % kChunkSize);
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::begin() const {
  return Deque::const_iterator(chain_.data(), kChunkSize,
                               chunk_begin_ + begin_ / kChunkSize,
                               begin_ % kChunkSize);
}

template <typename T>
typename Deque<T>::const_reverse_iterator Deque<T>::crend() const {
  return std::make_reverse_iterator(cbegin());
}

template <typename T>
typename Deque<T>::const_reverse_iterator Deque<T>::crbegin() const {
  return std::make_reverse_iterator(cend());
}

template <typename T>
typename Deque<T>::reverse_iterator Deque<T>::rend() {
  return std::make_reverse_iterator(begin());
}

template <typename T>
typename Deque<T>::reverse_iterator Deque<T>::rbegin() {
  return std::make_reverse_iterator(end());
}

template <typename T>
typename Deque<T>::const_reverse_iterator Deque<T>::rend() const {
  return std::make_reverse_iterator(cbegin());
}

template <typename T>
typename Deque<T>::const_reverse_iterator Deque<T>::rbegin() const {
  return std::make_reverse_iterator(cend());
}

template <typename T>
template <bool IsConst>
class Deque<T>::common_iterator {
 public:
  /* Тип возвращаемого значения в зависимости от константности */
  using value_type = std::conditional_t<IsConst, const T, T>;
  using difference_type = int;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator_category = std::random_access_iterator_tag;

  common_iterator(T** chain, const int& chunk_size, const int& chunk_index,
                  const int& element_index);

  common_iterator(T* const* chain, const int& chunk_size,
                  const int& chunk_index, const int& element_index);

  common_iterator<IsConst>& operator++();

  common_iterator<IsConst> operator++(int);

  common_iterator<IsConst>& operator--();

  common_iterator<IsConst> operator--(int);

  common_iterator<IsConst>& operator+=(int number);

  common_iterator<IsConst>& operator-=(int number);

  common_iterator<IsConst> operator+(int number) const;

  common_iterator<IsConst> operator-(int number) const;

  int operator-(const common_iterator& iter);

  bool operator<(const common_iterator& iter);

  bool operator>(const common_iterator& iter);

  bool operator<=(const common_iterator& iter);

  bool operator>=(const common_iterator& iter);

  bool operator==(const common_iterator& iter) const;

  bool operator!=(const common_iterator& iter) const;

  value_type& operator*();

  value_type* operator->();

  operator const_iterator();  // Не explicit, чтобы была возможность неявного
                              // каста

 private:
  /* Указатель на массив кластеров */
  std::conditional_t<IsConst, T* const*, T**> chain_;

  /* Абсолютный индекс кластера */
  int chunk_index_;

  /* Индекс элемента в кластере */
  int element_index_;

  /* Размер кластера */
  int chunk_size_;
};

template <typename T>
template <bool IsConst>
Deque<T>::common_iterator<IsConst>::common_iterator(T** chain,
                                                    const int& chunk_size,
                                                    const int& chunk_index,
                                                    const int& element_index)
    : chain_(chain),
      chunk_index_(chunk_index),
      element_index_(element_index),
      chunk_size_(chunk_size) {}

template <typename T>
template <bool IsConst>
Deque<T>::common_iterator<IsConst>::common_iterator(T* const* chain,
                                                    const int& chunk_size,
                                                    const int& chunk_index,
                                                    const int& element_index)
    : chain_(chain),
      chunk_index_(chunk_index),
      element_index_(element_index),
      chunk_size_(chunk_size) {}

template <typename T>
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>&
Deque<T>::common_iterator<IsConst>::operator++() {
  if (++element_index_ >= chunk_size_) {
    element_index_ = 0;
    ++chunk_index_;
  }
  return *this;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>
Deque<T>::common_iterator<IsConst>::operator++(int) {
  common_iterator<IsConst> old(*this);
  ++(*this);
  return old;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>&
Deque<T>::common_iterator<IsConst>::operator--() {
  if (--element_index_ < 0) {
    element_index_ = chunk_size_ - 1;
    --chunk_index_;
  }
  return *this;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>
Deque<T>::common_iterator<IsConst>::operator--(int) {
  common_iterator<IsConst> old(*this);
  --(*this);
  return old;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>&
Deque<T>::common_iterator<IsConst>::operator+=(int number) {
  if (number < 0) {
    return this->operator-=(-number);
  }
  if (element_index_ + number >= chunk_size_) {
    chunk_index_ += (element_index_ + number) / chunk_size_;
    element_index_ = (element_index_ + number) % chunk_size_;
  } else {
    element_index_ += number;
  }
  return *this;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>&
Deque<T>::common_iterator<IsConst>::operator-=(int number) {
  if (number < 0) {
    return this->operator+=(-number);
  }
  if (element_index_ - number < 0) {
    chunk_index_ -= (number - element_index_) / chunk_size_;
    chunk_index_ -= (((number - element_index_) % chunk_size_) == 0 ? 0 : 1);
    element_index_ =
        (chunk_size_ + element_index_ - number % chunk_size_) % chunk_size_;
  } else {
    element_index_ -= number;
  }
  return *this;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>
Deque<T>::common_iterator<IsConst>::operator+(int number) const {
  common_iterator<IsConst> iter(*this);
  iter += number;
  return iter;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>
Deque<T>::common_iterator<IsConst>::operator-(int number) const {
  common_iterator<IsConst> iter(*this);
  iter -= number;
  return iter;
}

template <typename T>
template <bool IsConst>
int Deque<T>::common_iterator<IsConst>::operator-(const common_iterator& iter) {
  return (element_index_ + chunk_size_ * chunk_index_) -
         (iter.element_index_ + chunk_size_ * iter.chunk_index_);
}

template <typename T>
template <bool IsConst>
bool Deque<T>::common_iterator<IsConst>::operator<(
    const common_iterator& iter) {
  return (element_index_ + chunk_size_ * chunk_index_) <
         (iter.element_index_ + chunk_size_ * iter.chunk_index_);
}

template <typename T>
template <bool IsConst>
bool Deque<T>::common_iterator<IsConst>::operator>(
    const common_iterator& iter) {
  return (element_index_ + chunk_size_ * chunk_index_) >
         (iter.element_index_ + chunk_size_ * iter.chunk_index_);
}

template <typename T>
template <bool IsConst>
bool Deque<T>::common_iterator<IsConst>::operator<=(
    const common_iterator& iter) {
  return (element_index_ + chunk_size_ * chunk_index_) <=
         (iter.element_index_ + chunk_size_ * iter.chunk_index_);
}

template <typename T>
template <bool IsConst>
bool Deque<T>::common_iterator<IsConst>::operator>=(
    const common_iterator& iter) {
  return (element_index_ + chunk_size_ * chunk_index_) >=
         (iter.element_index_ + chunk_size_ * iter.chunk_index_);
}

template <typename T>
template <bool IsConst>
bool Deque<T>::common_iterator<IsConst>::operator==(
    const common_iterator& iter) const {
  return (element_index_ + chunk_size_ * chunk_index_) ==
         (iter.element_index_ + chunk_size_ * iter.chunk_index_);
}

template <typename T>
template <bool IsConst>
bool Deque<T>::common_iterator<IsConst>::operator!=(
    const common_iterator& iter) const {
  return (element_index_ + chunk_size_ * chunk_index_) !=
         (iter.element_index_ + chunk_size_ * iter.chunk_index_);
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>::value_type&
Deque<T>::common_iterator<IsConst>::operator*() {
  return chain_[chunk_index_][element_index_];
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template common_iterator<IsConst>::value_type*
Deque<T>::common_iterator<IsConst>::operator->() {
  return &chain_[chunk_index_][element_index_];
}

template <typename T>
template <bool IsConst>
Deque<T>::common_iterator<IsConst>::operator const_iterator() {
  return const_iterator(*this);
}

template <typename T>
Deque<T>::Deque(int size, const T& elem) {
  reserve(size);
  int counter = 0;
  try {
    for (; counter < size; ++counter) {
      if (counter % 2 == 0) {
        push_back(elem);
      } else {
        push_front(elem);
      }
    }
  } catch (...) {
    for (--counter; counter >= 0; --counter) {
      if (counter % 2 == 0) {
        pop_back();
      } else {
        pop_front();
      }
    }
    for (int i = 0;
         i < static_cast<int>(capacity_ / static_cast<size_t>(kChunkSize));
         ++i) {
      delete[] reinterpret_cast<char*>(chain_[i]);
    }
    throw;
  }
}

template <typename T>
size_t Deque<T>::size() const {
  return size_;
}

template <typename T>
Deque<T>::Deque(const Deque<T>& deque) {
  chunk_count_ = deque.chunk_count_;
  chunk_begin_ = deque.chunk_begin_;
  chunk_end_ = deque.chunk_end_;
  begin_ = deque.begin_;
  end_ = deque.end_;
  size_ = deque.size_;
  capacity_ = deque.capacity_;

  chain_.resize(deque.chain_.size());
  for (int i = 0; i < static_cast<int>(deque.chain_.size()); ++i) {
    chain_[i] = reinterpret_cast<T*>(new char[sizeof(T) * kChunkSize]);
    std::copy(deque.chain_[i], deque.chain_[i] + kChunkSize, chain_[i]);
  }
}
template <typename T>
Deque<T>& Deque<T>::operator=(const Deque<T>& deque) {
  for (int i = 0;
       i < static_cast<int>(capacity_ / static_cast<size_t>(kChunkSize)); ++i) {
    if ((chunk_begin_ <= i) && (i <= chunk_end_)) {
      for (int j = (i == chunk_begin_ ? begin_ : 0);
           j < (i == chunk_end_ ? end_ : kChunkSize); ++j) {
        chain_[i][j].~T();
      }
    }
    delete[] reinterpret_cast<char*>(chain_[i]);
  }

  chunk_count_ = deque.chunk_count_;
  chunk_begin_ = deque.chunk_begin_;
  chunk_end_ = deque.chunk_end_;
  begin_ = deque.begin_;
  end_ = deque.end_;
  size_ = deque.size_;
  capacity_ = deque.capacity_;

  chain_.resize(deque.chain_.size());
  for (int i = 0; i < static_cast<int>(deque.chain_.size()); ++i) {
    //    std::copy(deque.chain_[i], deque.chain_[i] + kChunkSize, chain_[i]);
    chain_[i] = reinterpret_cast<T*>(new char[sizeof(T) * kChunkSize]);
    std::copy(deque.chain_[i], deque.chain_[i] + kChunkSize, chain_[i]);
  }

  return *this;
}

template <typename T>
Deque<T>::Deque() {
  reserve(1);
}

template <typename T>
Deque<T>::Deque(int size) {
  reserve(size);
  int counter = 0;
  try {
    for (; counter < size; ++counter) {
      if (counter % 2 == 0) {
        push_back(T());
      } else {
        push_front(T());
      }
    }
  } catch (...) {
    for (--counter; counter >= 0; --counter) {
      if (counter % 2 == 0) {
        pop_back();
      } else {
        pop_front();
      }
    }
    for (int i = 0;
         i < static_cast<int>(capacity_ / static_cast<size_t>(kChunkSize));
         ++i) {
      delete[] reinterpret_cast<char*>(chain_[i]);
    }
    throw;
  }
}

template <typename T>
Deque<T>::~Deque() {
  for (int i = 0;
       i < static_cast<int>(capacity_ / static_cast<size_t>(kChunkSize)); ++i) {
    if ((chunk_begin_ <= i) && (i <= chunk_end_)) {
      for (int j = (i == chunk_begin_ ? begin_ : 0);
           j < (i == chunk_end_ ? end_ : kChunkSize); ++j) {
        chain_[i][j].~T();
      }
    }
    delete[] reinterpret_cast<char*>(chain_[i]);
  }
}

template <typename T>
void Deque<T>::reserve(size_t size) {
  // Если уже зарезервировано больше, то ничего не делать
  if (capacity_ >= size) {
    return;
  }

  // Создание нового массива с кластерами
  int new_chunk_count = size / kChunkSize + (size % kChunkSize != 0 ? 1 : 0);
  std::vector<T*> new_chain(new_chunk_count);
  for (int i = 0, j = (chunk_count_ - new_chunk_count) / 2; i < new_chunk_count;
       ++i, ++j) {
    // Если кластер уже создан, то заменить, иначе создать
    if ((0 <= j) && (j < static_cast<int>(chunk_count_))) {
      new_chain[i] = chain_[j];
    } else {
      new_chain[i] = reinterpret_cast<T*>(new char[sizeof(T) * kChunkSize]);
    }
  }

  // Присваивание
  chain_.swap(new_chain);
  chunk_begin_ += (new_chunk_count - chunk_count_) / 2;
  chunk_end_ += (new_chunk_count - chunk_count_) / 2;
  chunk_count_ = new_chunk_count;
  capacity_ = chunk_count_ * kChunkSize;
}

template <typename T>
void Deque<T>::push_back(const T& elem) {
  new (chain_[chunk_end_] + end_) T(elem);
  ++end_;
  ++size_;
  if (end_ >= kChunkSize) {
    end_ = 0;
    ++chunk_end_;
  }
  if (chunk_end_ >=
      static_cast<int>(capacity_ / static_cast<size_t>(kChunkSize))) {
    reserve(capacity_ * 3);
  }
}

template <typename T>
T& Deque<T>::operator[](int index) {
  index += begin_;
  return chain_[chunk_begin_ + index / kChunkSize][index % kChunkSize];
}

template <typename T>
const T& Deque<T>::operator[](int index) const {
  index += begin_;
  return chain_[chunk_begin_ + index / kChunkSize][index % kChunkSize];
}

template <typename T>
T& Deque<T>::at(size_t index) {
  if (index >= size_) {
    throw std::out_of_range("Deque out of range");
  }
  index += begin_;
  index += begin_;
  int chain_index = chunk_begin_ + index / kChunkSize;
  int chunk_index = index % kChunkSize;
  return chain_[chain_index][chunk_index];
}

template <typename T>
void Deque<T>::pop_back() {
  (chain_[chunk_end_] + --end_)->~T();
  if (end_ < 0) {
    end_ = kChunkSize - 1;
    --chunk_end_;
  }
  --size_;
}

template <typename T>
void Deque<T>::push_front(const T& elem) {
  --begin_;
  if (begin_ < 0) {
    begin_ = kChunkSize - 1;
    --chunk_begin_;
  }
  if (chunk_begin_ < 0) {
    reserve(capacity_ * 3);
  }
  try {
    new (chain_[chunk_begin_] + begin_) T(elem);
  } catch (...) {
    ++begin_;
    if (begin_ >= kChunkSize) {
      begin_ = 0;
      ++chunk_begin_;
    }
    throw;
  }
  ++size_;
}

template <typename T>
[[maybe_unused]] void Deque<T>::pop_front() {
  (chain_[chunk_begin_] + begin_++)->~T();
  if (begin_ >= kChunkSize) {
    begin_ = 0;
    ++chunk_begin_;
  }
  --size_;
}
