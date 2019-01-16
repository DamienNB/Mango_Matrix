// Copyright Damien Bobrek 2018-2019
// damienbobrek@gmail.com

#include "m_matrix.hpp"
#include <vector>
#include <array>
#include <typeinfo>
#include <utility>
#include <type_traits>
#include <memory>
#include <initializer_list>
#include <cassert>
#include <numeric>
#include <iostream>
#include <stdexcept>

using std::vector;
using std::common_type;

int main()
{
	mg_matrix<int> test({ 24, 23, 1, 0, 12 }, true);

	std::cout << test.size().first << ", " << test.size().second << std::endl;

	std::cout << "(1,1): " << test(1, 1) << " (4,1): " << test(4, 1) << std::endl;

	mg_matrix<int> test2(5, 5, 1);

	std::cout << test.size().first << ", " << test.size().second << std::endl;

	std::cout << "(1,1): " << test2(1, 1) << " (4,1): " << test2(4, 1)
		<< "(1,4): " << test2(1, 4) << " (5,5): " << test2(5, 5) << std::endl;
	/*
	mg_matrix<int> r = test2 * test;

	std::cout << "(1,1): " << r(1,1) << " (4,1): " << r(4,1)
			  << "(1,4): " << r(1,4) << " (5,5): " << r(5,5) << std::endl;
	*/
	while (1);
}
/*
template <typename T>
mango_matrix<T>::mango_matrix()
{
  static_assert(std::is_arithmetic<T>(),
				"mango_matrix requires arithmetic types");
}
*/
template <typename T>
mango_matrix<T>::mango_matrix(std::initializer_list<T> l,
	bool is_column_vector)
{
	if (is_column_vector)
		matrix_type = m_type::column_vector;
	else
		matrix_type = m_type::row_vector;

	one_d.assign(l);
}
/*
template <typename T>
mango_matrix<T>::mango_matrix(T one_dimensional_array[], size_t length,
							  bool is_column_vector)
{
  size_t &array[length] one_d_array = one_dimensional_array;
}
*/

template <typename T>
mango_matrix<T>::mango_matrix(size_t num_columns, size_t num_rows, T value)
{
	if (num_columns == 1)
	{
		matrix_type = m_type::column_vector;
		one_d.assign(num_columns, value);
	}
	else if (num_rows == 1)
	{
		matrix_type = m_type::row_vector;
		one_d.assign(num_rows, value);
	}
	else
	{
		matrix_type = m_type::two_d_matrix;
		two_d.assign(num_rows, vector<T>(num_columns, value,
			std::allocator<T>()));
	}
}

template<typename T>
mango_matrix<T>& mango_matrix<T>::operator= (const mango_matrix<T>& other)
{
	if (this != &other)
	{ // self-assignment check expected
		if (other.size != size)
		{         // storage cannot be reused
			delete;              // destroy storage in this
			size = 0;
			mArray = nullptr;             // preserve invariants in case next line throws
			mArray = new int[other.size]; // create storage in this
			size = other.size;
		}
		std::copy(other.mArray, other.mArray + other.size, mArray);
	}
	return *this;
}

template <typename T>
size_t mango_matrix<T>::m() const
{
	switch (matrix_type)
	{
	case m_type::two_d_matrix:
		return two_d.size();

	case m_type::column_vector:
		return one_d.size();

	case m_type::row_vector:
		return 1;

	default:
		throw std::logic_error("unaccounted-for case in getting m-dimension");
		break;
	}
}

template <typename T>
size_t mango_matrix<T>::n() const
{
	switch (matrix_type)
	{
	case m_type::two_d_matrix:
		if (!two_d.empty()) // verify the matrix isn't empty
			return two_d[0].size();
		else
			return 0;

	case m_type::column_vector:
		return 1;

	case m_type::row_vector:
		return one_d.size();

	default:
		throw std::logic_error("unaccounted-for case in getting n-dimension");
		break;
	}
}

