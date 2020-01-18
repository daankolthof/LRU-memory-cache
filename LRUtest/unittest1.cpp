#include "stdafx.h"
#include "CppUnitTest.h"

#include "../LRU-memory-cache/LRU.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LRUtest
{		
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(TestConstruction)
		{
			LRU lru;

			Assert::IsTrue(lru.empty());
			Assert::AreEqual(0u, lru.size());
		}

		TEST_METHOD(TestPut)
		{
			LRU lru;

			lru.put("firstname", "Daan");
			lru.put("lastname", "Kolthof");

			Assert::IsFalse(lru.empty());
			Assert::AreEqual(2u, lru.size());
		}

		TEST_METHOD(TestGetByPointer)
		{
			LRU lru;

			lru.put("firstname", "Daan");
			lru.put("lastname", "Kolthof");

			Assert::IsNotNull(lru.get_by_pointer("firstname"));
			Assert::IsNotNull(lru.get_by_pointer("lastname"));
			Assert::IsNull(lru.get_by_pointer("address"));

			const std::string *firstname_str_ptr = lru.get_by_pointer("firstname");
			Assert::IsNotNull(firstname_str_ptr);
			Assert::AreEqual(std::string("Daan"), *firstname_str_ptr);

			const std::string *lastname_str_ptr = lru.get_by_pointer("lastname");
			Assert::IsNotNull(lastname_str_ptr);
			Assert::AreEqual(std::string("Kolthof"), *lastname_str_ptr);

			const std::string *address_str_ptr = lru.get_by_pointer("address");
			Assert::IsNull(address_str_ptr);
		}

		TEST_METHOD(TestGetByReference)
		{
			LRU lru;

			lru.put("firstname", "Daan");
			lru.put("lastname", "Kolthof");

			std::string firstname_str;
			const bool firstname_get_result = lru.get("firstname", firstname_str);
			Assert::IsTrue(firstname_get_result);
			Assert::AreEqual(std::string("Daan"), firstname_str);

			std::string lastname_str;
			const bool lastname_get_result = lru.get("lastname", lastname_str);
			Assert::IsTrue(lastname_get_result);
			Assert::AreEqual(std::string("Kolthof"), lastname_str);

			std::string address_str;
			const bool address_get_result = lru.get("address", address_str);
			Assert::IsFalse(address_get_result);
			Assert::IsTrue(address_str.empty());
		}

		TEST_METHOD(TestPutOverwrite)
		{
			LRU lru;

			lru.put("firstname", "Bob");
			lru.put("lastname", "Smith");

			lru.put("firstname", "Oliver");

			Assert::IsFalse(lru.empty());
			Assert::AreEqual(2u, lru.size());

			std::string firstname_str;
			const bool firstname_get_result = lru.get("firstname", firstname_str);
			Assert::IsTrue(firstname_get_result);
			Assert::AreEqual(std::string("Oliver"), firstname_str);
		}

		TEST_METHOD(TestOrder)
		{
			LRU lru;

			lru.put("key1", "value1");
			lru.put("key2", "value2");
			lru.put("key3", "value3");

			const LRU::lru_list_type& lru_list = lru.recently_used_list();
			LRU::lru_list_type::const_iterator iter = lru_list.begin();

			Assert::AreEqual(std::string("key3"), iter->first);
			Assert::AreEqual(std::string("value3"), iter->second);
			++iter;
			Assert::AreEqual(std::string("key2"), iter->first);
			Assert::AreEqual(std::string("value2"), iter->second);
			++iter;
			Assert::AreEqual(std::string("key1"), iter->first);
			Assert::AreEqual(std::string("value1"), iter->second);

			Assert::IsTrue(lru_list.end() == ++iter);

			// Key2 is now most recently used.
			lru.get_by_pointer("key2");

			iter = lru_list.begin();

			Assert::AreEqual(std::string("key2"), iter->first);
			Assert::AreEqual(std::string("value2"), iter->second);
			++iter;
			Assert::AreEqual(std::string("key3"), iter->first);
			Assert::AreEqual(std::string("value3"), iter->second);
			++iter;
			Assert::AreEqual(std::string("key1"), iter->first);
			Assert::AreEqual(std::string("value1"), iter->second);

			Assert::IsTrue(lru_list.end() == ++iter);

			// Key1 is now most recently used.
			lru.put("key1", "value1");

			iter = lru_list.begin();

			Assert::AreEqual(std::string("key1"), iter->first);
			Assert::AreEqual(std::string("value1"), iter->second);
			++iter;
			Assert::AreEqual(std::string("key2"), iter->first);
			Assert::AreEqual(std::string("value2"), iter->second);
			++iter;
			Assert::AreEqual(std::string("key3"), iter->first);
			Assert::AreEqual(std::string("value3"), iter->second);

			Assert::IsTrue(lru_list.end() == ++iter);
		}

	};
}
