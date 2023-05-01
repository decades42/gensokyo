#pragma once

#include <array>

namespace gensokyo
{
    namespace impl
    {
        template <std::size_t rows, std::size_t cols>
        class Matrix
        {
          public:
            Matrix()
             : data_()
            {
                static_assert(rows > 0 && cols > 0, "a matrix is at least 1x1");
            }

            Matrix(const std::array<float, rows * cols>& data)
             : data_(data)
            {
                static_assert(rows > 0 && cols > 0, "a matrix is at least 1x1");
            }

            std::array<float, rows * cols>& operator[](std::size_t row)
            {
                return *reinterpret_cast<std::array<float, cols>*>(&data_[row * cols]);
            }

            const std::array<float, rows * cols>& operator[](std::size_t row) const
            {
                return *reinterpret_cast<const std::array<float, cols>*>(&data_[row * cols]);
            }

            float& at(std::size_t row, std::size_t col)
            {
                return data_[row * cols + col];
            }

            const float& at(std::size_t row, std::size_t col) const
            {
                return data_[row * cols + col];
            }

            fVector3 at(std::size_t col) const
            {
                return { at(0, col), at(1, col), at(2, col) };
            }

            float* base()
            {
                return &data_[0];
            }

            const float* base() const
            {
                return &data_[0];
            }

          private:
            std::array<float, rows * cols> data_;
        };
    }

    using Matrix3x4 = impl::Matrix<3, 4>;
    using Matrix4x4 = impl::Matrix<4, 4>;
}
