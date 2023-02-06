#pragma once

#include <cassert>
#include <iostream>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <iterator>
#include "array_ptr.h"

class ReserveProxyObj {
public:
	explicit ReserveProxyObj(size_t capacity_to_reserve)
	:
			capacity_(capacity_to_reserve)
	{
	}
	size_t capacity_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
	return ReserveProxyObj(capacity_to_reserve);
}

template<typename Type>
class SimpleVector {
private:
	ArrayPtr<Type> array_;
	size_t size_ = 0;
	size_t capacity_ = 0;

public:

	using Iterator = Type*;
	using ConstIterator = const Type*;

	SimpleVector() noexcept = default;

	explicit SimpleVector(size_t size) :
			array_(size), size_(size), capacity_(size) {
		std::fill(begin(), end(), Type { });
	}

	SimpleVector(size_t size, const Type &value) :
			array_(size), size_(size), capacity_(size) {
		std::fill(begin(), end(), value);
	}

	SimpleVector(std::initializer_list<Type> init) :
			array_(init.size()), size_(init.size()), capacity_(init.size()) {
		for (size_t i = 0; i < init.size(); ++i) {
			array_[i] = data(init)[i];
		}
	}
	SimpleVector(const SimpleVector &other) :
			array_(other.capacity_), size_(other.size_), capacity_(other.capacity_) {
		std::copy(other.begin(), other.end(), begin());
	}

	SimpleVector(SimpleVector &&other) {
		std::swap(size_, other.size_);
		std::swap(capacity_, other.capacity_);
		array_.swap(other.array_);
	}

	SimpleVector(ReserveProxyObj capacity_to_reserve) {
		Reserve(capacity_to_reserve.capacity_);
	}
	SimpleVector& operator=(const SimpleVector &rhs) {
		if (this == &rhs) {
			return *this;
		}
		SimpleVector<Type> temp(rhs);
		swap(temp);
		return *this;
	}

	SimpleVector& operator=(SimpleVector &&rhs) {
		if (this == &rhs) {
			return *this;
		}
		SimpleVector<Type> temp(std::move(rhs));
		swap(temp);
		return *this;
	}

	void PushBack(const Type &item) {
		if (size_ == capacity_) {
			if (capacity_ == 0) {
				Reserve(1);
			} else {
				Reserve(capacity_ * 2);
			}
			array_[size_++] = item;
		} else {
			array_[size_++] = item;
		}
	}

	void PushBack(Type &&item) {
		if (size_ == capacity_) {
			if (capacity_ == 0) {
				Reserve(1);
			} else {
				Reserve(capacity_ * 2);
			}
			array_[size_++] = std::move(item);
		} else {
			array_[size_++] = std::move(item);
		}
	}
// Вставляет значение value в позицию pos.
// Возвращает итератор на вставленное значение
// Если перед вставкой значения вектор был заполнен полностью,
// вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
	Iterator Insert(ConstIterator pos, Type &&value) {
		if (IsEmpty()) {
			PushBack(std::move(value));
			return begin();
		}
		size_t index = std::distance(cbegin(), pos);
		if (size_ == capacity_) {
			Reserve(capacity_ * 2);
		}
		std::move_backward(begin() + index, end(), end() + 1);
		array_[index] = std::move(value);
		++size_;
		return &array_[index];
	}

	Iterator Insert(ConstIterator pos, const Type &value) {
		if (IsEmpty()) {
			PushBack(value);
			return begin();
		}
		size_t index = std::distance(cbegin(), pos);
		if (size_ == capacity_) {
			Reserve(capacity_ * 2);
		}
		std::copy_backward(begin() + index, end(), end() + 1);
		array_[index] = value;
		++size_;
		return &array_[index];
	}

// "Удаляет" последний элемент вектора. Вектор не должен быть пустым
	void PopBack() noexcept {
		if (size_ != 0) {
			--size_;
		}
	}

// Удаляет элемент вектора в указанной позиции
	Iterator Erase(ConstIterator pos) {
		size_t index = std::distance(cbegin(), pos);
		std::copy(pos + 1, cend(), begin() + index);
		--size_;
		return &array_[index];
	}

