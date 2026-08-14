#include <iostream>
#include <unordered_set>
#include <assert.h>

bool check_domain(const std::unordered_set<std::string>& dns_records, const std::string& domain) {
    std::string lowercase_domain = domain.data();
    std::transform(lowercase_domain.begin(), lowercase_domain.end(), lowercase_domain.begin(), [](unsigned char c){ return std::tolower(c); });
    if (lowercase_domain.starts_with(".") || lowercase_domain.ends_with(".")) return false;
    if (dns_records.contains(lowercase_domain)) return true;

    std::string left_subdomain(lowercase_domain.data());
    std::string right_subdomain(lowercase_domain.data());

    //left subdomain test
    size_t left_idx = left_subdomain.find(".");
    while (left_idx != std::string::npos) {
        left_subdomain = left_subdomain.substr(left_idx + 1);
        left_idx = left_subdomain.find(".");
        if (left_idx == 0) return false;
        if (dns_records.contains("*." + left_subdomain)) return true;
    }

    //right subdomain test
    size_t right_idx = right_subdomain.rfind(".");
    while (right_idx != std::string::npos) {
        right_subdomain.resize(right_idx);
        right_idx = right_subdomain.rfind(".");
        if (right_idx == right_subdomain.size() - 1) return false;
        if (dns_records.contains(right_subdomain + ".*")) return true;
    }

    return false;
}

