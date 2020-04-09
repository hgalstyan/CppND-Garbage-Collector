#ifndef CPPND_GARBAGE_COLLECTOR_GC_ITERATOR_H_
#define CPPND_GARBAGE_COLLECTOR_GC_ITERATOR_H_
// Exception thrown when an attempt is made to
// us an Iter that exeeds the range of the
// underlying object.
//

class OutOfRangeExc {
  // Add functionality if needed by your application.
};

// An iterator-like class for cycling through arrays
// that are pointed to by GCPtrs. Iter pointers
// ** do not ** participate in or affect garbage
// collection. Thus, an Iter pointing to
// some object does not prevent that object
// from being recycled.
//
template<typename T>
class Iter {
 public:
  // Types
  
  // Constants

  // Constructors
  Iter() : ptr(nullptr), begin(nullptr), end(nullptr), length(0) {}
  Iter(T *p, T *first, T *last) 
      : ptr(p), begin(first), end(last), length(last - first) {}

  // Assignment operators

  // Destructor

  // All other methods

  // Return length of sequence to which this Iter points.
  unsigned size() { return length; }

  // Return value pointed to by ptr.
  // Do not allow out-of-bounds access.
  T &operator*() {
    if ((ptr >= end) || (ptr < begin))
      throw OutOfRangeExc();
    return *ptr;
  };

  // Return address contained in ptr.
  // Do not allow out-of-bounds access.
  T *operator->() {
    if ((ptr >= end) || (ptr < begin))
      throw OutOfRangeExc();
    return ptr;
  }

  // Prefix ++.
  Iter operator++() {
    ++ptr;
    return *this;
  }

  // Postfix ++.
  Iter operator++(int) {
    T *tmp = ptr;
    ++ptr;
    return Iter<T>(tmp, begin, end);
  }

  // Prefix --.
  Iter operator--() {
    --ptr;
    return *this;
  }

  // Postfix --.
  Iter operator--(int) {
    T *tmp = ptr;
    --ptr;
    return Iter<T>(tmp, begin, end);
  }

  // Return a reference to the object at the
  // specified index. Do not allow out-of-bounds
  // access.
  T &operator[](int i) {
    if ((i < 0) || (i >= length))
      throw OutOfRangeExc();
    return ptr[i];
  }

  // Define the relational operators.
  bool operator==(const Iter &op2) { return ptr == op2.ptr; }
  bool operator!=(const Iter &op2) { return ptr != op2.ptr; }
  bool operator<(const Iter &op2) { return ptr < op2.ptr; }
  bool operator<=(const Iter &op2) { return ptr <= op2.ptr; }
  bool operator>(const Iter &op2) { return ptr > op2.ptr; }
  bool operator>=(const Iter &op2) { return ptr >= op2.ptr; }

  // Subtract an integer from an Iter.
  Iter operator-(int n) {
    ptr -= n;
    return *this;
  }

  // Add an integer to an Iter.
  Iter operator+(int n) {
    ptr += n;
    return *this;
  }

  // Return number of elements between two Iters.
  int operator-(Iter<T> &itr2) {
    return ptr - itr2.ptr;
  }

 private:
  T *ptr;  // current pointer value
  T *begin;        // points to start of allocated array
  T *end;  // points to element one past end
  unsigned length;  // length of sequence
};

#endif  // CPPND_GARBAGE_COLLECTOR_GC_ITERATOR_H_
