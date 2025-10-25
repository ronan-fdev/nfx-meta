/**
 * @file Sample_JSONPSerializer.cpp
 * @brief Comprehensive sample demonstrating JSON Serializer functionality
 * @details Real-world examples showcasing all serializer features including primitive types,
 *          containers, custom objects, nfx datatypes, time types, and serialization options
 */

#include <array>
#include <iostream>
#include <deque>
#include <map>
#include <list>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/ArrayEnumerator.h>
#include <nfx/serialization/json/FieldEnumerator.h>
#include <nfx/serialization/json/Serializer.h>

#include <nfx/containers/HashMap.h>
#include <nfx/containers/StringMap.h>
#include <nfx/containers/StringSet.h>
#include <nfx/datatypes/Int128.h>
#include <nfx/datatypes/Decimal.h>
#include <nfx/datetime/DateTime.h>

using namespace nfx::serialization::json;

//=====================================================================
// Custom data structures for demonstration
//=====================================================================

struct Person
{
	std::string name;
	int age;
	bool isActive;
	std::optional<std::string> email;
	std::vector<std::string> hobbies;

	// Equality operator for comparison
	bool operator==( const Person& other ) const
	{
		return name == other.name && age == other.age &&
			   isActive == other.isActive && email == other.email &&
			   hobbies == other.hobbies;
	}

	// Custom serialization method - no parameters
	Document serialize() const
	{
		Document doc;
		doc.set<std::string>( "/name", name );
		doc.set<int64_t>( "/age", age );
		doc.set<bool>( "/isActive", isActive );

		if ( email.has_value() )
		{
			doc.set<std::string>( "/email", *email );
		}
		// Note: includeNullFields not available without serializer parameter

		// Serialize hobbies array
		if ( !hobbies.empty() )
		{
			Document hobbiesArray;
			hobbiesArray.set<Document::Array>( "" );
			auto hobbiesArrayRef = hobbiesArray.get<Document::Array>( "" ).value();
			for ( const auto& hobby : hobbies )
			{
				hobbiesArrayRef.add( hobby );
			}
			doc.set<Document>( "/hobbies", hobbiesArray );
		}

		return doc;
	}

	// Custom serialization method - with serializer (for options access)
	Document serialize( Serializer<Person>& serializer ) const
	{
		Document doc;
		doc.set<std::string>( "/name", name );
		doc.set<int64_t>( "/age", age );
		doc.set<bool>( "/isActive", isActive );

		if ( email.has_value() )
		{
			doc.set<std::string>( "/email", *email );
		}
		else if ( serializer.options().includeNullFields )
		{
			doc.setNull( "/email" );
		}

		// Serialize hobbies array
		if ( !hobbies.empty() )
		{
			Document hobbiesArray;
			hobbiesArray.set<Document::Array>( "" );
			auto hobbiesArrayRef = hobbiesArray.get<Document::Array>( "" ).value();
			for ( const auto& hobby : hobbies )
			{
				hobbiesArrayRef.add( hobby );
			}
			doc.set<Document>( "/hobbies", hobbiesArray );
		}

		return doc;
	}

	// Alternative serialization method - void with document parameter (different API style)
	void serialize( Serializer<Person>& serializer, Document& doc ) const
	{
		doc = serialize( serializer ); // Delegate to the Document-returning version
	}

