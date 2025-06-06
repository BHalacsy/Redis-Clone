#define CATCH_CONFIG_MAIN

#include "catch2/catch_test_macros.hpp"
#include "catch2/internal/catch_assertion_handler.hpp"
#include "catch2/internal/catch_compiler_capabilities.hpp"
#include "catch2/internal/catch_decomposer.hpp"
#include "catch2/internal/catch_preprocessor_internal_stringify.hpp"
#include "catch2/internal/catch_result_type.hpp"
#include "catch2/internal/catch_test_macro_impl.hpp"
#include "catch2/internal/catch_test_registry.hpp"

TEST_CASE("Temporary test", "[placeholder]")
 {
    REQUIRE(true);
}
