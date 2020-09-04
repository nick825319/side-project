file(GLOB schedulingSources *.cpp)
file(GLOB schedulingIncludes *.h )
message("-- schedulingSources:  ${schedulingSources}")
message("-- schedulingIncludes:  ${schedulingIncludes}")

find_package(Boost 1.55.0 REQUIRED COMPONENTS system filesystem )

include_directories(${Boost_INCLUDE_DIRS} ./)

cuda_add_executable(scheduling ${schedulingSources})

link_directories(${Boost_LIBRARY_DIRS})
target_link_libraries(scheduling jetson-inference ${Boost_LIBRARIES} -lcrypto -li2c)

#install(TARGETS segnet-camera DESTINATION bin)