	// Custom deserialization method
	void deserialize( const Serializer<Person>& serializer, const Document& doc )
	{
		if ( auto nameVal = doc.get<std::string>( "/name" ) )
		{
			name = *nameVal;
		}
		if ( auto ageVal = doc.get<int64_t>( "/age" ) )
		{
			age = static_cast<int>( *ageVal );
		}
		if ( auto activeVal = doc.get<bool>( "/isActive" ) )
		{
			isActive = *activeVal;
		}

		// Handle optional email
		if ( auto emailVal = doc.get<std::string>( "/email" ) )
		{
			email = *emailVal;
		}
		else
		{
			email = std::nullopt;
		}

		// Deserialize hobbies array manually since we have custom deserialization
		hobbies.clear();
		if ( auto hobbiesDoc = doc.get<Document>( "/hobbies" ) )
		{
			if ( hobbiesDoc->is<Document::Array>( "" ) )
			{
				ArrayEnumerator enumerator( *hobbiesDoc );
				if ( enumerator.setPointer( "" ) )
				{
					enumerator.reset();
					while ( !enumerator.isEnd() )
					{
						Document hobbyDoc = enumerator.currentElement();
						if ( auto hobbyStr = hobbyDoc.get<std::string>( "" ) )
						{
							hobbies.push_back( *hobbyStr );
						}
						if ( !enumerator.next() )
						{
							break;
						}
					}
				}
			}
		}

		// Validation if enabled
		if ( serializer.options().validateOnDeserialize )
		{
			if ( age < 0 || age > 150 )
			{
				throw std::runtime_error( "Invalid age: must be between 0 and 150" );
			}
		}
	}
};

struct Company
{
	std::string name;
	std::vector<Person> employees;
	std::map<std::string, int> departments;
	nfx::datetime::DateTime founded;

	bool operator==( const Company& other ) const
	{
		return name == other.name && employees == other.employees &&
			   departments == other.departments && founded == other.founded;
	}

	// Custom serialization method - no parameters (simple case)
	Document serialize() const
	{
		Document doc;
		doc.set<std::string>( "/name", name );

		// Serialize founded date
		std::string foundedStr = founded.toIso8601Extended();
		doc.set<std::string>( "/founded", foundedStr );

		// Serialize employees array using default serialization
		if ( !employees.empty() )
		{
			Document employeesArray;
			employeesArray.set<Document::Array>( "" );
			auto employeesArrayWrapper = employeesArray.get<Document::Array>( "" );
			for ( const auto& employee : employees )
			{
				// Use the no-parameter serialize method for simplicity
				Document employeeDoc = employee.serialize();
				if ( employeesArrayWrapper.has_value() )
				{
					employeesArrayWrapper->add<Document>( employeeDoc );
				}
			}
			doc.set<Document>( "/employees", employeesArray );
		}

		// Serialize departments map
		if ( !departments.empty() )
		{
			Document departmentsObj;

			for ( const auto& [deptName, count] : departments )
			{
				std::string fieldPath = "/" + deptName;
				departmentsObj.set<int64_t>( fieldPath, count );
			}
			doc.set<Document>( "/departments", departmentsObj );
		}

		return doc;
	}

	// Custom serialization method - with serializer (for options and cross-type serialization)
	Document serialize( Serializer<Company>& companySerializer ) const
	{
		Document doc;
		doc.set<std::string>( "/name", name );

		// Serialize founded date
		std::string foundedStr = founded.toIso8601Extended();
		doc.set<std::string>( "/founded", foundedStr );

		// Serialize employees array
		if ( !employees.empty() )
		{
			Document employeesArray;
			employeesArray.set<Document::Array>( "" );
			auto employeesArrayWrapper = employeesArray.get<Document::Array>( "" );
			for ( const auto& employee : employees )
			{
				// Create Person serializer with same options as Company serializer
				auto personOptions = Serializer<Person>::Options::createFrom<Company>( companySerializer.options() );
				Serializer<Person> personSerializer( personOptions );
				Document employeeDoc = personSerializer.serialize( employee );
				if ( employeesArrayWrapper.has_value() )
				{
					employeesArrayWrapper->add<Document>( employeeDoc );
				}
			}
			doc.set<Document>( "/employees", employeesArray );
		}

		// Serialize departments map
		if ( !departments.empty() )
		{
			Document departmentsObj;

			for ( const auto& [deptName, count] : departments )
			{
				std::string fieldPath = "/" + deptName;
				departmentsObj.set<int64_t>( fieldPath, count );
			}
			doc.set<Document>( "/departments", departmentsObj );
		}

		return doc;
	}

