///
/// Contains basic geometrical definitions
///

#ifndef GAME_METRICS_H
#define GAME_METRICS_H

#include <cmath>
#include <string>
#include <stdexcept>

namespace Game {

    class GeometryError : public std::runtime_error {
    public:
        GeometryError(const std::string &message);
    };

    class ZeroVectorError : public GeometryError {
    public:
        ZeroVectorError();
        
        ZeroVectorError(const std::string &message);
    };

    ///
    /// \type models a two dimensional vector
    ///

    template<typename Scalar_> struct Vector2 {
        ///
        /// the type of the coordinate
        ///
        using Scalar = Scalar_;

        ///
        /// the coordinate along the x-axis
        ///
        Scalar x;

        ///
        /// the coordinate along the y-axis
        ///
        Scalar y;

        ///sssssssss
        /// Creates a vector with all coordinates set to zero
        ///

        Vector2() : x(), y() {
        };

        ///
        /// Creates a vector with the specified coordinates
        /// \param x_ the coordinate along the x-axis
        /// \param y_ the coordinate along the y-axis
        ///

        Vector2(Scalar x_, Scalar y_) : x(x_), y(y_) {
        };

        ///
        /// Calculates the square of the norm of this vector
        /// \return the square of the norm of this vector
        ///

        Scalar normSquared() const {
            return x * x + y*y;
        };

        ///
        /// Calculates the the norm of this vector
        /// \return the norm of this vector
        ///

        Scalar norm() const {
            return sqrt(normSquared());
        };

        ///
        /// Normalizes this vector
        /// \return a reference to the current vector
        ///

        Vector2<Scalar> normalize() {
            Scalar n = norm();
            if (n == 0) {
                throw ZeroVectorError{};
            } else {
                x /= n;
                y /= n;
                return *this;
            }
        };

        bool operator==(const Vector2<Scalar> &v) const {
            return x == v.x && y == v.y;
        };

        bool operator!=(const Vector2<Scalar> &v) const {
            return x != v.x || y != v.y;
        };

        Vector2<Scalar> &operator+=(const Vector2<Scalar> &v) {
            x += v.x;
            y += v.y;
            return *this;
        };

        Vector2<Scalar> &operator-=(const Vector2<Scalar> &v) {
            x -= v.x;
            y -= v.y;
            return *this;
        };

        Vector2<Scalar> &operator*=(Scalar s) {
            x *= s;
            y *= s;
            return *this;
        };

        Vector2<Scalar> &operator/=(Scalar s) {
            x /= s;
            y /= s;
            return *this;
        };

        ///
        /// \return false if the vector is a null vector, true otherwise
        ///

        operator bool() const {
            return x == 0 && y == 0;
        };

        ///
        /// \return true if the vector is a null vector, false otherwise
        ///

        bool operator!() const {
            return x == 0 && y == 0;
        };
    };

    template<typename Scalar> Vector2<Scalar> operator+(const Vector2<Scalar> &first, const Vector2<Scalar> &second) {
        return Vector2<Scalar>{first.x + second.x, first.y + second.y};
    };

    template<typename Scalar> Vector2<Scalar> operator-(const Vector2<Scalar> &first, const Vector2<Scalar> &second) {
        return Vector2<Scalar>{first.x - second.x, first.y - second.y};
    };

    template<typename Scalar> Vector2<Scalar> operator*(const Vector2<Scalar> &first, Scalar factor) {
        return Vector2<Scalar>{first.x*factor, first.y * factor};
    };

    template<typename Scalar> Vector2<Scalar> operator*(Scalar factor, const Vector2<Scalar> &first) {
        return Vector2<Scalar>{first.x*factor, first.y * factor};
    };

    template<typename Scalar> Vector2<Scalar> operator/(const Vector2<Scalar> &first, Scalar factor) {
        return Vector2<Scalar>{first.x*factor, first.y * factor};
    };

    ///
    /// Performs a dot operation (internal or scalar product) on two vectors
    /// \param first the first vector
    /// \param second the second vector
    /// \return the scalar result
    ///

    template<typename Scalar> Scalar dot(const Vector2<Scalar> &first, const Vector2<Scalar> &second) {
        return first.x * second.x + first.y * second.y;
    };

    ///
    /// Creates a normalized version of the vector
    /// \param the vector;
    ///

    template<typename Scalar> Vector2<Scalar> normalize(const Vector2<Scalar> &vector) {
        Scalar norm = vector.norm();
        if (norm == 0) {
            throw ZeroVectorError{};
        } else {
            return Vector2<Scalar>{vector.x / norm, vector.y / norm};
        }
    };

    ///
    /// Makes the compiler calculate the mathematical constant of pi.
    /// \return 3.14...
    ///

    constexpr inline double pi() {
        return std::asin(1.0) * 2;
    };

}

#endif
