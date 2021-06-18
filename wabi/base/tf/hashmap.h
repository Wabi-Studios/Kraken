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

#ifndef WABI_BASE_TF_HASHMAP_H
#define WABI_BASE_TF_HASHMAP_H

#include "wabi/base/arch/defines.h"
#include "wabi/wabi.h"

#if defined(__GNUC__)
/**
 * Wrap one of various unordered map implementations. The exposed API
 * is similar to  std::unordered_map  but is missing some methods and
 * erase(const_iterator) returns nothing.
 *
 * Wrapping provides a convenient way to switch between implementations.
 *
 * Use the C++ standard implementation. Otherwise, the GNU extension had
 * better performance than the standard implementation in versions <= 4.3
 * however, this is no longer the case.
 */
#  include <features.h>
#  if __GNUC_PREREQ(4, 3)
#    include <unordered_map>
#  elif defined(ARCH_HAS_GNU_STL_EXTENSIONS)
#    define USE_DEPRECATED_GNU_HASH_MAP
#    include <ext/hash_map>
#  else /* ARCH_HAS_GNU_STL_EXTENSIONS */
#    include <unordered_set>
#  endif /* __GNUC_PREREQ >= 4.3 */
#else
#  include <unordered_map>
#endif /* __GNUC__ */

WABI_NAMESPACE_BEGIN

#if !defined(USE_DEPRECATED_GNU_HASH_MAP)

template<class Key,
         class Mapped,
         class HashFn = std::hash<Key>,
         class EqualKey = std::equal_to<Key>,
         class Alloc = std::allocator<std::pair<const Key, Mapped>>>
class TfHashMap : private std::unordered_map<Key, Mapped, HashFn, EqualKey, Alloc>
{
  typedef std::unordered_map<Key, Mapped, HashFn, EqualKey, Alloc> _Base;

 public:
  typedef typename _Base::key_type key_type;
  typedef typename _Base::mapped_type mapped_type;
  typedef typename _Base::value_type value_type;
  typedef typename _Base::hasher hasher;
  typedef typename _Base::key_equal key_equal;
  typedef typename _Base::size_type size_type;
  typedef typename _Base::difference_type difference_type;
  typedef typename _Base::pointer pointer;
  typedef typename _Base::const_pointer const_pointer;
  typedef typename _Base::reference reference;
  typedef typename _Base::const_reference const_reference;
  typedef typename _Base::iterator iterator;
  typedef typename _Base::const_iterator const_iterator;
  typedef typename _Base::allocator_type allocator_type;
  // No local_iterator nor any methods using them.

  TfHashMap()
    : _Base()
  {}
  explicit TfHashMap(size_type n,
                     const hasher &hf = hasher(),
                     const key_equal &eql = key_equal(),
                     const allocator_type &alloc = allocator_type())
    : _Base(n, hf, eql, alloc)
  {}
  explicit TfHashMap(const allocator_type &alloc)
    : _Base(alloc)
  {}
  template<class InputIterator>
  TfHashMap(InputIterator first,
            InputIterator last,
            size_type n = 0,
            const hasher &hf = hasher(),
            const key_equal &eql = key_equal(),
            const allocator_type &alloc = allocator_type())
    : _Base(first, last, n, hf, eql, alloc)
  {}
  TfHashMap(const TfHashMap &other)
    : _Base(other)
  {}

  TfHashMap &operator=(const TfHashMap &rhs)
  {
    _Base::operator=(rhs);
    return *this;
  }

  iterator begin()
  {
    return _Base::begin();
  }
  const_iterator begin() const
  {
    return _Base::begin();
  }
  // using _Base::bucket;
  using _Base::bucket_count;
  using _Base::bucket_size;
  const_iterator cbegin() const
  {
    return _Base::cbegin();
  }
  const_iterator cend() const
  {
    return _Base::cend();
  }
  using _Base::clear;
  using _Base::count;
  using _Base::empty;
  iterator end()
  {
    return _Base::end();
  }
  const_iterator end() const
  {
    return _Base::end();
  }
  using _Base::equal_range;
  size_type erase(const key_type &key)
  {
    return _Base::erase(key);
  }
  void erase(const_iterator position)
  {
    _Base::erase(position);
  }
  void erase(const_iterator first, const_iterator last)
  {
    _Base::erase(first, last);
  }
  using _Base::find;
  using _Base::get_allocator;
  using _Base::hash_function;
  std::pair<iterator, bool> insert(const value_type &v)
  {
    return _Base::insert(v);
  }
  iterator insert(const_iterator hint, const value_type &v)
  {
    return _Base::insert(hint, v);
  }
  template<class InputIterator>
  void insert(InputIterator first, InputIterator last)
  {
    _Base::insert(first, last);
  }
  using _Base::key_eq;
  using _Base::load_factor;
  using _Base::max_bucket_count;
  using _Base::max_load_factor;
  // using _Base::max_load_factor;
  using _Base::max_size;
  using _Base::rehash;
  using _Base::reserve;
  using _Base::size;
  void swap(TfHashMap &other)
  {
    _Base::swap(other);
  }
  mapped_type &operator[](const key_type &k)
  {
    return _Base::operator[](k);
  }