void test_cases(const std::unordered_set<std::string>& dns_records) { // Generated with AI
    // ==========================================
    // 1. EXACT MATCHES
    // ==========================================
    assert(check_domain(dns_records, "admin.panel.local"));
    assert(check_domain(dns_records, "super.exact.com"));
    assert(check_domain(dns_records, "localhost"));
    assert(check_domain(dns_records, "a.b.c.d.e"));

    // Exact match failures
    assert(!check_domain(dns_records, "admin.panel.local.com")); // Extra suffix
    assert(!check_domain(dns_records, "my.admin.panel.local"));  // Extra prefix
    assert(!check_domain(dns_records, "super.exact.co"));        // Partial typo
    assert(!check_domain(dns_records, "localhos"));              // Partial typo

    // ==========================================
    // 2. LEFT WILDCARDS (*.domain)
    // ==========================================
    // Standard left wildcard matches
    assert(check_domain(dns_records, "api.dev.internal"));
    assert(check_domain(dns_records, "db.cluster.dev.internal")); // Multiple subdomains deep
    assert(check_domain(dns_records, "video.eu.cdn.net"));
    assert(check_domain(dns_records, "test.a"));                  // Extremely short wildcard base

    // Left wildcard failures (Subdomain logic)
    assert(!check_domain(dns_records, "dev.internal"));           // BASE DOMAIN ONLY - MUST BE FALSE! (needs subdomain)
    assert(!check_domain(dns_records, "eu.cdn.net"));             // BASE DOMAIN ONLY - MUST BE FALSE!
    assert(!check_domain(dns_records, "a"));                      // BASE DOMAIN ONLY - MUST BE FALSE!

    // Left wildcard failures (Substring trap - VERY IMPORTANT)
    // "mydev.internal" should fail because it lacks the dot separating the wildcard.
    // If your algorithm just uses .endswith("dev.internal"), it will falsely pass this!
    assert(!check_domain(dns_records, "mydev.internal"));
    assert(!check_domain(dns_records, "super-eu.cdn.net"));

    // ==========================================
    // 3. RIGHT WILDCARDS (domain.*)
    // ==========================================
    // Standard right wildcard matches
    assert(check_domain(dns_records, "static-assets.com"));
    assert(check_domain(dns_records, "static-assets.co.uk"));
    assert(check_domain(dns_records, "static-assets.internal.net")); // Multiple zones deep
    assert(check_domain(dns_records, "user-content.org"));
    assert(check_domain(dns_records, "b.xyz"));                      // Extremely short wildcard base

    // Right wildcard failures (Subdomain logic)
    assert(!check_domain(dns_records, "static-assets"));          // BASE DOMAIN ONLY - MUST BE FALSE!
    assert(!check_domain(dns_records, "user-content"));           // BASE DOMAIN ONLY - MUST BE FALSE!
    assert(!check_domain(dns_records, "b"));                      // BASE DOMAIN ONLY - MUST BE FALSE!

    // Right wildcard failures (Substring trap - VERY IMPORTANT)
    // "static-assets-new.com" should fail.
    // If your algorithm just uses .startswith("static-assets"), it will falsely pass this!
    assert(!check_domain(dns_records, "static-assets-new.com"));
    assert(!check_domain(dns_records, "user-content-1.org"));

    // ==========================================
    // 4. OVERLAPPING RULES (Exact + Wildcard)
    // ==========================================
    // Database has both "overlap.com" (exact) and "*.overlap.com" (left wildcard)
    assert(check_domain(dns_records, "overlap.com"));             // Matches exact rule
    assert(check_domain(dns_records, "sub.overlap.com"));         // Matches wildcard rule
    assert(check_domain(dns_records, "deep.sub.overlap.com"));    // Matches wildcard rule

    // ==========================================
    // 5. MALFORMED INPUTS & EXTREME EDGE CASES
    // ==========================================
    // Empty and almost empty strings
    assert(!check_domain(dns_records, ""));
    assert(!check_domain(dns_records, "."));

    // Multiple consecutive dots (Algorithm should not crash or falsely match)
    assert(!check_domain(dns_records, ".."));
    assert(!check_domain(dns_records, "dev..internal"));
    assert(!check_domain(dns_records, "api...dev.internal"));
    assert(!check_domain(dns_records, "static-assets..com"));

    // FQDN style domains (trailing dot). 
    // In DNS, "google.com." is valid. Depending on your system's strictness, 
    // this usually fails unless explicitly stripped before checking. 
    // For this algorithm, treating it as raw string matching, it should fail.
    assert(!check_domain(dns_records, "api.dev.internal."));
    assert(!check_domain(dns_records, "super.exact.com."));

    // Leading dots
    assert(!check_domain(dns_records, ".dev.internal"));
    assert(!check_domain(dns_records, ".static-assets.com"));

    // Reversed or completely scrambled logic
    assert(!check_domain(dns_records, "internal.dev.api"));       // Reversed left wildcard
    assert(!check_domain(dns_records, "com.static-assets"));      // Reversed right wildcard

    // ==========================================
    // 6. STRICT RFC AND NGINX EDGE CASES
    // ==========================================
    
    // Case-Insensitivity
    // The Nginx documentation explicitly states: "Search for a match is case-insensitive."
    // Your algorithm MUST convert the input to lowercase before checking.
    assert(check_domain(dns_records, "API.DEV.INTERNAL")); 
    assert(check_domain(dns_records, "STATIC-ASSETS.COM"));
    assert(check_domain(dns_records, "LoCaLhOsT"));

    // Invalid characters (RFC violations)
    assert(!check_domain(dns_records, "dev.internal "));       // Trailing space
    assert(!check_domain(dns_records, "api.dev/internal"));    // Slash instead of a dot
    
    // Extremely long domains (Memory/Performance stress test)
    std::string long_domain = "a.b.c.d.e.f.g.h.i.j.k.l.m.n.o.p.dev.internal";
    assert(check_domain(dns_records, long_domain));            // Should match *.dev.internal

    // Prefix/Suffix overlap traps (Without a dot separator)
    assert(!check_domain(dns_records, "notlocalhost"));
    assert(!check_domain(dns_records, "localhostnot"));

    std::cout << "All tests passed!" << std::endl;
}

int main() {

    std::unordered_set<std::string> dns_records = { // Generated with AI
        "admin.panel.local",
        "super.exact.com",
        "overlap.com",
        "localhost",
        "a.b.c.d.e",
        "*.dev.internal",
        "*.eu.cdn.net",
        "*.overlap.com",
        "*.a",
        "static-assets.*",
        "user-content.*",
        "b.*"
    };

    test_cases(dns_records);

    return EXIT_SUCCESS;
}