	Iterator Erase(Iterator &&pos) {
		size_t index = std::distance(begin(), pos);
		std::move(pos + 1, end(), begin() + index);
		--size_;
		return &array_[index];
	}

// Обменивает значение с другим вектором
	void swap(SimpleVector &other) noexcept {
		std::swap(size_, other.size_);
		std::swap(capacity_, other.capacity_);
		array_.swap(other.array_);
	}
	size_t GetSize() const noexcept {
		return size_;
	}

// Возвращает вместимость массива
	size_t GetCapacity() const noexcept {
		// Напишите тело самостоятельно
		return capacity_;
	}

// Сообщает, пустой ли массив
	bool IsEmpty() const noexcept {
		return size_ == 0;
	}

// Возвращает ссылку на элемент с индексом index
	Type& operator[](size_t index) noexcept {
		return array_[index];
	}

// Возвращает константную ссылку на элемент с индексом index
	const Type& operator[](size_t index) const noexcept {
		return array_[index];
	}

// Возвращает константную ссылку на элемент с индексом index
// Выбрасывает исключение std::out_of_range, если index >= size
	Type& At(size_t index) {
		if (index >= size_) {
			throw std::out_of_range("");
		}
		return array_[index];
	}

// Возвращает константную ссылку на элемент с индексом index
// Выбрасывает исключение std::out_of_range, если index >= size
	const Type& At(size_t index) const {
		if (index >= size_) {
			throw std::out_of_range("");
		}
		return array_[index];
	}

// Обнуляет размер массива, не изменяя его вместимость
	void Clear() noexcept {
		size_ = 0;
	}

// Изменяет размер массива.
// При увеличении размера новые элементы получают значение по умолчанию для типа Type
	void Resize(size_t new_size) {
		if (new_size <= size_) {
			size_ = new_size;
		} else if (new_size <= capacity_) {
			std::generate(begin() + size_, begin() + new_size, []() {return Type{};});
			size_ = new_size;
		} else {
			size_t size_max = std::max(new_size, capacity_ * 2);
			Reserve(std::move(size_max));
			size_ = new_size;
		}
	}

	void Reserve(const size_t &new_capacity) {
		if (new_capacity > capacity_) {
			ArrayPtr<Type> new_array(new_capacity);
			std::copy(begin(), end(), new_array.Get());
			array_.swap(new_array);
            std::fill(end(), begin() + new_capacity, Type{});
			capacity_ = new_capacity;
		}
	}

	void Reserve(size_t &&new_capacity) {
		if (new_capacity > capacity_) {
			ArrayPtr<Type> new_array(new_capacity);
			std::move(begin(), end(), new_array.Get());
			array_.swap(new_array);
			std::generate(end(), begin() + new_capacity, []() {return Type{};});
			capacity_ = new_capacity;
		}
	}

// Возвращает итератор на начало массива
// Для пустого массива может быть равен (или не равен) nullptr
	Iterator begin() noexcept {
		return array_.Get();
	}

// Возвращает итератор на элемент, следующий за последним
// Для пустого массива может быть равен (или не равен) nullptr
	Iterator end() noexcept {
		return array_.Get() + size_;
	}

// Возвращает константный итератор на начало массива
// Для пустого массива может быть равен (или не равен) nullptr
	Iterator begin() const noexcept {
		return array_.Get();
	}

// Возвращает итератор на элемент, следующий за последним
// Для пустого массива может быть равен (или не равен) nullptr
	Iterator end() const noexcept {
		return array_.Get() + size_;
	}

// Возвращает константный итератор на начало массива
// Для пустого массива может быть равен (или не равен) nullptr
	ConstIterator cbegin() const noexcept {
		return array_.Get();
	}

// Возвращает итератор на элемент, следующий за последним
// Для пустого массива может быть равен (или не равен) nullptr
	ConstIterator cend() const noexcept {
		return array_.Get() + size_;
	}
};

template<typename Type>
inline bool operator==(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
	if (lhs.GetSize() == rhs.GetSize()) {
		return std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}
	return false;
}

template<typename Type>
inline bool operator!=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
	return !(lhs == rhs);
}

template<typename Type>
inline bool operator<(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename Type>
inline bool operator<=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
	return !(rhs < lhs);
}

template<typename Type>
inline bool operator>(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
	return !(lhs <= rhs);
}

template<typename Type>
inline bool operator>=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
	return !(lhs < rhs);
}

