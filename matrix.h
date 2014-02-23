#ifndef MATRIX_H
#define MATRIX_H

#include <QVector>

class Matrix;

class Vector
{
public:
    /* Constructor */
    Vector(int n = 0):n(n), values(n) {}

    /* Operators */
    double &operator[](int index) { return values[index]; }
    Vector operator+(Vector b);
    Vector operator-(Vector b);
    Vector operator*(double b);
    Vector operator=(QVector<double> b);
    double operator*(Vector b); // row by column
    Matrix operator()(Vector b); // column by row

    operator QVector<double>() { return values; }

    /* Methods */
    int dim() { return n; }
    void zero();
    double norm();
    void print();
private:
    int n;
    QVector<double> values;
};

class Matrix
{
public:
    /* Constructor */
    Matrix(int n = 0):n(n), values(n, QVector<double>(n)) {}

    /* Operators */
    QVector<double> &operator[](int index) { return values[index]; }
    Matrix operator+(Matrix b);
    Matrix operator-(Matrix b);
    Vector operator*(Vector b);
    Matrix operator*(double b);
    Matrix operator*(Matrix b);

    Matrix operator=(Matrix b) { values = b.values; n = b.n; return *this; }

    /* Methods */
    int dim() { return n; }
    Matrix trans();
    double norm();
    void QR(Matrix *Q, Matrix *R);
    Vector reflection(Vector b);
    void print();
private:
    int n;
    QVector<QVector<double> > values;
};

#endif // MATRIX_H