template <typename T>
std::pair<size_t, size_t> mango_matrix<T>::size()
{
	return std::pair<size_t, size_t>(this->m(), this->n());
}

template <typename T>
const T& mango_matrix<T>::operator()(size_t row, size_t column) const
{
	switch (matrix_type)
	{
	case m_type::two_d_matrix:
		assert((row >= index_offset) &&
			(row < (this->m() + index_offset)));
		assert((column >= index_offset) &&
			(column < (this->n() + index_offset)));

		return two_d[row - index_offset][column - index_offset];

	case m_type::column_vector:
		assert(column == 0 + index_offset);
		assert((row >= index_offset) &&
			(row < (this->m() + index_offset)));

		return one_d[row - index_offset];

	case m_type::row_vector:
		assert((column >= index_offset) &&
			(column < (this->n() + index_offset)));
		assert(row == 0 + index_offset);

		return one_d[column - index_offset];

	default: // should never reach default (all cases should've been handled)
		throw std::logic_error("unaccounted-for case in getting matrix element");
		break;
	}
}

template <typename T>
T& mango_matrix<T>::operator()(size_t row, size_t column)
{
	return const_cast<T&>(const_cast<mango_matrix<T> const&>(*this)(row, column));
}

template <typename result_type, typename T1, typename T2>
mango_matrix<common_type<T1, T2>> operator+ (const mango_matrix<T1> A,
	const mango_matrix<T2> B)
{
	// TODO: Add support for zero matrices
	// For now, use of zero matrices will not be allowed
	assert((A.m() != 0) && (A.n() != 0));
	assert((B.m() != 0) && (B.n() != 0));

	// verify dimensions agree
	assert(A.m() == B.m());
	assert(A.n() == B.n());

	mango_matrix<common_type<T1, T2>> result(A.m(), B.n(), 0);

	for (size_t i = 0; i < result.m(); i++)
		for (size_t j = 0; j < result.n(); j++)
		{
			result(i, j) = A(i, j) + B(i, j); // TODO: Possibly check for overflow?
		}

	return result;
}

template <typename result_type, typename T1, typename T2>
mango_matrix<common_type<T1, T2>> operator- (const mango_matrix<T1> A,
	const mango_matrix<T2> B)
{
	// TODO: Add support for zero matrices
	// For now, use of zero matrices will not be allowed
	assert((A.m() != 0) && (A.n() != 0));
	assert((B.m() != 0) && (B.n() != 0));

	// verify dimensions agree
	assert(A.m() == B.m());
	assert(A.n() == B.n());

	mango_matrix<common_type<T1, T2>> result(A.m(), B.n(), 0);

	for (size_t i = 0; i < result.m(); i++)
		for (size_t j = 0; j < result.n(); j++)
		{
			result(i, j) = A(i, j) - B(i, j); // TODO: Possibly check for underflow?
		}

	return result;
}

// For now, naive matrix multiplication is used
template <typename T1, typename T2>
mango_matrix<common_type<T1, T2>> operator* (const mango_matrix<T1> A,
	const mango_matrix<T2> B)
{
	// TODO: Add support for zero matrices
	// For now, use of zero matrices will not be allowed
	assert((A.m() != 0) && (A.n() != 0));
	assert((B.m() != 0) && (B.n() != 0));

	assert(A.n() == B.m()); // verify inner-dimensions agree

	mango_matrix<common_type<T1, T2>> result(A.m(), B.n(), 0);

	for (size_t result_row = 0; result_row < result.m(); result_row++)
	{
		for (size_t result_column = 0; result_column < result.n(); result_column++)
		{
			common_type<T1, T2> val = 0;

			for (size_t middle_term = 0; middle_term < A.n(); middle_term++)
			{
				val += A(result_row, middle_term) * B(middle_term, result_column);
			}

			result(result_row, result_column) = val;
		}
	}

	return result;
}
