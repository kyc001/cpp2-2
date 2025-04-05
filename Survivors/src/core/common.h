#pragma once
#include "defines.h"

/* TODO: This could be the macro way, could test this out
#define TEST_ARRAY(_name, _type, _N) \
struct \
{\
_type data[_N]; \
int count; \
} _name

#define TEST_array_add(_array, _value) ((_array).data[(_array).count++] = (_value))
struct Test 
{
  TEST_ARRAY(array_name, float, 10);
};

void test()
{
  Test t;
  TEST_array_add(t.array_name, 0.f);
}
*/

// @Note(tkap, 29/11/2022): Better to have a bad assert than no assert :smile:
#ifdef DEBUG
#include <stdlib.h>
#include <stdio.h>
#define assert(cond) if(!(cond))                              \
{                                                           \
__debugbreak();                                           \
printf("ASSERT FAILED AT %s (%i)\n", __FILE__, __LINE__); \
exit(1);                                                  \
}
#else
#define assert(cond)
#endif // DEBUG

template <typename T, int N>
struct Array
{
  static constexpr int maxElements = N;
  
  int count = 0;
  T elements[N];
  
  T& operator[](int idx)
  {
    //KYC_ASSERT(idx >= 0, "Idx negative!");
    //KYC_ASSERT(idx < count, "Idx out of bounds!");
    assert(idx >= 0);
    assert(idx < count);
    return elements[idx];
  }
  
  int add(T element)
  {
    //KYC_ASSERT(count < maxElements, "Array Full!");
    assert(count < maxElements);
    elements[count] = element;
    return count++;
  }
  
  void remove_idx_and_swap(int idx)
  {
    //KYC_ASSERT(idx >= 0, "Idx negative!");
    //KYC_ASSERT(idx < count, "Idx out of bounds!");
    assert(idx >= 0);
    assert(idx < count);
    elements[idx] = elements[--count];
  }
  
  void clear()
  {
    count = 0;
  }
  
  bool is_full()
  {
    return count == N;
  }
};

template<typename T, int N>
inline static void array_remove(Array<T, N> *a, int idx)
{
  //KYC_ASSERT(idx >= 0, "Idx negative!");
  //KYC_ASSERT(idx < a->count, "Idx out of bounds!");
  a->count--;
  a->elements[idx] = a->elements[a->count];
}

template<typename T, int N>
inline static int array_add(Array<T, N> *a, T element)
{
  //KYC_ASSERT(a->count < a->max_elements, "Array Full!");
  int result = a->count++;
  a->elements[result] = element;
  return result;
}

template<typename T, int N>
inline static void array_remove_ordered(Array<T, N> *a, int idx)
{
  //KYC_ASSERT(idx >= 0, "Idx negative!");
  //KYC_ASSERT(idx < a->count, "Idx out of bounds!");

  // Shift every element after the index back by one
  for (int i = idx; i < a->count - 1; i++)
  {
    a->elements[i] = a->elements[i + 1];
  }
  a->count--;
}