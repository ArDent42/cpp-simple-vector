#include <iostream>
#include <numeric>
#include "simple_vector.h"
#include "tests.h"

using namespace std;

void TestReserveConstructor() {
	cout << "TestReserveConstructor"s << endl;
	SimpleVector<int> v(Reserve(5));
	assert(v.GetCapacity() == 5);
	assert(v.IsEmpty());
	cout << "Done!"s << endl;
}

void TestReserveMethod() {
	cout << "TestReserveMethod"s << endl;
	SimpleVector<int> v;
	// зарезервируем 5 мест в векторе
	v.Reserve(5);
	assert(v.GetCapacity() == 5);
	assert(v.IsEmpty());

	// попытаемся уменьшить capacity до 1
	v.Reserve(1);
	// capacity должно остаться прежним
	assert(v.GetCapacity() == 5);
	// поместим 10 элементов в вектор
	for (int i = 0; i < 10; ++i) {
		v.PushBack(i);
	}
	assert(v.GetSize() == 10);
	// увеличим capacity до 100
	v.Reserve(100);
	// проверим, что размер не поменялся
	assert(v.GetSize() == 10);
	assert(v.GetCapacity() == 100);
	// проверим, что элементы на месте
	for (int i = 0; i < 10; ++i) {
		assert(v[i] == i);
	}
	cout << "Done!"s << endl;
}

class X {
public:
	X()
	:
			X(5) {
	}
	X(size_t num)
	:
			x_(num) {
	}
	X(const X &other) = delete;
	X& operator=(const X &other) = delete;
	X(X &&other) {
		x_ = exchange(other.x_, 0);
	}
	X& operator=(X &&other) {
		x_ = exchange(other.x_, 0);
		return *this;
	}
	size_t GetX() const {
		return x_;
	}

private:
	size_t x_;
};

SimpleVector<int> GenerateVector(size_t size) {
	SimpleVector<int> v(size);
	iota(v.begin(), v.end(), 1);
	return v;
}

void TestTemporaryObjConstructor() {
	const size_t size = 1000000;
	cout << "Test with temporary object, copy elision" << endl;
	SimpleVector<int> moved_vector(GenerateVector(size));
	assert(moved_vector.GetSize() == size);
	cout << "Done!" << endl << endl;
}

void TestTemporaryObjOperator() {
	const size_t size = 1000000;
	cout << "Test with temporary object, operator=" << endl;
	SimpleVector<int> moved_vector;
	assert(moved_vector.GetSize() == 0);
	moved_vector = GenerateVector(size);
	assert(moved_vector.GetSize() == size);
	cout << "Done!" << endl << endl;
}

void TestNamedMoveConstructor() {
	const size_t size = 1000000;
	cout << "Test with named object, move constructor" << endl;
	SimpleVector<int> vector_to_move(GenerateVector(size));
	assert(vector_to_move.GetSize() == size);
	cout << "ok" << endl;
	SimpleVector<int> moved_vector(move(vector_to_move));
	assert(moved_vector.GetSize() == size);
	assert(vector_to_move.GetSize() == 0);
	cout << "Done!" << endl << endl;
}

void TestNamedMoveOperator() {
	const size_t size = 1000000;
	cout << "Test with named object, operator=" << endl;
	SimpleVector<int> vector_to_move(GenerateVector(size));
	assert(vector_to_move.GetSize() == size);

	SimpleVector<int> moved_vector = move(vector_to_move);
	assert(moved_vector.GetSize() == size);
	assert(vector_to_move.GetSize() == 0);
	cout << "Done!" << endl << endl;
}

void TestNoncopiableMoveConstructor() {
	const size_t size = 5;
	cout << "Test noncopiable object, move constructor" << endl;
	SimpleVector<X> vector_to_move;
	for (size_t i = 0; i < size; ++i) {
		vector_to_move.PushBack(X(i));
	}

	SimpleVector<X> moved_vector = move(vector_to_move);
	assert(moved_vector.GetSize() == size);
	assert(vector_to_move.GetSize() == 0);

	for (size_t i = 0; i < size; ++i) {
		assert(moved_vector[i].GetX() == i);
	}
	cout << "Done!" << endl << endl;
}

void TestNoncopiablePushBack() {
	const size_t size = 5;
	cout << "Test noncopiable push back" << endl;
	SimpleVector<X> v;
	for (size_t i = 0; i < size; ++i) {
		v.PushBack(X(i));
	}

	assert(v.GetSize() == size);

	for (size_t i = 0; i < size; ++i) {
		assert(v[i].GetX() == i);
	}
	cout << "Done!" << endl << endl;
}

void TestNoncopiableInsert() {
	const size_t size = 5;
	cout << "Test noncopiable insert" << endl;
	SimpleVector<X> v;
	for (size_t i = 0; i < size; ++i) {
		v.PushBack(X(i));
	}

	// в начало
	v.Insert(v.begin(), X(size + 1));
	assert(v.GetSize() == size + 1);
	assert(v.begin()->GetX() == size + 1);
	// в конец
	v.Insert(v.end(), X(size + 2));
	assert(v.GetSize() == size + 2);
	assert((v.end() - 1)->GetX() == size + 2);
	// в середину
	v.Insert(v.begin() + 3, X(size + 3));
	assert(v.GetSize() == size + 3);
	assert((v.begin() + 3)->GetX() == size + 3);
	cout << "Done!" << endl << endl;
}

void TestNoncopiableErase() {
	const size_t size = 3;
	cout << "Test noncopiable erase" << endl;
	SimpleVector<X> v;
	for (size_t i = 0; i < size; ++i) {
		v.PushBack(X(i));
	}

	auto it = v.Erase(v.begin());
	assert(it->GetX() == 1);
	cout << "Done!" << endl << endl;
}

int main() {
	Test1();
	Test2();
	TestReserveConstructor();
	TestReserveMethod();
	TestTemporaryObjConstructor();
	TestTemporaryObjOperator();
	TestNamedMoveConstructor();
	TestNamedMoveOperator();
	TestNoncopiableMoveConstructor();
	TestNoncopiablePushBack();
	TestNoncopiableInsert();
	TestNoncopiableErase();
	cout << "All Ok" << endl;
	return 0;
}
