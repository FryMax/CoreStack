#include "pch.h"
#include "google_test_shared.h"

using namespace LIB_NAME;
#define Test(t) TEST(GUID, t)

namespace
{
	Test(ConvertString)
	{
		for (size_t i = 0; i++ < TEST_ROUNDS;)
		{
			const auto uid = UID::gen_rand_uid();
			const auto str = UID::uid_to_string(uid);
			EXPECT_EQ(UID::uid_from_string(str), uid);
		}
	}

	Test(MurMur_HashCollision)
	{
		std::set<uint64_t> collection;

        for (size_t i = 0; i++ < TEST_ROUNDS;)
		{
			const uint64_t hash = math::hash64(i);
			const auto it = collection.find(hash);
			if (it == collection.end())
			{
				collection.emplace_hint(it, hash);
			}
			else
			{
				// collision
				EXPECT_TRUE(false);
			}
		}
		EXPECT_TRUE(true);
	}

}