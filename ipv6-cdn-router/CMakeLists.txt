add_executable(ipv6-router main.cpp)

# Copy routing data to the build directory so the executable can read it during testing
configure_file(routing-data.txt routing-data.txt COPYONLY)
