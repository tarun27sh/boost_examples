link_libraries(librt.so)
#target_link_libraries(hello_world librt.so)
add_executable (hello_world test.cpp)

add_executable (test_interprocess_memory_mapped_files test_interprocess_memory_mapped_files.cpp)

add_executable (test_interprocess_shm test_interprocess_shm.cpp)
