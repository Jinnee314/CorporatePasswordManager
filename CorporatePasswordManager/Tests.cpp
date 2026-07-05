#include "Tests.hpp"
#include "UnitTestFreimwork.hpp"
#include "Crypto.hpp"

#include <iterator>
#include <utility>

/*
* 
* Пример тестов с использование фреймворка UnitTestFreimwork.
* Эти тесты я использовал при написании парольного менеджера.
* 
* Есть точечные функции тестирования, которые проверяют конкретные куски кода. Это testConstructAndDestruct(), testGetNames() и т.д.
* А есть общая функция тестирования testAll(), в которой создаётся объект класса TestRunner и которая через этот объект прогоняет 
* точечные функции тестирования.
* 
* Т.е. сначала создаются функции, которые тестируют конкретный код, а затем они обрабатываются в общей функции.

* Функция точечного тестирования кода, в данном случае конструктора и деструктора класса PasswordManager
void testConstructAndDestruct()
{
	* В таких секциях отделял куски тестов, чтобы не парится с новыми именами и объектами. Когда блок фигурных скобок заканчивается, 
	* все объекты, что в нём были, уничтожаются.
	{
		PasswordManager ps;
		ps.readDataFromFile(file);
		ps.decryptData(mKey);

		* Пример того, как можно использовать ASSERT_EQUAL:
		* Левый аргумент - результат работы кода
		* Правый аргумент - то, что должно быть по идее.
		ASSERT_EQUAL(ps.numRecords(), 0);

		ps.addRecord("f", "l", "p");
		ps.addRecord("s", "l", "p");
	}

	{
		PasswordManager ps;
		ps.readDataFromFile(file);
		ps.decryptData(mKey);

		Record exp1{ "f", "l", "p" };
		Record exp2{ "s", "l", "p" };

		ASSERT_EQUAL(ps.getRecordByName(exp1.name), exp1);
		ASSERT_EQUAL(ps.getRecordByName(exp2.name), exp2);
	}
}

* Функция точечного тестирования кода
void testGetNames()
{
	PasswordManager ps;
	ps.readDataFromFile(file);
	ps.decryptData(mKey);

	auto names = ps.getNames();

	std::vector<std::string_view> exp{ "f", "s" };

	ASSERT_EQUAL(names, exp);
}

* Функция точечного тестирования кода
void testAddRecord()
{
	PasswordManager ps;
	ps.readDataFromFile(file);
	ps.decryptData(mKey);

	ps.addRecord("first", "log", "pass");
	ps.addRecord("second", "log", "pass");

	auto rec = ps.getRecordByName("first");
	Record exp{ "first", "log", "pass", "" };

	ASSERT_EQUAL(rec, exp);

	auto rec2 = ps.getRecordByNumber(0);
	Record exp2{ "f", "l", "p", "" };
	ASSERT_EQUAL(rec2, exp2);

	auto size = ps.numRecords();
	ps.addRecord("", "", "");
	ASSERT_EQUAL(ps.numRecords(), size);

	ps.addRecord("first", "log2", "pass2");
	ASSERT_EQUAL(ps.numRecords(), size);

	Record r3{ "third", "log", "pass", ""};
	Record r4{ "ssssssssss", "zzzz", "bbbb", "" };
	Record exp3 = r3;
	Record exp4 = r4;
	ps.addRecord(std::move(r3));
	ps.addRecord(std::move(r4));
	ASSERT_EQUAL(ps.getRecordByName("third"), exp3);
	ASSERT_EQUAL(ps.getRecordByName("ssssssssss"), exp4);

	Record r5{ "someText", "rrrr", "tttt", "ddd" };
	Record r6{ "AaAaAaAaAa", "asdfga", "ssss", "dddd" };
	ps.addRecord(r5);
	ps.addRecord(r6);
	ASSERT_EQUAL(ps.getRecordByName("someText"), r5);
	ASSERT_EQUAL(ps.getRecordByName("AaAaAaAaAa"), r6);
}

* Функция точечного тестирования кода
void testGetRecordBySome()
{
	PasswordManager ps;
	ps.readDataFromFile(file);
	ps.decryptData(mKey);

	{
		Record exp{ "AaAaAaAaAa", "asdfga", "ssss", "dddd" };
		auto rec = ps.getRecordByNumber(0);
		ASSERT_EQUAL(rec, exp);
	}

	{
		Record exp{ "third", "log", "pass", "" };
		auto rec = ps.getRecordByNumber(ps.numRecords() - 1);
		ASSERT_EQUAL(rec, exp);
	}
}

* Функция точечного тестирования кода
void testChangeRecord()
{
	PasswordManager ps;
	ps.readDataFromFile(file);
	ps.decryptData(mKey);

	{
		Record exp{ "test name","test login", "test pass", "test desription" };
		auto rec = ps.getRecordByNumber(1);
		ps.changeLoginRecord("test login");
		ps.changeDescriptionRecord("test desription");
		ps.changeNameRecord("test name");
		ps.changePasswordRecord("test pass");

		rec = ps.getRecordByName("test name");
		ASSERT_EQUAL(rec, exp);
	}

	{
		auto rec = ps.getRecordByName("test name");
		auto name = rec.name;
		ps.changeNameRecord("");
		ASSERT_EQUAL(ps.getRecordByName(name).name, name);
	}
}

* Функция точечного тестирования кода
void testWrongKey()
{
	{
		PasswordManager ps;
		ps.readDataFromFile(file);
		ASSERT_EQUAL(ps.decryptData("WrongKey"), 1);
		ASSERT_EQUAL(ps.decryptData("BestMasterKeyEvery"), 1);
		ASSERT_EQUAL(ps.decryptData("BestMasterKeuEveryTime"), 1);
		ASSERT_EQUAL(ps.decryptData(""), 1);
	}
	{
		PasswordManager ps;
		ps.readDataFromFile(file);
	}
	ASSERT(static_cast<bool>(std::filesystem::file_size(file)));
}

* Функция точечного тестирования кода
void testDelete()
{
	{
		PasswordManager ps;
		ps.readDataFromFile(file);
		ps.decryptData(mKey);

		{
			auto numRec = ps.numRecords();
			ps.deleteRecordByName("qwertyuioasdfghjk");
			ASSERT_EQUAL(ps.numRecords(), numRec)
		}

		{
			Record exp{};
			auto recName = ps.getRecordByNumber(1).name;
			ps.deleteRecordByNumber(1);
			Record res;
			try
			{
				res = ps.getRecordByName(recName);
			}
			catch (const std::exception&)
			{
				res = Record{};
			}
			ASSERT_EQUAL(res, exp);
		}

		{
			Record exp{};
			ps.deleteRecordByName("test name");
			Record res;
			try
			{
				res = ps.getRecordByName("test name");
			}
			catch (const std::exception&)
			{
				res = Record{};
			}
			ASSERT_EQUAL(res, exp);
		}

		ps.clearData();
		ASSERT_EQUAL(ps.numRecords(), 0);
	}
	ASSERT(!static_cast<bool>(std::filesystem::file_size(file)));
}

* Функция, которая прогоняет все тесты
void testAll()
{
	UnitTest::TestRunner tr;
	RUN_TEST(tr, testConstructAndDestruct);
	RUN_TEST(tr, testGetNames);
	RUN_TEST(tr, testAddRecord);
	RUN_TEST(tr, testGetRecordBySome);
	RUN_TEST(tr, testChangeRecord);
	RUN_TEST(tr, testWrongKey);
	RUN_TEST(tr, testDelete);
}

*/

void testAll()
{
	
}