#include "../src/engine/board.h"
#include "includes/catch.hpp"
// Basic format

TEST_CASE("Board access/set Basics ", "[tags]")
{

	go::engine::BoardState board_state;

	for (uint32_t i = 0; i < board_state.MAX_BOARD_SIZE; i++)
	{
		for (uint32_t j = 0; j < board_state.MAX_BOARD_SIZE; j++)
		{
			REQUIRE(board_state(i, j) == go::engine::Cell::EMPTY);
		}
	}

	board_state(0, 0) = go::engine::Cell::BLACK;
	REQUIRE(board_state(0, 0) == go::engine::Cell::BLACK);

	board_state(1, 1) = go::engine::Cell::BLACK;
	board_state(0, 0) = go::engine::Cell::WHITE;

	REQUIRE(board_state(0, 0) == go::engine::Cell::WHITE);
	REQUIRE(board_state(1, 1) == go::engine::Cell::BLACK);
}

// BDD approach (behavioural-driven development)

SCENARIO("descriptive name", "[tags]")
{

	GIVEN("descriptive name")
	{
		// set up code and assertions
		WHEN("descriptive name")
		{
			// further setup

			THEN("descriptive name")
			{
				REQUIRE(1 /*assertion condition*/);
			}
		}
		WHEN("descriptive name")
		{
			// further setup

			THEN("descriptive name")
			{
				REQUIRE(1 /*assertion condition*/);
			}
		}
	}
}