  template<class Key2, class Mapped2, class HashFn2, class EqualKey2, class Alloc2>
  friend bool operator==(const TfHashMap<Key2, Mapped2, HashFn2, EqualKey2, Alloc2> &,
                         const TfHashMap<Key2, Mapped2, HashFn2, EqualKey2, Alloc2> &);
};

template<class Key,
         class Mapped,
         class HashFn = std::hash<Key>,
         class EqualKey = std::equal_to<Key>,
         class Alloc = std::allocator<std::pair<const Key, Mapped>>>
class TfHashMultiMap : private std::unordered_multimap<Key, Mapped, HashFn, EqualKey, Alloc>
{
  typedef std::unordered_multimap<Key, Mapped, HashFn, EqualKey, Alloc> _Base;

 public:
  typedef typename _Base::key_type key_type;
  typedef typename _Base::mapped_type mapped_type;
  typedef typename _Base::value_type value_type;
  typedef typename _Base::hasher hasher;
  typedef typename _Base::key_equal key_equal;
  typedef typename _Base::size_type size_type;
  typedef typename _Base::difference_type difference_type;
  typedef typename _Base::pointer pointer;
  typedef typename _Base::const_pointer const_pointer;
  typedef typename _Base::reference reference;
  typedef typename _Base::const_reference const_reference;
  typedef typename _Base::iterator iterator;
  typedef typename _Base::const_iterator const_iterator;
  typedef typename _Base::allocator_type allocator_type;
  // No local_iterator nor any methods using them.

  TfHashMultiMap()
    : _Base()
  {}
  explicit TfHashMultiMap(size_type n,
                          const hasher &hf = hasher(),
                          const key_equal &eql = key_equal(),
                          const allocator_type &alloc = allocator_type())
    : _Base(n, hf, eql, alloc)
  {}
  explicit TfHashMultiMap(const allocator_type &alloc)
    : _Base(alloc)
  {}
  template<class InputIterator>
  TfHashMultiMap(InputIterator first,
                 InputIterator last,
                 size_type n = 0,
                 const hasher &hf = hasher(),
                 const key_equal &eql = key_equal(),
                 const allocator_type &alloc = allocator_type())
    : _Base(first, last, n, hf, eql, alloc)
  {}
  TfHashMultiMap(const TfHashMultiMap &other)
    : _Base(other)
  {}

  TfHashMultiMap &operator=(const TfHashMultiMap &rhs)
  {
    _Base::operator=(rhs);
    return *this;
  }

  iterator begin()
  {
    return _Base::begin();
  }
  const_iterator begin() const
  {
    return _Base::begin();
  }
  // using _Base::bucket;
  using _Base::bucket_count;
  using _Base::bucket_size;
  const_iterator cbegin() const
  {
    return _Base::cbegin();
  }
  const_iterator cend() const
  {
    return _Base::cend();
  }
  using _Base::clear;
  using _Base::count;
  using _Base::empty;
  iterator end()
  {
    return _Base::end();
  }
  const_iterator end() const
  {
    return _Base::end();
  }
  using _Base::equal_range;
  size_type erase(const key_type &key)
  {
    return _Base::erase(key);
  }
  void erase(const_iterator position)
  {
    _Base::erase(position);
  }
  void erase(const_iterator first, const_iterator last)
  {
    _Base::erase(first, last);
  }
  using _Base::find;
  using _Base::get_allocator;
  using _Base::hash_function;
  iterator insert(const value_type &v)
  {
    return _Base::insert(v);
  }
  iterator insert(const_iterator hint, const value_type &v)
  {
    return _Base::insert(hint, v);
  }
  template<class InputIterator>
  void insert(InputIterator first, InputIterator last)
  {
    _Base::insert(first, last);
  }
  using _Base::key_eq;
  using _Base::load_factor;
  using _Base::max_bucket_count;
  using _Base::max_load_factor;
  // using _Base::max_load_factor;
  using _Base::max_size;
  using _Base::rehash;
  using _Base::reserve;
  using _Base::size;
  void swap(TfHashMultiMap &other)
  {
    _Base::swap(other);
  }
  mapped_type &operator[](const key_type &k)
  {
    return _Base::operator[](k);
  }

