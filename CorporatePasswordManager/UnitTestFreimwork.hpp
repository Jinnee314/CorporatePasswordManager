#pragma once
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <sstream>

// INTERFACE
// Overloading the output operator for vectors
// Перегрузка оператора вывода для векторов
template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec);

// Overloading the output operator for sets
// Перегрузка оператора вывода для множеств
template<typename T>
std::ostream& operator<<(std::ostream& out, const std::set<T>& s);

// Overloading the output operator for maps
// Перегрузка оператора вывода для словарей
template<typename K, typename V>
std::ostream& operator<<(std::ostream& out, const std::map<K, V>& m);

namespace UnitTest
{
	// Функция сравнения двух произвольных объектов на равенство. По идее, равенство достигается, когда равны типы объектов и их наполнение
	// Данная функция отвечает за проверку результатов работы кода. Идея проста: в одном аргументе передаётся результат выполнения кода, 
	// в другом - тот результат, которые должен быть, сформированный вручную (или как-либо ещё). 
	// Если они не совпали, эта функция отреагирует и выдаст запись, где укажет, что чему не равно и строку hint - метку, 
	// чтобы можно было понять, какой конкретно тест не прошёл.
	template<typename T, typename U>
	void AssertEqual(const T& t, const U& u, const std::string& hint = {});
	
	// Функция проверки буллевой переменной на истиность. Можно использовать для проверки существования объекта/пути и т.п. Идея та же,
	// что и выше - помещаем результат кода, которые должен вернуть нам true, в данную функцию, и метку, по которой позже сможем распознать
	// конкретный тест.
	void Assert(bool b, const std::string& hint = {});

	// Класс для запуска тестов и обработки их результатов
	class TestRunner
	{
	private:
		uint32_t failCount = 0;
	public:

		~TestRunner();

		// Метод, которые запускает функцию тестирования, которая передана в func, и обрабатывает её результаты
		template<class TestFunc>
		void RunTest(TestFunc func, const std::string& test_name);
	};
}

//IMPLEMENTATION

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
{
	out << "[";

	bool first = true;
	for (const auto& elem : vec)
	{
		if (!first)
		{
			out << ", ";
		}
		first = false;
		out << elem;
	}
	out << "]";

	return out;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::set<T>& s)
{
	out << "{";

	bool first = true;
	for (const auto& elem : s)
	{
		if (!first)
		{
			out << ", ";
		}
		first = false;
		out << elem;
	}
	out << "}";

	return out;
}

template<typename K, typename V>
std::ostream& operator <<(std::ostream& out, const std::map<K, V>& m)
{
	out << "{";

	bool first = true;
	for (const auto& [key, value] : m)
	{
		if (!first)
		{
			out << ", ";
		}
		first = false;
		out << key << ": " << value;
	}
	out << "}";

	return out;
}


template<typename T, typename U>
void UnitTest::AssertEqual(const T& t, const U& u, const std::string& hint)
{
	if (t != u)
	{
		std::ostringstream os;
		os << "Assertion failed:\n" << t << " != " << u << ";\nHint " << hint << "\n";
		throw std::runtime_error(os.str());
	}
}

template<class TestFunc>
void UnitTest::TestRunner::RunTest(TestFunc func, const std::string& test_name) {
	try {
		func();
		std::cerr << test_name << " OK" << std::endl;
	}
	catch (std::exception& e) {
		++failCount;
		std::cerr << test_name << " fail: " << e.what() << std::endl;
	}
	catch (...) {
		++failCount;
		std::cerr << "Unknown exception caught" << std::endl;
	}
}

// Опеределение алиасов? Не помню, как это называется, но вообще это сделано, 
// чтобы постоянно самому не выдумывать запись hint для функций AssertEqual и Assert и не писать огромные вызовы через namespace .
// Данный алиас(вроде так) формирует строку, в которой указывает файл и строчку в нём, где находится конкретные вызов данного алиаса.
// Помогает сразу понять, какой тест словил ошибку.
#define ASSERT_EQUAL(x, y)						\
	{											\
		std::ostringstream testOut;					\
		testOut << #x << " != " << #y				\
		<< ", " << __FILE__ << ":"				\
		<< __LINE__;							\
		UnitTest::AssertEqual(x,y, testOut.str());	\
	}

#define ASSERT(x)								\
	{											\
		std::ostringstream testOut;					\
		testOut << #x << " is false"				\
		<< ", " << __FILE__ << ":"				\
		<< __LINE__;							\
		UnitTest::Assert(x, testOut.str());		\
	}

// Этот, если не ошибаюсь, просто для компании и удобства сделано.
#define RUN_TEST(tr, func) tr.RunTest(func, #func);