	// Alternative serialization method - void with document parameter (different API style)
	void serialize( Serializer<Company>& companySerializer, Document& doc ) const
	{
		doc = serialize( companySerializer ); // Delegate to the Document-returning version
	}

	// Custom deserialization method
	void deserialize( const Serializer<Company>& serializer, const Document& doc )
	{
		if ( auto nameVal = doc.get<std::string>( "/name" ) )
		{
			name = *nameVal;
		}

		// Deserialize founded date
		if ( auto foundedVal = doc.get<std::string>( "/founded" ) )
		{
			if ( !nfx::datetime::DateTime::tryParse( *foundedVal, founded ) )
			{
				throw std::runtime_error( "Invalid DateTime format in Company::founded" );
			}
		}

		// Deserialize employees array
		employees.clear();
		if ( auto employeesArray = doc.get<Document>( "/employees" ) )
		{
			ArrayEnumerator enumerator( *employeesArray );
			if ( enumerator.setPointer( "" ) && enumerator.isValid() )
			{
				enumerator.reset();
				while ( !enumerator.isEnd() )
				{
					Document employeeDoc = enumerator.currentElement();

					// Create Person serializer with same options as Company serializer
					Serializer<Person>::Options personOptions;
					personOptions.includeNullFields = serializer.options().includeNullFields;
					personOptions.prettyPrint = serializer.options().prettyPrint;
					personOptions.validateOnDeserialize = serializer.options().validateOnDeserialize;

					Serializer<Person> personSerializer( personOptions );
					Person employee = personSerializer.deserialize( employeeDoc );
					employees.push_back( std::move( employee ) );

					if ( !enumerator.next() )
						break;
				}
			}
		}

		// Deserialize departments map
		departments.clear();
		if ( auto departmentsObj = doc.get<Document>( "/departments" ) )
		{
			FieldEnumerator enumerator( *departmentsObj );
			if ( enumerator.setPointer( "" ) && enumerator.isValid() )
			{
				enumerator.reset();
				while ( !enumerator.isEnd() )
				{
					std::string deptName = enumerator.currentKey();
					Document valueDoc = enumerator.currentValue();

					if ( auto countVal = valueDoc.get<int64_t>( "" ) )
					{
						departments[deptName] = static_cast<int>( *countVal );
					}

					if ( !enumerator.next() )
						break;
				}
			}
		}
	}
};

//=====================================================================
// Demonstration functions
//=====================================================================

void demonstratePrimitiveTypes()
{
	std::cout << "=== Primitive Types Serialization ===" << std::endl;

	// Boolean
	bool flag = true;
	std::string json = Serializer<bool>::toJson( flag );
	std::cout << "Boolean: " << json << std::endl;
	bool deserializedFlag = Serializer<bool>::fromJson( json );
	std::cout << "Roundtrip successful: " << ( flag == deserializedFlag ? "YES" : "NO" ) << std::endl;

	// Integer
	int number = 42;
	json = Serializer<int>::toJson( number );
	std::cout << "Integer: " << json << std::endl;

	// Floating point
	double pi = 3.14159;
	json = Serializer<double>::toJson( pi );
	std::cout << "Double: " << json << std::endl;

	// String
	std::string text = "Hello, JSON Serializer!";
	json = Serializer<std::string>::toJson( text );
	std::cout << "String: " << json << std::endl;

	std::cout << std::endl;
}

