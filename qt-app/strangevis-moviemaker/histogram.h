#pragma once
#include <QWidget>
#include <cstddef>
#include <memory>
#include <cstdlib>
#include <array>
#include <vector>
#include <chrono>

// Custom allocator compatible with std::vector.
// Memory is allocated using _aligned_malloc and released using _aligned_free.
// Template parameter A specifies allocated memory alignment.
template <class T, std::size_t A>
class aligned_allocator {
public:
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	template <class U>
	struct rebind {
		typedef aligned_allocator<U, A> other;
	};

	aligned_allocator() noexcept = default;
	aligned_allocator(const aligned_allocator&) noexcept = default;

	template <class U>
	explicit aligned_allocator(const aligned_allocator<U, A>&) noexcept {}

	~aligned_allocator() noexcept = default;

	pointer address(reference r) { return &r; }
	const_pointer address(const_reference r) const { return &r; }

	T* allocate(size_type n, const_pointer hint = nullptr);
	void deallocate(pointer p, size_type);

	void construct(pointer p, const_reference value) { new(p) value_type(value); }
	void destroy(pointer p) { p->~value_type(); }

	auto max_size() const noexcept { return size_type(-1) / sizeof(T); }

	auto operator==(const aligned_allocator&) { return true; }
	auto operator!=(const aligned_allocator&) { return false; }
};

template <class T, std::size_t A>
T* aligned_allocator<T, A>::allocate(size_type n, const_pointer hint) {
	if (auto res = static_cast<T*>(_aligned_malloc(n * sizeof(T), A))) { return res; }
	throw std::bad_alloc{};
}

template <class T, std::size_t A>
void aligned_allocator<T, A>::deallocate(pointer p, size_type) { _aligned_free(p); }


namespace custom_allocation {
	const std::size_t alignment_16{ 16u };
	const std::size_t alignment_32{ 32u };
	const std::size_t alignment_64{ 64u };
}

using histogram_entry_t = unsigned __int32;
using histogram_t = std::vector<histogram_entry_t, aligned_allocator<histogram_entry_t, custom_allocation::alignment_16>>;
using histogram_bundle_t = std::array<histogram_t, 4>;


class Histogram : public QWidget {

public:
	explicit Histogram(QWidget* parent = Q_NULLPTR);

	void plot(const histogram_t& histogram);

private:
	QtCharts::QChartView* chart;
};
