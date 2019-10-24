#include "includes/catch.hpp"
#include <iostream>

// Basic format

TEST_CASE( "name", "[tags]" ) {
   REQUIRE( 1/*assertion condition*/ );
   REQUIRE( 1/*assertion condition*/ );
   REQUIRE( 1/*assertion condition*/ );
}


//test case with sections
TEST_CASE( "name", "[tags]" ) {

//  set up code and assertions

    SECTION( "name" ) {
        //further setup 

        REQUIRE( 1/*assertion condition*/ );
    }
    SECTION( "name" ) {
        SECTION("nested section"){

        }
    }
}

//BDD approach (behavioural-driven development)

SCENARIO( "descriptive name", "[tags]" ) {

    GIVEN( "descriptive name" ) {
        //set up code and assertions
        WHEN( "descriptive name") {
            //further setup

            THEN( "descriptive name" ) {
                 REQUIRE( 1/*assertion condition*/ );
            }
        }
          WHEN( "descriptive name") {
            //further setup

            THEN( "descriptive name" ) {
                 REQUIRE( 1/*assertion condition*/ );
            }
        }
    }
}