void demonstrateContainers()
{
	std::cout << "=== STL Containers Serialization ===" << std::endl;

	// Vector
	std::vector<int> numbers{ 1, 2, 3, 4, 5 };
	std::string json = Serializer<std::vector<int>>::toJson( numbers );
	std::cout << "Vector<int>: " << json << std::endl;
	auto deserializedNumbers = Serializer<std::vector<int>>::fromJson( json );
	std::cout << "Roundtrip successful: " << ( numbers == deserializedNumbers ? "YES" : "NO" ) << std::endl;

	// Map
	std::map<std::string, int> scores{ { "Alice", 95 }, { "Bob", 87 }, { "Charlie", 92 } };
	json = Serializer<std::map<std::string, int>>::toJson( scores );
	std::cout << "Map<string,int>: " << json << std::endl;

	// Set
	std::set<std::string> tags{ "cpp", "json", "serialization" };
	json = Serializer<std::set<std::string>>::toJson( tags );
	std::cout << "Set<string>: " << json << std::endl;

	// Array
	std::array<double, 3> coordinates{ 1.0, 2.5, 3.7 };
	json = Serializer<std::array<double, 3>>::toJson( coordinates );
	std::cout << "Array<double,3>: " << json << std::endl;

	std::cout << std::endl;
}

void demonstrateNfxContainers()
{
	std::cout << "=== NFX Containers Serialization ===" << std::endl;

	// HashMap
	nfx::containers::HashMap<std::string, int> hashMap;
	hashMap.insertOrAssign( "key1", 100 );
	hashMap.insertOrAssign( "key2", 200 );
	hashMap.insertOrAssign( "key3", 300 );
	std::string json = Serializer<nfx::containers::HashMap<std::string, int>>::toJson( hashMap );
	std::cout << "HashMap: " << json << std::endl;

	// StringMap
	nfx::containers::StringMap<double> stringMap{
		{ "pi", 3.14159 },
		{ "e", 2.71828 } };
	json = Serializer<nfx::containers::StringMap<double>>::toJson( stringMap );
	std::cout << "StringMap: " << json << std::endl;

	// StringSet
	nfx::containers::StringSet stringSet{ "alpha", "beta", "gamma" };
	json = Serializer<nfx::containers::StringSet>::toJson( stringSet );
	std::cout << "StringSet: " << json << std::endl;

	std::cout << std::endl;
}

void demonstrateNfxDatatypes()
{
	std::cout << "=== NFX Datatypes Serialization ===" << std::endl;

	// Int128
	nfx::datatypes::Int128 bigNumber{ 0x123456789ABCDEF0ULL, 0xFEDCBA9876543210ULL };
	std::string json = Serializer<nfx::datatypes::Int128>::toJson( bigNumber );
	std::cout << "Int128: " << json << std::endl;
	auto deserializedBigNumber = Serializer<nfx::datatypes::Int128>::fromJson( json );
	std::cout << "Roundtrip successful: " << ( bigNumber == deserializedBigNumber ? "YES" : "NO" ) << std::endl;

	// Decimal
	nfx::datatypes::Decimal decimal = nfx::datatypes::Decimal::one();
	json = Serializer<nfx::datatypes::Decimal>::toJson( decimal );
	std::cout << "Decimal: " << json << std::endl;

	std::cout << std::endl;
}

void demonstrateTimeTypes()
{
	std::cout << "=== NFX Time Types Serialization ===" << std::endl;

	// TimeSpan
	nfx::datetime::TimeSpan duration = nfx::datetime::TimeSpan::fromHours( 25.5 );
	std::string json = Serializer<nfx::datetime::TimeSpan>::toJson( duration );
	std::cout << "TimeSpan: " << json << std::endl;
	auto deserializedDuration = Serializer<nfx::datetime::TimeSpan>::fromJson( json );
	std::cout << "Roundtrip successful: " << ( duration == deserializedDuration ? "YES" : "NO" ) << std::endl;

	// DateTime
	nfx::datetime::DateTime date{ 2024, 12, 31, 23, 59, 59, 999 };
	json = Serializer<nfx::datetime::DateTime>::toJson( date );
	std::cout << "DateTime: " << json << std::endl;

	// DateTimeOffset
	nfx::datetime::DateTimeOffset dateOffset{ nfx::datetime::DateTime{ 2024, 6, 15, 18, 30, 45 }, nfx::datetime::TimeSpan::fromHours( -8 ) };
	json = Serializer<nfx::datetime::DateTimeOffset>::toJson( dateOffset );
	std::cout << "DateTimeOffset: " << json << std::endl;

	std::cout << std::endl;
}

