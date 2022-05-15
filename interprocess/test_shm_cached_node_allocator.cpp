#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/cached_node_allocator.hpp>
#include <cassert>

using namespace boost::interprocess;

enum class mode_en {
    PARENT,
    CHILD,
};

static void
do_shm_stuff(mode_en m)
{
   //Remove shared memory on construction and destruction
   struct shm_remove
   {
      shm_remove() { shared_memory_object::remove("MySharedMemory"); }
      ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
   } remover;


   printf("%d: mode=%u\n", ::getpid(), m); 
   //Create shared memory
   managed_shared_memory segment(create_only,
                                 "MySharedMemory",  //segment name
                                 65536);

   //Create a cached_node_allocator that allocates ints from the managed segment
   //The number of chunks per segment is the default value
   typedef cached_node_allocator<int, managed_shared_memory::segment_manager>
      cached_node_allocator_t;
   cached_node_allocator_t allocator_instance(segment.get_segment_manager());

   //The max cached nodes are configurable per instance
   allocator_instance.set_max_cached_nodes(3);

   //Create another cached_node_allocator. Since the segment manager address
   //is the same, this cached_node_allocator will be
   //attached to the same pool so "allocator_instance2" can deallocate
   //nodes allocated by "allocator_instance"
   cached_node_allocator_t allocator_instance2(segment.get_segment_manager());

   //The max cached nodes are configurable per instance
   allocator_instance2.set_max_cached_nodes(5);

   //Create another cached_node_allocator using copy-constructor. This
   //cached_node_allocator will also be attached to the same pool
   cached_node_allocator_t allocator_instance3(allocator_instance2);

   //We can clear the cache
   allocator_instance3.deallocate_cache();

   //All allocators are equal
   assert(allocator_instance == allocator_instance2);
   assert(allocator_instance2 == allocator_instance3);

   //So memory allocated with one can be deallocated with another
   cached_node_allocator_t::pointer ptr1 = allocator_instance.allocate(1);
   cached_node_allocator_t::pointer ptr2 = allocator_instance2.allocate(1);
   allocator_instance2.deallocate(ptr1, 1);
   allocator_instance3.deallocate(ptr2, 1);

   //The common pool will be destroyed here, since no allocator is
   //attached to the pool
}

int main ()
{
    do_shm_stuff(fork() ? mode_en::PARENT: mode_en::CHILD);
   return 0;
}
