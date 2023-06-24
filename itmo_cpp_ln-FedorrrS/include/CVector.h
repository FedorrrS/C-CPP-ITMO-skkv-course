#pragma once
#include <cstring>
#include <memory>

template< typename T >
class CVector
{
  public:
	CVector() : size_(), capacity_(), data_() {}
	CVector(const CVector< T >& cv) : CVector()
	{
		CVector< T > tmp(cv.size_);
		memcpy(tmp.data_.get(), cv.data_.get(), cv.size_ * sizeof(T));
		tmp.size_ = cv.size_;
		swap(tmp);
	}
	CVector(CVector< T >&& cv) noexcept : CVector() { swap(cv); }
	explicit CVector(size_t sz) : size_(sz), capacity_(sz), data_(std::make_unique< T[] >(capacity_)) {}
	CVector< T >& operator=(const CVector< T >& cv)
	{
		if (&cv != this)
		{
			CVector< T > tmp(cv);
			swap(tmp);
		}
		return *this;
	}
	CVector< T >& operator=(CVector< T >&& cv) noexcept
	{
		if (&cv != this)
		{
			swap(cv);
			cv.size_ = 0;
			cv.capacity_ = 0;
			cv.data_ = nullptr;
		}
		return *this;
	}
	const T& operator[](size_t i) const { return data_[i]; }
	T& operator[](size_t i) { return data_[i]; }

	void push(const T& e)
	{
		resize();
		data_[size_++] = e;
	}
	void pop()
	{
		resize();
		size_--;
	}

	const T& back() const { return data_[size_ - 1]; }
	T& back() { return data_[size_ - 1]; }
	[[nodiscard]] bool empty() const { return size_ == 0; }
	[[nodiscard]] size_t get_size() const { return size_; }
	void remove_trailing_zeroes()
	{
		while (size_ > 0 && data_[size_ - 1] == 0)
		{
			resize();
			size_--;
		}
	}

  private:
	size_t size_{};
	size_t capacity_{};
	std::unique_ptr< T[] > data_;

	void resize()
	{
		if (size_ >= capacity_)
		{
			capacity_ = (capacity_ == 0) ? 1 : capacity_ * 2;
			std::unique_ptr< T[] > new_data = std::make_unique< T[] >(capacity_);
			memcpy(new_data.get(), data_.get(), size_ * sizeof(T));
			data_ = std::move(new_data);
		}
	}

	void swap(CVector< T >& cv) noexcept
	{
		std::swap(size_, cv.size_);
		std::swap(capacity_, cv.capacity_);
		std::swap(data_, cv.data_);
	}
};