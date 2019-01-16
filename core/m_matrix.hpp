#pragma once

#include <vector>
#include <array>
#include <typeinfo>
#include <utility>
#include <type_traits>

// changed to 0 or 1 depending on use of 1-indexed arrays
#define index_offset 1
// in case someone doesn't want to write mango every time, allow mg_matrix
#define mg_matrix mango_matrix

using std::vector;
using std::common_type;

template <typename T>
class mango_matrix
{
public:
	// TODO: Reinclude the static_assert of arithmetic types somewhere
	//mango_matrix(); // TODO: decide on a default empty constructor
	mango_matrix(std::initializer_list<T> l, bool is_column_vector);
	/*
	mango_matrix(T one_dimensional_array[], size_t length, bool is_column_vector);
	template <typename N>
	mango_matrix(std::array<T,N> one_dimensional_array, bool is_column_vector);
	*/
	mango_matrix(size_t num_columns, size_t num_rows, T value);
	/*
	mango_matrix(T array[][], size_t num_columns, size_t num_rows);
	mango_matrix(vector<vector<T>>, size_t num_columns, size_t num_rows);
	mango_matrix(vector<vector<T>>);
	*/
	// TODO: Add `=` operator with type conversion
	mango_matrix<T>& operator= (const mango_matrix<T>& other);
	mango_matrix<T>& operator= (const mango_matrix<T>&& other) noexcept;

	size_t m() const;
	size_t n() const;
	std::pair<size_t, size_t> size();

	const T& operator()(size_t row, size_t column) const;
	T& operator()(size_t row, size_t column);

private:
	// keeps track of what type of matrix is being used
	enum m_type { two_d_matrix, column_vector, row_vector } matrix_type;

	vector<vector<T>> two_d; // used for mathematical 2D vectors
	vector<T> one_d; // used for mathematical vectors (aka 1D matrices)
};

template <typename T1, typename T2>
mango_matrix<common_type<T1, T2>> operator+ (const mango_matrix<T1> A,
	const mango_matrix<T2> B);

template <typename T1, typename T2>
mango_matrix<common_type<T1, T2>> operator- (const mango_matrix<T1> A,
	const mango_matrix<T2> B);

template <typename T1, typename T2>
mango_matrix<common_type<T1, T2>> operator* (const mango_matrix<T1> A,
	const mango_matrix<T2> B);
