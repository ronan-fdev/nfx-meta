/**
 * @file TESTS_JSONObjects.cpp
 * @brief Comprehensive tests for JSON Object class functionality
 * @details Tests covering object construction, serialization methods, factory methods,
 *          validation, and integration with Document system
 */

#include <gtest/gtest.h>

#include <nfx/serialization/json/Document.h>

namespace nfx::serialization::json::test
{
	//=====================================================================
	// Object serialization and factory methods tests
	//=====================================================================

	class JSONObjectTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			// Create test document with object structure
			std::string jsonStr = R"({
				"user": {
					"name": "Alice",
					"age": 30,
					"active": true,
					"height": 1.65,
					"spouse": null,
					"preferences": {
						"theme": "dark",
						"notifications": true
					},
					"hobbies": ["reading", "gaming", "cooking"]
				},
				"settings": {
					"volume": 0.8,
					"language": "en",
					"debug": false
				}
			})";

			auto doc = Document::fromJsonString( jsonStr );
			ASSERT_TRUE( doc.has_value() );
			testDoc = std::move( doc.value() );
		}

		Document testDoc;
	};

	//----------------------------------------------
	// Object serialization methods (toJsonString/toJsonBytes)
	//----------------------------------------------

	TEST_F( JSONObjectTest, ToJsonStringEmpty )
	{
		Document emptyDoc;
		auto emptyObj = emptyDoc.get<Document::Object>( "" );
		ASSERT_TRUE( emptyObj.has_value() );

		std::string jsonStr = emptyObj.value().toJsonString();
		EXPECT_EQ( jsonStr, "{}" );
	}

	TEST_F( JSONObjectTest, ToJsonBytesEmpty )
	{
		Document emptyDoc;
		auto emptyObj = emptyDoc.get<Document::Object>( "" );
		ASSERT_TRUE( emptyObj.has_value() );

		std::vector<uint8_t> jsonBytes = emptyObj.value().toJsonBytes();
		std::string jsonStr( jsonBytes.begin(), jsonBytes.end() );
		EXPECT_EQ( jsonStr, "{}" );
	}

	//----------------------------------------------
	// Object validation methods (isValid/lastError)
	//----------------------------------------------

	TEST_F( JSONObjectTest, IsValidForValidObject )
	{
		auto userObj = testDoc.get<Document::Object>( "user" );
		ASSERT_TRUE( userObj.has_value() );

		EXPECT_TRUE( userObj.value().isValid() );
		EXPECT_TRUE( userObj.value().lastError().empty() );
	}

	TEST_F( JSONObjectTest, IsValidForEmptyObject )
	{
		Document emptyDoc;
		auto emptyObj = emptyDoc.get<Document::Object>( "" );
		ASSERT_TRUE( emptyObj.has_value() );

		EXPECT_TRUE( emptyObj.value().isValid() );
		EXPECT_TRUE( emptyObj.value().lastError().empty() );
	}
} // namespace nfx::serialization::json::test