  template<class Key2, class Mapped2, class HashFn2, class EqualKey2, class Alloc2>
  friend bool operator==(const TfHashMap<Key2, Mapped2, HashFn2, EqualKey2, Alloc2> &,
                         const TfHashMap<Key2, Mapped2, HashFn2, EqualKey2, Alloc2> &);
};

#else /* USE_DEPRECATED_GNU_HASH_MAP */

template<class Key,
         class Mapped,
         class HashFn = __gnu_cxx::hash<Key>,
         class EqualKey = __gnu_cxx::equal_to<Key>,
         class Alloc = __gnu_cxx::allocator<Mapped>>
class TfHashMap : private __gnu_cxx::hash_map<Key, Mapped, HashFn, EqualKey, Alloc>
{

  using _Base = __gnu_cxx::hash_map<Key, Mapped, HashFn, EqualKey, Alloc>;

 public:
  typedef typename _Base::key_type key_type;
  typedef typename _Base::mapped_type mapped_type;
  typedef typename _Base::value_type value_type;
  typedef typename _Base::hasher hasher;
  typedef typename _Base::key_equal key_equal;
  typedef typename _Base::size_type size_type;
  typedef typename _Base::difference_type difference_type;
  typedef typename _Base::pointer pointer;
  typedef typename _Base::const_pointer const_pointer;
  typedef typename _Base::reference reference;
  typedef typename _Base::const_reference const_reference;
  typedef typename _Base::iterator iterator;
  typedef typename _Base::const_iterator const_iterator;
  typedef typename _Base::allocator_type allocator_type;
  // No local_iterator nor any methods using them.

  TfHashMap()
    : _Base()
  {}

  explicit TfHashMap(size_type n,
                     const hasher &hf = hasher(),
                     const key_equal &eql = key_equal(),
                     const allocator_type &alloc = allocator_type())
    : _Base(n, hf, eql, alloc)
  {}

  explicit TfHashMap(const allocator_type &alloc)
    : _Base(0, hasher(), key_equal(), alloc)
  {}

  template<class InputIterator>
  TfHashMap(InputIterator first,
            InputIterator last,
            size_type n = 0,
            const hasher &hf = hasher(),
            const key_equal &eql = key_equal(),
            const allocator_type &alloc = allocator_type())
    : _Base(first, last, n, hf, eql, alloc)
  {}

  TfHashMap(const TfHashMap &other)
    : _Base(other)
  {}

  TfHashMap &operator=(const TfHashMap &rhs)
  {
    _Base::operator=(rhs);
    return *this;
  }

  iterator begin()
  {
    return _Base::begin();
  }
  const_iterator begin() const
  {
    return const_iterator(_Base::begin());
  }
  // size_type bucket(const key_type& key) const;
  using _Base::bucket_count;
  size_type bucket_size(size_type n) const
  {
    return _Base::elems_in_bucket(n);
  }
  const_iterator cbegin() const
  {
    return const_iterator(_Base::begin());
  }
  const_iterator cend() const
  {
    return const_iterator(_Base::end());
  }
  using _Base::clear;
  using _Base::count;
  using _Base::empty;
  iterator end()
  {
    return _Base::end();
  }
  const_iterator end() const
  {
    return const_iterator(_Base::end());
  }
  std::pair<iterator, iterator> equal_range(const key_type &key)
  {
    return _Base::equal_range(key);
  }
  std::pair<const_iterator, const_iterator> equal_range(const key_type &key) const
  {
    return _Base::equal_range(key);
  }
  size_type erase(const key_type &key)
  {
    return _Base::erase(key);
  }
  void erase(const_iterator position)
  {
    _Base::erase(_MakeIterator(position));
  }
  void erase(const_iterator first, const_iterator last)
  {
    _Base::erase(_MakeIterator(first), _MakeIterator(last));
  }
  iterator find(const key_type &key)
  {
    return _Base::find(key);
  }
  const_iterator find(const key_type &key) const
  {
    return const_iterator(_Base::find(key));
  }
  using _Base::get_allocator;
  hasher hash_function() const
  {
    return _Base::hash_funct();
  }
  using _Base::insert;
  iterator insert(const_iterator, const value_type &v)
  {
    return insert(v).first;
  }
  iterator insert(const_iterator, value_type &&v)
  {
    return insert(v).first;
  }
  std::pair<iterator, bool> insert(value_type &&v)
  {
    return insert(v);
  }
  using _Base::key_eq;
  float load_factor() const
  {
    return static_cast<float>(static_cast<double>(size()) / bucket_count());
  }
  using _Base::max_bucket_count;
  float max_load_factor() const
  {
    return 1.0;
  }
  // void max_load_factor(float) { }
  using _Base::max_size;
  void rehash(size_type n)
  {
    _Base::resize(__gnu_cxx::__stl_next_prime(n));
  }
  void reserve(size_type n)
  {
    _Base::resize(n);
  }
  using _Base::size;
  void swap(TfHashMap &other)
  {
    _Base::swap(other);
  }
  mapped_type &operator[](const key_type &k)
  {
    return _Base::operator[](k);
  }

