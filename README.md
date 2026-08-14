# High-Performance DNS & CDN Routing

This repository contains two high-performance C++ networking components designed to handle DNS wildcard matching and CDN IPv6 routing efficiently. These algorithms focus on optimal time complexities for high-throughput networking environments.

## Components

### 1. Wildcard DNS Matcher (`/wildcard-dns-matcher`)
A highly optimized C++ component for evaluating DNS wildcard records in **O(1) average time complexity**.

**Features:**
- Exact domain matches.
- Left (`*.domain.com`) and right (`domain.*`) wildcard resolution.
- Subdomain string manipulation and robust edge-case handling.
- Implemented using `std::unordered_set` for O(1) average time lookups.
- Case-insensitive checking (in compliance with Nginx / RFC standards).

**How it works:**
The algorithm converts the incoming domain to lowercase. It then iteratively splits and evaluates subdomains from the left and right, matching them against the hash set of known wildcard records. This completely avoids O(N) linear iteration over all records.

---

### 2. IPv6 CDN Routing Tree (`/ipv6-cdn-router`)
A specialized binary tree implementation for determining the closest CDN Point of Presence (PoP) based on the client's IPv6 subnet, commonly used in EDNS Client Subnet (ECS) resolution (RFC 7871).

**Features:**
- Longest Prefix Match (LPM) logic for IPv6 addresses.
- Pointer-based Binary Tree (Trie) for IP routing.
- Direct conversion of string subnets to `uint8_t` arrays using `inet_pton` for maximum speed.
- High-performance bit-by-bit tree traversal to resolve queries to the most specific PoP.

**How it works:**
The data structure parses a comprehensive list of routing data (subnets, prefix masks, and PoP IDs) and constructs a binary tree. When an ECS query is received, the tree is traversed bit by bit based on the IPv6 address. The algorithm keeps track of the deepest valid `pop_id` and `prefix_scope` encountered, guaranteeing the most specific subnet match.

## Future Improvements for Production Use
- **Radix Tree Implementation:** Replace the simple Binary Tree in the IPv6 CDN router with a Radix Tree (Patricia Trie) to significantly reduce the spatial complexity (node count in memory) on massive BGP routing tables.
- **Smart Pointers:** Refactor raw pointer management to `std::unique_ptr` / `std::shared_ptr` to ensure zero memory leaks.
- **Thread Safety:** Introduce mutexes or read-write locks (`std::shared_mutex`) to allow safe concurrent lookups across multiple worker threads.
- **Data Validation:** Add robust input validation for incoming IPv6 configurations and handle potentially malformed routing data safely.

## Build and Run (CMake)
The project is configured with **CMake** for an easy, cross-platform build process. A modern C++ compiler supporting C++20 is required (for features like `starts_with` and `ends_with`).

```bash
# 1. Clone the repository and navigate into it
# 2. Create a build directory and configure the project
mkdir build && cd build
cmake ..

# 3. Compile the binaries
cmake --build .

# 4. Run the tests
./wildcard-dns-matcher/dns-matcher
./ipv6-cdn-router/ipv6-router
```