void demonstrateOptionalAndSmartPointers()
{
	std::cout << "=== Optional and Smart Pointers ===" << std::endl;

	// Optional with value
	std::optional<int> optValue{ 42 };
	std::string json = Serializer<std::optional<int>>::toJson( optValue );
	std::cout << "Optional<int> with value: " << json << std::endl;

	// Optional without value
	std::optional<int> optEmpty;
	json = Serializer<std::optional<int>>::toJson( optEmpty );
	std::cout << "Optional<int> empty: " << json << std::endl;

	// Unique pointer
	auto uniquePtr = std::make_unique<std::string>( "Unique pointer content" );
	json = Serializer<std::unique_ptr<std::string>>::toJson( uniquePtr );
	std::cout << "Unique_ptr<string>: " << json << std::endl;

	// Shared pointer
	auto sharedPtr = std::make_shared<int>( 123 );
	json = Serializer<std::shared_ptr<int>>::toJson( sharedPtr );
	std::cout << "Shared_ptr<int>: " << json << std::endl;

	std::cout << std::endl;
}

void demonstrateCustomObjects()
{
	std::cout << "=== Custom Objects with Custom Serialization ===" << std::endl;

	// Create a person
	Person person;
	person.name = "John Doe";
	person.age = 30;
	person.isActive = true;
	person.email = "john.doe@example.com";
	person.hobbies = { "reading", "coding", "hiking" };

	// Serialize with default options
	std::string json = Serializer<Person>::toJson( person );
	std::cout << "Person (default): " << json << std::endl;

	// Serialize with pretty printing
	Serializer<Person>::Options prettyOptions;
	prettyOptions.prettyPrint = true;
	json = Serializer<Person>::toJson( person, prettyOptions );
	std::cout << "Person (pretty): " << json << std::endl;

	// Test roundtrip
	Person deserializedPerson = Serializer<Person>::fromJson( json );
	std::cout << "Roundtrip successful: " << ( person == deserializedPerson ? "YES" : "NO" ) << std::endl;

	// Person without email
	Person personNoEmail;
	personNoEmail.name = "Jane Smith";
	personNoEmail.age = 25;
	personNoEmail.isActive = false;
	personNoEmail.hobbies = { "music", "art" };

	// Serialize with includeNullFields option
	Serializer<Person>::Options nullOptions;
	nullOptions.includeNullFields = true;
	nullOptions.prettyPrint = true;
	json = Serializer<Person>::toJson( personNoEmail, nullOptions );
	std::cout << "Person without email (includeNullFields): " << json << std::endl;

	std::cout << std::endl;
}

void demonstrateComplexNesting()
{
	std::cout << "=== Complex Nested Structures ===" << std::endl;

	// Create a company with employees
	Company company;
	company.name = "Tech Innovations Inc.";
	company.founded = nfx::datetime::DateTime{ 2010, 3, 15 };

	// Add employees
	Person ceo;
	ceo.name = "Alice Johnson";
	ceo.age = 45;
	ceo.isActive = true;
	ceo.email = "alice@techinnovations.com";
	ceo.hobbies = { "leadership", "strategy" };

	Person developer;
	developer.name = "Bob Wilson";
	developer.age = 28;
	developer.isActive = true;
	developer.email = "bob@techinnovations.com";
	developer.hobbies = { "coding", "gaming", "coffee" };

	company.employees = { ceo, developer };
	company.departments = { { "Engineering", 50 }, { "Sales", 25 }, { "Marketing", 15 } };

	// Serialize the entire company
	Serializer<Company>::Options options;
	options.prettyPrint = true;
	std::string json = Serializer<Company>::toJson( company, options );
	std::cout << "Complete Company Structure:" << std::endl
			  << json << std::endl;

	// Test roundtrip
	Company deserializedCompany = Serializer<Company>::fromJson( json );
	std::cout << "Complex roundtrip successful: " << ( company == deserializedCompany ? "YES" : "NO" ) << std::endl;

	std::cout << std::endl;
}