  template<class Key2, class Mapped2, class HashFn2, class EqualKey2, class Alloc2>
  friend bool operator==(const TfHashMap<Key2, Mapped2, HashFn2, EqualKey2, Alloc2> &,
                         const TfHashMap<Key2, Mapped2, HashFn2, EqualKey2, Alloc2> &);

 private:
  // _Base::erase() takes an iterator, not a const_iterator.  We happen
  // to know const_iterator and iterator have the same layout.
  static const iterator &_MakeIterator(const const_iterator &i)
  {
    return reinterpret_cast<const iterator &>(i);
  }
};

template<class Key,
         class Mapped,
         class HashFn = __gnu_cxx::hash<Key>,
         class EqualKey = __gnu_cxx::equal_to<Key>,
         class Alloc = __gnu_cxx::allocator<Mapped>>
class TfHashMultiMap : private __gnu_cxx::hash_map<Key, Mapped, HashFn, EqualKey, Alloc>
{

  using _Base = __gnu_cxx::hash_map<Key, Mapped, HashFn, EqualKey, Alloc>;

 public:
  typedef typename _Base::key_type key_type;
  typedef typename _Base::mapped_type mapped_type;
  typedef typename _Base::value_type value_type;
  typedef typename _Base::hasher hasher;
  typedef typename _Base::key_equal key_equal;
  typedef typename _Base::size_type size_type;
  typedef typename _Base::difference_type difference_type;
  typedef typename _Base::pointer pointer;
  typedef typename _Base::const_pointer const_pointer;
  typedef typename _Base::reference reference;
  typedef typename _Base::const_reference const_reference;
  typedef typename _Base::iterator iterator;
  typedef typename _Base::const_iterator const_iterator;
  typedef typename _Base::allocator_type allocator_type;
  // No local_iterator nor any methods using them.

  TfHashMultiMap()
    : _Base()
  {}
  explicit TfHashMultiMap(size_type n,
                          const hasher &hf = hasher(),
                          const key_equal &eql = key_equal(),
                          const allocator_type &alloc = allocator_type())
    : _Base(n, hf, eql, alloc)
  {}
  explicit TfHashMultiMap(const allocator_type &alloc)
    : _Base(0, hasher(), key_equal(), alloc)
  {}
  template<class InputIterator>
  TfHashMultiMap(InputIterator first,
                 InputIterator last,
                 size_type n = 0,
                 const hasher &hf = hasher(),
                 const key_equal &eql = key_equal(),
                 const allocator_type &alloc = allocator_type())
    : _Base(first, last, n, hf, eql, alloc)
  {}
  TfHashMultiMap(const TfHashMultiMap &other)
    : _Base(other)
  {}

  TfHashMultiMap &operator=(const TfHashMultiMap &rhs)
  {
    _Base::operator=(rhs);
    return *this;
  }

