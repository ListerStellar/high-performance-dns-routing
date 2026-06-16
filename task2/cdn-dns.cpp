#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <assert.h>

struct Node {
    uint16_t pop_id = 0;
    int prefix_scope = -1;
    Node * left = nullptr;
    Node * right = nullptr;
};

class DNS_Tree {
public:
    DNS_Tree() {
        root = new Node;
    }
    ~DNS_Tree() {
        delete_nodes_recursive(root);
    }
    void add_pop(uint8_t ipv6[16], int prefix, uint16_t pop_id) {
        int prefix_scope = prefix;
        Node * node = root;
        size_t i = 0; uint8_t bitmask = 0b10000000;
        while (prefix > 0) {
            if (ipv6[i] & bitmask) {
                if (!node->right) node->right = new Node;
                node = node->right;
            } else {
                if (!node->left) node->left = new Node;
                node = node->left;
            }
            prefix--;
            i = bitmask == 0b00000001 ? i + 1 : i;
            bitmask = bitmask == 0b00000001 ? 0b10000000 : bitmask >> 1;
        }
        node->pop_id = pop_id;
        node->prefix_scope = prefix_scope;
    }
    void parse_routing_data(const char * file) {
        std::ifstream routing_data(file);
        std::string line;
        uint16_t pop_id;
        int prefix;
        size_t space_pos, prefix_length_pos;
        uint8_t ipv6[16];
        while (getline(routing_data, line)) {
            space_pos = line.find(" ");
            pop_id = std::stoi(line.substr(space_pos + 1));
            prefix_length_pos = line.rfind("/");
            prefix = std::stoi(line.substr(prefix_length_pos + 1, space_pos - prefix_length_pos - 1));
            line.resize(prefix_length_pos);
            inet_pton(AF_INET6, line.data(), ipv6);

            add_pop(ipv6, prefix, pop_id);
        }
        routing_data.close();
    }
    std::pair<uint16_t, int> route(uint8_t ipv6[16], int prefix) const {
        uint16_t pop_id = 0; int prefix_scope = 0;
        Node * node = root;
        size_t i = 0; uint8_t bitmask = 0b10000000;
        while (prefix > 0 && (node->left || node->right)) {
            if (ipv6[i] & bitmask) {
                if (node->right) node = node->right;
                else break;
            } else {
                if (node->left) node = node->left;
                else break;
            }
            if (node->prefix_scope != -1) {
                pop_id = node->pop_id;
                prefix_scope = node->prefix_scope;
            }
            prefix--;
            i = bitmask == 0b00000001 ? i + 1 : i;
            bitmask = bitmask == 0b00000001 ? 0b10000000 : bitmask >> 1;
        }
        return {pop_id, prefix_scope};
    }
private:
    void delete_nodes_recursive(Node * node) {
        if (node->left) delete_nodes_recursive(node->left);
        if (node->right) delete_nodes_recursive(node->right);
        delete node;
    }
    Node * root;
};

std::pair<uint16_t, int> route(const DNS_Tree & dns_tree, std::string ecs) {
    size_t prefix_length_pos = ecs.find("/");
    int prefix = std::stoi(ecs.substr(prefix_length_pos + 1));
    ecs.resize(prefix_length_pos);
    uint8_t ipv6[16];
    inet_pton(AF_INET6, ecs.data(), ipv6);
    return dns_tree.route(ipv6, prefix);
}

void test(const DNS_Tree & dns_tree) {
    // auto res = route(dns_tree, "2001:49f0:d0b8:8a00::/56");
    assert(route(dns_tree, "2001:49f0:d0b8:8a00::/56") == std::make_pair(174, 48));
    std::cout << "All tests passed!" << std::endl;
}

// --------------------------------------------------------------------------------------
// Generated with AI

// Helper function to generate a deeper ECS query.
// It takes the original IP prefix but artificially increases the mask length
// to test the Longest Prefix Match (LPM) fallback logic.
std::string make_deeper_query(std::string base_prefix, int deeper_mask) {
    size_t slash_pos = base_prefix.find("/");
    return base_prefix.substr(0, slash_pos) + "/" + std::to_string(deeper_mask);
}

void test_from_file(const DNS_Tree& dns_tree, const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    int exact_matches = 0;
    int lpm_matches = 0;
    
    std::cout << "Starting automated tests against " << filename << "..." << std::endl;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        // Parse the routing data format: "IPv6_subnet/mask PoP_ID"
        size_t space_pos = line.find(" ");
        uint16_t expected_pop = std::stoi(line.substr(space_pos + 1));
        std::string ecs_query = line.substr(0, space_pos);
        
        size_t slash_pos = ecs_query.find("/");
        int rule_prefix = std::stoi(ecs_query.substr(slash_pos + 1));

        // ==========================================
        // 1. EXACT MATCH TEST
        // The Trie MUST return the exact PoP ID and scope that was inserted.
        // ==========================================
        auto exact_res = route(dns_tree, ecs_query);
        if (exact_res.first != expected_pop || exact_res.second != rule_prefix) {
            std::cerr << "FAIL (Exact): " << ecs_query 
                      << " | Expected PoP: " << expected_pop << " Scope: " << rule_prefix
                      << " | Got PoP: " << exact_res.first << " Scope: " << exact_res.second << std::endl;
            assert(false); // Crash with a detailed log
        }
        exact_matches++;

        // ==========================================
        // 2. LONGEST PREFIX MATCH (LPM) TEST
        // Simulate a client query with a deeper mask than the database rule.
        // Example: DB has /32. Client sends /42.
        // The Trie should fall back and return the /32 rule (assuming no 
        // deeper nested rule exists for this specific IP).
        // ==========================================
        if (rule_prefix <= 48) { // Test relatively broad networks
            // Increase the query mask by 10 bits
            std::string deeper_ecs = make_deeper_query(ecs_query, rule_prefix + 10);
            auto lpm_res = route(dns_tree, deeper_ecs);
            
            // CRITICAL FIX: We do NOT check if PoP ID != 0 here, because 0 is a valid PoP.
            // Instead, we verify that the returned scope is AT LEAST as specific 
            // as the original rule_prefix. (If there's a nested subnet, it might be > rule_prefix).
            // If it's strictly less, it means the algorithm erroneously dropped the valid parent rule.
            if (lpm_res.second < rule_prefix) {
                std::cerr << "FAIL (LPM Mask Drop): Query " << deeper_ecs 
                          << " fell back to scope " << lpm_res.second 
                          << " instead of >= " << rule_prefix << std::endl;
                assert(false);
            }
            lpm_matches++;
        }
    }
    
    // ==========================================
    // 3. COMPLETE MISS (UNROUTABLE IP)
    // ==========================================
    
    // Test an obscure IP address that is definitively not in your BGP dump.
    // Assuming you don't have a catch-all "::/0" rule loaded, 
    // the Trie should return a scope of 0 indicating no match.
    auto miss_res = route(dns_tree, "2001:db8:ffff:ffff::/64");
    
    // If you DO have a default route (::/0), change the expected scope to 0 
    // and verify the PoP ID corresponds to your default PoP.
    assert(miss_res.second == 0); 
    
    std::cout << "SUCCESS! Verified " << exact_matches << " exact rules." << std::endl;
    std::cout << "SUCCESS! Verified " << lpm_matches << " dynamic LPM subnets." << std::endl;
}

// Generated with AI
// --------------------------------------------------------------------------------------

int main() {
    DNS_Tree dns_tree;
    dns_tree.parse_routing_data("routing-data.txt");

    test_from_file(dns_tree, "routing-data.txt");

    return EXIT_SUCCESS;
}