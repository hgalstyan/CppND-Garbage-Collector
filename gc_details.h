#ifndef CPPND_GARBAGE_COLLECTOR_GC_DETAILS_H_
#define CPPND_GARBAGE_COLLECTOR_GC_DETAILS_H_

// This class defines an element that is stored
// in the garbage collection information list.
//
template<typename T>
struct PtrDetails {
 public:
  unsigned refcount;   // current reference count
  T *memPtr;  // pointer to allocated memory

  // isArray is true is memPtr points
  // to an allocated array. It is false otherwise.
  bool isArray;  // true if pointing to array

  // If memPtr is pointing to an allocated
  // array, then arraySize contains its size.
  unsigned arraySize;  // size of array
  // Here, memPtr points to the allocated memory.
  // If this is an array, then size specifies
  // the size of the array.

  PtrDetails() 
      : refcount(0), memPtr(nullptr), isArray(false), arraySize(0) {}
};

// Overloading operator== allows two class objects to be compared.
// This is needed by the STL list class.
template<typename T>
bool operator==(const PtrDetails<T> &ob1,
                const PtrDetails<T> &ob2) {
  return ob1.memPtr == ob2.memPtr;
}

#endif  // CPPND_GARBAGE_COLLECTOR_GC_DETAILS_H_
