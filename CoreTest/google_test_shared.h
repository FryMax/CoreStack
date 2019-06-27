#pragma once

#if BUILD_MODE == BUILD_MODE_DEBUG
  #define TEST_ROUNDS 5'000
#else
  #define TEST_ROUNDS 500'000
#endif