#ifndef CPPND_GARBAGE_COLLECTOR_GC_POINTER_H_
#define CPPND_GARBAGE_COLLECTOR_GC_POINTER_H_

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <list>
#include <typeinfo>
#include <utility>

#include "gc_details.h"
#include "gc_iterator.h"

// Pointer implements a pointer type that uses
// garbage colection to release unused memory.
// A Pointer must only be used to point to memory
// that was dynamically allocated using new.
// When used to refer to an allocated array,
// specify the array size.
template<typename T, int size = 0>
class Pointer {
 public:
  // Collect garbage. Returns true if at least
  // one object was freed.
  static bool Collect();
  // A utility function that displays refContainer.
  static void ShowList();
  // Clear refContainer when program exits.
  static void Shutdown();
  // Return the size of refContainer for this type of Pointer.
  static int RefContainerSize() { return refContainer.size(); }

  template<typename U, int sizeU>
  friend void swap(Pointer<U, sizeU> &, Pointer<U, sizeU> &);

  // Types
  // Define an iterator type for Pointer<T>.
  using GCIterator = Iter<T>;

  // Constants

  // Factory functions

  // Constructors
  Pointer() : Pointer(nullptr) {}
  Pointer(T*);

  // Copy constructor.
  Pointer(const Pointer &ob) : Pointer(ob.addr) {}

  // Assignment operators
  // Overload assignment of pointer to Pointer.
  T *operator=(T *t);

  // Overload assignment of Pointer to Pointer.
  Pointer &operator=(Pointer ptr);

  // Destructors
  // Destructor for Pointer.
  ~Pointer();

  // All other methods
  // Return a reference to the object pointed
  // to by this Pointer.
  T &operator*() { return *addr; }

  // Return the address being pointed to.
  T *operator->() { return addr; }

  // Return a reference to the object at the
  // index specified by i.
  T &operator[](int i) { return addr[i]; }

  // Conversion function to T *.
  operator T*() { return addr; }

  // Return an Iter to the start of the allocated memory.
  Iter<T> begin() {
    int _size = (isArray) ? arraySize : 1;
    return Iter<T>(addr, addr, addr + _size);
  }

  // Return an Iter to one past the end of an allocated array.
  Iter<T> end() {
    int _size = (isArray) ? arraySize : 1;
    return Iter<T>(addr + _size, addr, addr + _size);
  }

  // Data members

 private:
  // Return an iterator to pointer details in refContainer.
  typename std::list<PtrDetails<T>>::iterator FindPtrInfo(T *ptr);

  // refContainer maintains the garbage collection list.
  static std::list<PtrDetails<T>> refContainer;
  static bool first;  // true when first Pointer is created
  
  // addr points to the allocated memory to which
  // this Pointer pointer current points.
  T *addr;

  // isArray is true if this Pointer points
  // to an allocated array. It is false otherwise.
  bool isArray;  // true if pointing to array

  // If this Pointer is pointing to an allocated
  // array, then arraySize contains its size.
  unsigned arraySize;  // size of the array
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template<typename T, int size>
std::list<PtrDetails<T>> Pointer<T, size>::refContainer;

template<typename T, int size>
bool Pointer<T, size>::first = true;

// Collect garbage. Returns true if at least
// one object was freed.
template<typename T, int size>
bool Pointer<T, size>::Collect() {
  // TODO: Implement Collect function
  bool freed = false;

  typename std::list<PtrDetails<T>>::iterator p;

  do {
    for (p = refContainer.begin(); p != refContainer.end(); ++p) {
      if (p->refcount > 0) continue;
      refContainer.erase(p);
      if (p->isArray) delete[] p->memPtr;
      else delete p->memPtr;
      freed = true;
      break;
    }
  } while (p != refContainer.end());
  return freed;
}

// // A utility function that displays refContainer.
// template<typename T, int size>
// void Pointer<T, size>::ShowList() {
//   typename std::list<PtrDetails<T>>::iterator p;
//   std::cout << "ref_container<" << typeid(T).name() << ", " << size << ">:\n";
//   std::cout << "mem_ptr refcount value\n ";
//   if (refContainer.begin() == refContainer.end())
//     std::cout << " Container is empty!\n\n ";

//   for (p = refContainer.begin(); p != refContainer.end(); ++p) {
//     std::cout << "[" << (void *)p->memPtr << "]" << " " << p->refcount << " ";
//     if (p -> memPtr)
//       std::cout << " " << *p->memPtr;
//     else
//       std::cout << "---";
//     std::cout << std::endl;
//   }
//   std::cout << std::endl;
// }

// Clear refContainer when program exits.
template<typename T, int size>
void Pointer<T, size>::Shutdown() {
  if (RefContainerSize() == 0)
    return;  // list is empty
  typename std::list<PtrDetails<T>>::iterator p;
  for (p = refContainer.begin(); p != refContainer.end(); ++p)
    p->refcount = 0;  // Set all reference counts to zero
  Collect();
}

// Find a pointer in refContainer.
template<typename T, int size>
typename std::list<PtrDetails<T>>::iterator
Pointer<T, size>::FindPtrInfo(T *ptr) {
  typename std::list<PtrDetails<T>>::iterator p;
  // Find ptr in refContainer.
  for (p = refContainer.begin(); p != refContainer.end(); ++p)
    if (p->memPtr == ptr)
      return p;
  return p;
}

// Swap method
template<typename T, int size>
void swap(Pointer<T, size> &first, Pointer<T, size> &second) {

  using std::swap;

  swap(first.addr, second.addr);
  swap(first.isArray, second.isArray);
  swap(first.arraySize, second.arraySize);
}

// Constructor for both initialized and uninitialized objects.
// See class interface.
template<typename T, int size>
Pointer<T, size>::Pointer(T *t) {
  // Register Shutdown() as an exit function.
  if (first)
    atexit(Shutdown);
  first = false;

  addr = t;
  isArray = (size > 0);
  arraySize = size;

  auto ptrInfo = FindPtrInfo(t);
  if (ptrInfo != refContainer.end()) {
    ++ptrInfo->refcount;
  } else {
    PtrDetails<T> ptrDetails;
    ptrDetails.refcount = 1;
    ptrDetails.memPtr = t;
    ptrDetails.isArray = isArray;
    ptrDetails.arraySize = arraySize;
    refContainer.push_back(ptrDetails);
  }
}


// Overload assignment of pointer to Pointer.
template<typename T, int size>
T *Pointer<T, size>::operator=(T *t) {
  Pointer<T, size> temp(t);
  swap(*this, temp);
  return t;
}

// Overload assignment of Pointer to Pointer.
template<typename T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer ptr) {
  swap(*this, ptr);
  return *this;
}

// Destructor
template<typename T, int size>
Pointer<T, size>::~Pointer() {
  auto ptrInfo = FindPtrInfo(addr);
  if (ptrInfo != refContainer.end()) {
    if (ptrInfo->refcount > 0) --ptrInfo->refcount;
    if (ptrInfo->refcount == 0) Collect();
  }
}

#endif  // CPPND_GARBAGE_COLLECTOR_GC_POINTER_H_
