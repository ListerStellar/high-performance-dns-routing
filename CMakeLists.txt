cmake_minimum_required(VERSION 3.10)
project(HighPerformanceDNSRouting)

# Require C++20 for string features like starts_with / ends_with
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add sub-components
add_subdirectory(wildcard-dns-matcher)
add_subdirectory(ipv6-cdn-router)
