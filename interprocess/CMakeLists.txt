link_libraries(librt.so libpthread.so)
#target_link_libraries(hello_world librt.so)
add_executable (hello_world test.cpp)

add_executable (test_interprocess_memory_mapped_files
                test_interprocess_memory_mapped_files.cpp)

add_executable (test_interprocess_shm
                test_interprocess_shm.cpp)

add_executable (test_shm_vector
                test_shm_vector.cpp)

add_executable (test_shm_map
                test_shm_map.cpp)

add_executable (test_shm_umap
                test_shm_umap.cpp)

add_executable (test_shm_multi_index
                test_shm_multi_index.cpp)

add_executable (test_shm_cached_node_allocator
                test_shm_cached_node_allocator.cpp)