void demonstrateValidation()
{
	std::cout << "=== Validation and Error Handling ===" << std::endl;

	// Create JSON with invalid age
	std::string invalidJson = R"({
		"name": "Invalid Person",
		"age": 200,
		"isActive": true
	})";

	try
	{
		Serializer<Person>::Options validationOptions;
		validationOptions.validateOnDeserialize = true;

		Person person = Serializer<Person>::fromJson( invalidJson, validationOptions );
		std::cout << "Validation failed - should not reach here!" << std::endl;
	}
	catch ( const std::exception& e )
	{
		std::cout << "Validation caught error (as expected): " << e.what() << std::endl;
	}

	// Test with validation disabled
	try
	{
		Serializer<Person>::Options noValidationOptions;
		noValidationOptions.validateOnDeserialize = false;

		Person person = Serializer<Person>::fromJson( invalidJson, noValidationOptions );
		std::cout << "Without validation - deserialized person with age: " << person.age << std::endl;
	}
	catch ( const std::exception& e )
	{
		std::cout << "Unexpected error: " << e.what() << std::endl;
	}

	std::cout << std::endl;
}

void demonstrateSerializerClass()
{
	std::cout << "=== Serializer Class Usage ===" << std::endl;

	// Using Serializer class directly
	Serializer<std::vector<int>> vectorSerializer;

	std::vector<int> data{ 10, 20, 30, 40, 50 };

	// Serialize to document
	Document document = vectorSerializer.serialize( data );
	std::cout << "Document serialization: " << document.toJsonString() << std::endl;

	// Serialize to string
	std::string jsonString = vectorSerializer.serializeToString( data );
	std::cout << "String serialization: " << jsonString << std::endl;

	// Deserialize from document
	std::vector<int> fromDoc = vectorSerializer.deserialize( document );
	std::cout << "From document - equal: " << ( data == fromDoc ? "YES" : "NO" ) << std::endl;

	// Deserialize from string
	std::vector<int> fromString = vectorSerializer.deserializeFromString( jsonString );
	std::cout << "From string - equal: " << ( data == fromString ? "YES" : "NO" ) << std::endl;

	// Using serializer with options
	Serializer<std::map<std::string, int>>::Options options;
	options.prettyPrint = true;
	Serializer<std::map<std::string, int>> mapSerializer{ options };

	std::map<std::string, int> mapData{ { "first", 1 }, { "second", 2 }, { "third", 3 } };
	std::string prettyJson = mapSerializer.serializeToString( mapData );
	std::cout << "Pretty printed map:" << std::endl
			  << prettyJson << std::endl;

	std::cout << std::endl;
}

//=====================================================================
// Main function
//=====================================================================

int main()
{
	std::cout << "NFX JSON Serializer - Comprehensive Feature Demonstration" << std::endl;
	std::cout << "==========================================================" << std::endl
			  << std::endl;

	try
	{
		demonstratePrimitiveTypes();
		demonstrateContainers();
		demonstrateNfxContainers();
		demonstrateNfxDatatypes();
		demonstrateTimeTypes();
		demonstrateOptionalAndSmartPointers();
		demonstrateCustomObjects();
		demonstrateComplexNesting();
		demonstrateValidation();
		demonstrateSerializerClass();

		std::cout << "All demonstrations completed successfully!" << std::endl;
	}
	catch ( const std::exception& e )
	{
		std::cerr << "Error during demonstration: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