  iterator begin()
  {
    return _Base::begin();
  }
  const_iterator begin() const
  {
    return const_iterator(_Base::begin());
  }
  // size_type bucket(const key_type& key) const;
  using _Base::bucket_count;
  size_type bucket_size(size_type n) const
  {
    return _Base::elems_in_bucket(n);
  }
  const_iterator cbegin() const
  {
    return const_iterator(_Base::begin());
  }
  const_iterator cend() const
  {
    return const_iterator(_Base::end());
  }
  using _Base::clear;
  using _Base::count;
  using _Base::empty;
  iterator end()
  {
    return _Base::end();
  }
  const_iterator end() const
  {
    return const_iterator(_Base::end());
  }
  std::pair<iterator, iterator> equal_range(const key_type &key)
  {
    return _Base::equal_range(key);
  }
  std::pair<const_iterator, const_iterator> equal_range(const key_type &key) const
  {
    return _Base::equal_range(key);
  }
  size_type erase(const key_type &key)
  {
    return _Base::erase(key);
  }
  void erase(const_iterator position)
  {
    _Base::erase(_MakeIterator(position));
  }
  void erase(const_iterator first, const_iterator last)
  {
    _Base::erase(_MakeIterator(first), _MakeIterator(last));
  }
  iterator find(const key_type &key)
  {
    return _Base::find(key);
  }
  const_iterator find(const key_type &key) const
  {
    return const_iterator(_Base::find(key));
  }
  using _Base::get_allocator;
  hasher hash_function() const
  {
    return _Base::hash_funct();
  }
  using _Base::insert;
  iterator insert(const_iterator, const value_type &v)
  {
    return insert(v).first;
  }
  using _Base::key_eq;
  float load_factor() const
  {
    return static_cast<float>(static_cast<double>(size()) / bucket_count());
  }
  using _Base::max_bucket_count;
  float max_load_factor() const
  {
    return 1.0;
  }
  // void max_load_factor(float) { }
  using _Base::max_size;
  void rehash(size_type n)
  {
    _Base::resize(__gnu_cxx::__stl_next_prime(n));
  }
  void reserve(size_type n)
  {
    _Base::resize(n);
  }
  using _Base::size;
  void swap(TfHashMultiMap &other)
  {
    _Base::swap(other);
  }
  mapped_type &operator[](const key_type &k)
  {
    return _Base::operator[](k);
  }

  template<class Key2, class Mapped2, class HashFn2, class EqualKey2, class Alloc2>
  friend bool operator==(const TfHashMap<Key2, Mapped2, HashFn2, EqualKey2, Alloc2> &,
                         const TfHashMap<Key2, Mapped2, HashFn2, EqualKey2, Alloc2> &);

 private:
  // _Base::erase() takes an iterator, not a const_iterator.  We happen
  // to know const_iterator and iterator have the same layout.
  static const iterator &_MakeIterator(const const_iterator &i)
  {
    return reinterpret_cast<const iterator &>(i);
  }
};

#endif /* USE_DEPRECATED_GNU_HASH_MAP */

template<class Key, class Mapped, class HashFn, class EqualKey, class Alloc>
inline void swap(TfHashMap<Key, Mapped, HashFn, EqualKey, Alloc> &lhs,
                 TfHashMap<Key, Mapped, HashFn, EqualKey, Alloc> &rhs)
{
  lhs.swap(rhs);
}

template<class Key, class Mapped, class HashFn, class EqualKey, class Alloc>
inline bool operator==(const TfHashMap<Key, Mapped, HashFn, EqualKey, Alloc> &lhs,
                       const TfHashMap<Key, Mapped, HashFn, EqualKey, Alloc> &rhs)
{
  typedef typename TfHashMap<Key, Mapped, HashFn, EqualKey, Alloc>::_Base _Base;
  return static_cast<const _Base &>(lhs) == static_cast<const _Base &>(rhs);
}

template<class Key, class Mapped, class HashFn, class EqualKey, class Alloc>
inline bool operator!=(const TfHashMap<Key, Mapped, HashFn, EqualKey, Alloc> &lhs,
                       const TfHashMap<Key, Mapped, HashFn, EqualKey, Alloc> &rhs)
{
  return !(lhs == rhs);
}

template<class Key, class Mapped, class HashFn, class EqualKey, class Alloc>
inline void swap(TfHashMultiMap<Key, Mapped, HashFn, EqualKey, Alloc> &lhs,
                 TfHashMultiMap<Key, Mapped, HashFn, EqualKey, Alloc> &rhs)
{
  lhs.swap(rhs);
}

template<class Key, class Mapped, class HashFn, class EqualKey, class Alloc>
inline bool operator==(const TfHashMultiMap<Key, Mapped, HashFn, EqualKey, Alloc> &lhs,
                       const TfHashMultiMap<Key, Mapped, HashFn, EqualKey, Alloc> &rhs)
{
  typedef typename TfHashMultiMap<Key, Mapped, HashFn, EqualKey, Alloc>::_Base _Base;
  return static_cast<const _Base &>(lhs) == static_cast<const _Base &>(rhs);
}

template<class Key, class Mapped, class HashFn, class EqualKey, class Alloc>
inline bool operator!=(const TfHashMultiMap<Key, Mapped, HashFn, EqualKey, Alloc> &lhs,
                       const TfHashMultiMap<Key, Mapped, HashFn, EqualKey, Alloc> &rhs)
{
  return !(lhs == rhs);
}

WABI_NAMESPACE_END

#endif  // WABI_BASE_TF_HASHMAP_H
