/*
 * Store a map in shared memory.
 * 
 * typedef map< char_string, complex_data
 *         , std::less<char_string>, map_value_type_allocator>          complex_map_type;
 *
 *  TODO: client iter is not right.
 */ 

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/foreach.hpp>


using namespace boost::interprocess;

//Typedefs of allocators and containers
typedef managed_shared_memory::segment_manager                       segment_manager_t;
typedef allocator<void, segment_manager_t>                           void_allocator;
typedef allocator<int, segment_manager_t>                            int_allocator;
typedef vector<int, int_allocator>                                   int_vector;
typedef allocator<int_vector, segment_manager_t>                     int_vector_allocator;
typedef vector<int_vector, int_vector_allocator>                     int_vector_vector;
typedef allocator<char, segment_manager_t>                           char_allocator;
typedef basic_string<char, std::char_traits<char>, char_allocator>   char_string;

struct complex_data
{
   int               id_;
   char_string       char_string_;
   int_vector_vector int_vector_vector_;

   public:
   //Since void_allocator is convertible to any other allocator<T>, we can simplify
   //the initialization taking just one allocator for all inner containers.
   complex_data(int id, const char *name, const void_allocator &void_alloc)
      : id_(id), char_string_(name, void_alloc), int_vector_vector_(void_alloc)
   {}
   //Other members...
   void print(int is_parent, int i) {
       auto val = *this;
        printf("%s: i=%d, id=%d, name=%s\n", is_parent ? "S" : "C", i, val.id_, val.char_string_.c_str());
   }
};

//Definition of the map holding a string as key and complex_data as mapped type
typedef std::pair<const char_string, complex_data>                      map_value_type;
typedef std::pair<char_string, complex_data>                            movable_to_map_value_type;
typedef allocator<map_value_type, segment_manager_t>                    map_value_type_allocator;
typedef map< char_string, complex_data
           , std::less<char_string>, map_value_type_allocator>          complex_map_type;

struct shm_remove
{
   shm_remove() { shared_memory_object::remove("MySharedMemory"); }
   ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
};


static void
do_client(void_allocator alloc_inst, complex_map_type *mymap) {
       printf("Client reading from map:%p\n", mymap);
       //BOOST_FOREACH( complex_map_type::value_type &i, mymap )
       //    i.second.print(index++);

       int index=0;
       for (auto it=mymap->begin(); it != mymap->end(); ++it) {
           it->second.print(0, index++);
           printf("key: %s\n", it->first.c_str());
       }
       printf("Client read %d entries\n", index);
}

static void
do_server(void_allocator alloc_inst, complex_map_type *mymap) {
       for(int i = 0; i < 10; ++i){
          //Both key(string) and value(complex_data) need an allocator in their constructors
          char key[10] = {};
          snprintf(key, 10, "KEY_%d", i );
          char_string  key_object(key, alloc_inst);
          complex_data mapped_object(i, "default_name", alloc_inst);
          map_value_type value(key_object, mapped_object);
          //Modify values and insert them in the map
          mymap->insert(value);
          mapped_object.print(1, i);
       }
       printf("Server wrote 10 k:v\n");

       int index=0;
       for (auto it=mymap->begin(); it != mymap->end(); ++it) {
           it->second.print(1, index++);
           printf("key: %s\n", it->first.c_str());
       }
       printf("Server read %d entries\n", index);
}

static void
run(int is_parent) 
{
   //Remove shared memory on construction and destruction
   //shm_remove remover;

   //Create shared memory
   managed_shared_memory segment(open_or_create,"MySharedMemory", 65536);

   //An allocator convertible to any allocator<T, segment_manager_t> type
   void_allocator alloc_inst (segment.get_segment_manager());


   printf("%u-%s %d run..\n", ::getpid(), is_parent?"S":"C", is_parent);

   if (is_parent) {
       //Construct the shared memory map and fill it
       complex_map_type *mymap = segment.construct<complex_map_type>
          //(object name), (first ctor parameter, second ctor parameter)
             ("MyMap")(std::less<char_string>(), alloc_inst);
       do_server(alloc_inst, mymap);
       sleep (5);
       shared_memory_object::remove("MySharedMemory");

   } else {
       sleep (3);
       //Find the shared memory map and read it
       auto res = segment.find<complex_map_type> ("MyMap");
       assert(res.first && "map == nullptr");

       do_client(alloc_inst, res.first);
   }

   printf("%u-%s exiting..\n", ::getpid(), is_parent?"S":"C");
}

int main ()
{
    run(fork());
    return 0;
}

