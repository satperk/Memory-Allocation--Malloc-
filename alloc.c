/**
 * Malloc
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


typedef struct _metadata_t {
  unsigned int size;
  unsigned char is_free;
  struct _metadata_t *prev;
  struct _metadata_t *next;
} metadata_t;

void *startOfHeap = NULL;

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
    return malloc(size*num);

}


/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */

void *malloc(size_t size) {
  unsigned int null_case = 0;
  unsigned int non_null_case = 1;
  size_t m_size = sizeof(metadata_t);
  if (size) {
    if (startOfHeap == NULL) {
    startOfHeap = sbrk(null_case);
  }

  metadata_t *curr_m_data = startOfHeap;
  void *endOfHeap = sbrk(null_case);
  while ((void *)curr_m_data < endOfHeap) {
    if (curr_m_data->is_free) {

        size_t new_block_size = curr_m_data->size - size - m_size;
        if (new_block_size > null_case) {
          metadata_t *new_metadata = (void *)curr_m_data + size + m_size;
          new_metadata->size = new_block_size;
          new_metadata->is_free = null_case;
          curr_m_data->size = size;
        } else {
          size = curr_m_data->size;
          curr_m_data->size = size;
        }

        return (void *)curr_m_data + sizeof(metadata_t);  
      } else {
        if(curr_m_data->size < size) {
        metadata_t* n = (void *)curr_m_data + curr_m_data->size + m_size;
        if (!n->is_free){
          size_t to_add = size + (curr_m_data->size + m_size);
          if (to_add <= n->size) {
            curr_m_data->size = size;

            metadata_t *n_meta = (void *)curr_m_data + size + m_size;
            n_meta->is_free = null_case;
            n_meta->size = n->size + to_add;
            
            curr_m_data->is_free = non_null_case;
            return (void *)curr_m_data + m_size;
          }
        }
      }
    } 
    curr_m_data = (void *)curr_m_data + curr_m_data->size + sizeof(metadata_t);   
  }
  
  metadata_t *meta = sbrk( sizeof(metadata_t));
  meta->size = size;
  meta->is_free = non_null_case;

  void *ptr = sbrk(meta->size);

  return ptr;
  }

  return NULL;
}


/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {
  size_t m_size = sizeof(metadata_t);
  metadata_t *curr_block = ptr + m_size;
  metadata_t * next_block = ptr + curr_block->size;
  if (!next_block->is_free) {
    unsigned int null_case = 0;
    curr_block->is_free = null_case;
  } else {
    curr_block->size += next_block->size + m_size;
  }
  
}


/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
    if (ptr != NULL) {
      size_t m_size = sizeof(metadata_t);
      void *new_block = ptr + m_size;
      metadata_t *new_m_data = (metadata_t *) new_block;

      if (new_m_data->size >= size) return ptr;

      void *curr_memory = malloc(size);
      
      if (curr_memory != NULL) {

        memcpy(curr_memory, ptr, new_m_data->size);
        free(ptr);

        return curr_memory;
      } else {
        return NULL;
      }
    } else {
      return malloc(size);
    }
}
