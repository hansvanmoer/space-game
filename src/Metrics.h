///
/// Contains basic geometrical definitions
///

#ifndef GAME_METRICS_H
#define GAME_METRICS_H

#include <cmath>
#include <string>
#include <stdexcept>
#include <array>

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

        ///
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
    
    template<typename Scalar_> class Transform2{
    public:
        
        using Scalar = Scalar_;
        
        using Coordinate = typename std::array<Scalar, 6>::size_type;
        
        Transform2(std::initializer_list<Scalar> values) : values_(values){};
        
        Transform2() : values_{Scalar{1}, Scalar{}, Scalar{}, Scalar{}, Scalar{1}, Scalar{}}{};

        static Transform2<Scalar> create_identity(){
            return Transform2<Scalar>{Scalar{1}, Scalar{}, Scalar{}, Scalar{}, Scalar{1}, Scalar{}};
        };
        
        static Transform2<Scalar> create_translation(Scalar dx, Scalar dy){
            return Transform2<Scalar>{Scalar{1}, Scalar{}, dx, Scalar{}, Scalar{1}, dy};
        };
        
        static Transform2<Scalar> create_scale(Scalar dx, Scalar dy){
            return Transform2<Scalar>{dx, Scalar{}, Scalar{}, Scalar{},  dy, Scalar{}};
        };
        
        static Transform2<Scalar> create_rotation(Scalar theta){
            return Transform2<Scalar>{std::cos(theta), std::sin(theta), std::cos(theta), -std::sin(theta),  Scalar{}, Scalar{}};
        };
        
        static Transform2<Scalar> create_rotation(Scalar px, Scalar py, Scalar theta){
            Transform2<Scalar> result{create_translation(-px,-py)};
            result*=create_rotation(theta);
            result*=create_translation(px,py);
            return result;
        };
        
        Vector2<Scalar> transform(const Vector2<Scalar> &vector) const{
            return Vector2<Scalar>{
                values_[0] * vector.x + values_[1] * vector.y + values_[2],
                values_[3] * vector.x + values_[4] * vector.y + values_[5]
            };
        };
        
        Transform2<Scalar> &concatenate(const Transform2<Scalar> &t){
            //(A + t) * (B + v) = (AB) + (B*t) + v
            
            std::array<Scalar,6> new_values;
            new_values[0] = values_[0]*t.values_[0] + values_[1] * t.values_[3];
            new_values[1] = values_[0]*t.values_[1] + values_[1] * t.values_[4];
            new_values[2] = t.values_[0]*values_[2] + t.values_[1]*values_[5] + t.values_[3];
            
            new_values[3] = values_[3]*t.values_[0] + values_[4] * t.values_[3];
            new_values[4] = values_[3]*t.values_[1] + values_[4] * t.values_[4];
            new_values[5] = t.values_[3]*values_[2] + t.values_[4]*values_[5] + t.values_[5];
            
            std::swap(values_, new_values);
            return *this;
        };
        
        Transform2<Scalar> &translate(Scalar dx, Scalar dy){
            return concatenate(create_translation(dx, dy));
        };
        
        Transform2<Scalar> &scale(Scalar sx, Scalar sy){
            return concatenate(create_scale(sx, sy));
        };
        
        Transform2<Scalar> &rotate(Scalar theta){
            return concatenate(create_rotation(theta));
        };
        
        Transform2<Scalar> &rotate(Scalar px, Scalar py, Scalar theta){
            return concatenate(create_rotation(px, py, theta));
        };
        
        Scalar operator[](Coordinate index) const{
            return values_[index];
        };
        
        Scalar &operator[](Coordinate index){
            return values_[index];
        };
        
        Scalar operator()(Coordinate row, Coordinate column) const{
            return values_[row*3+column];
        };
        
        Scalar &operator()(Coordinate row, Coordinate column){
            return values_[row*3+column];
        };
        
        Vector2<Scalar> operator()(const Vector2<Scalar> &vector) const{
            return transform(vector);
        };
        
        Scalar get(Coordinate index) const{
            if(index > 6){
                throw std::out_of_range{};
            }
            return values_[index];
        };
        
        Scalar get(Coordinate row, Coordinate column) const{
            if(row > 1 || column > 3){
                throw std::out_of_range{};
            }
            return values_[row*3+column];
        };
        
        void set(Coordinate index, Scalar value){
            if(index > 6){
                throw std::out_of_range{};
            }
            values_[index] = value;
        };
        
        void set(Coordinate row, Coordinate column, Scalar value){
            if(row > 1 || column > 3){
                throw std::out_of_range{};
            }
            values_[row*3+column] = value;
        };
        
        bool operator==(const Transform2<Scalar> &t) const{
            return values_ == t.values_;
        };
        
        bool operator!=(const Transform2<Scalar> &t) const{
            return values_ != t.values_;
        };
        
        Transform2 &operator*=(const Transform2<Scalar> &t){
            return concatenate(t);
        };
        

    private:
        std::array<Scalar, 6> values_;
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
    
    template<typename Scalar> Vector2<Scalar> operator*(const Transform2<Scalar> &transform, const Vector2<Scalar> &vector){
        return transform(vector);
    };
    
    template<typename Scalar> Transform2<Scalar> operator*(const Transform2<Scalar> &first, const Transform2<Scalar>&second){
        Transform2<Scalar> result{first};
        result*=second;
        return result;
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
