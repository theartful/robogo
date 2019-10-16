#include <CppUTest/TestHarness.h>

#include <iostream>

TEST_GROUP(FooTestGroup)
{
   void setup()
   {
      // Init stuff
      std::cout << "This runs first before every test in the group is run.\n"
                << "Use this function to initialize resources.\n";
   }

   void teardown()
   {
      // Uninit stuff
      std::cout << "This runs after every test in the group is completed.\n"
                << "Use this function to deallocate any resources.\n";
   }
};

TEST(FooTestGroup, Foo)
{
    // FAIL("Fail me!");
   // Test FOO
}

TEST(FooTestGroup, MoreFoo)
{
   // Test more FOO
}

TEST_GROUP(BarTestGroup)
{
   void setup()
   {
      // Init Bar
   }
};

TEST(BarTestGroup, Bar)
{
   // Test Bar
}