#include "matrix.h"

#include <math.h>
#include <assert.h>
#include <QDebug>

static int sign(double x)
{
    if (x > 0)
        return 1;
    if (x < 0)
        return -1;
    return 0;
}

/* MATRIX */

Matrix Matrix::operator -(Matrix b)
{
    Matrix c(n);

    assert(n == b.dim());

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            c[i][j] = values[i][j] - b[i][j];
    return c;
}

Matrix Matrix::operator +(Matrix b)
{
    Matrix c(n);

    assert(n == b.dim());

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            c[i][j] = values[i][j] + b[i][j];
    return c;
}

Matrix Matrix::operator *(Matrix b)
{
    Matrix c(n);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            c[i][j] = 0;

            for (int k = 0; k < n; k++)
            {
                c[i][j] += values[i][k] * b[k][j];
            }
        }
    }
    return c;
}

Matrix Matrix::operator *(double b)
{
    Matrix c(n);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            c[i][j] = values[i][j] * b;
        }
    }
    return c;
}

Vector Matrix::operator *(Vector b)
{
    Vector c(n);

    assert(n == b.dim());

    for (int i = 0; i < n; i++)
    {
        c[i] = 0;

        for (int j = 0; j < n; j++)
        {
            c[i] += values[i][j] * b[j];
        }
    }
    return c;
}

double Matrix::norm()
{
    double norm = 0;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            norm += values[i][j] * values[i][j];
        }
    }
    return sqrt(norm);
}

Matrix Matrix::trans()
{
    Matrix c(n);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            c[i][j] = values[j][i];
        }
    }
    return c;
}

void Matrix::print()
{
    for (int i = 0; i < n; ++i)
    {
        QDebug d = qDebug();
        for (int j = 0; j < n; ++j)
            d << values[i][j];
    }
}

void Matrix::QR(Matrix *Q, Matrix *R)
{
    Vector v(n), e(n), u(n);
    Matrix QT(n), Pi(n), E(n), A(n);
    A = *this;

    assert(n == Q->n);
    assert(n == R->n);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            E[i][j] = (i == j) ? 1 : 0;
        }
    }

    QT = E;

    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < n; i++)
        {
            v[i] = A[i][j];
        }
        u.zero();
        for (int i = 0; i < j; i++)
        {
            v[i] = 0;
        }
        for (int i = 0; i < n; i++)
        {
            e[i] = (i == j) ? 1 : 0;
        }
        u = v - e * v.norm() * (-1) * sign(v[j]);
        Pi = E - (u(u) * (2 / (u * u)));
        A = Pi * A;
        QT = Pi * QT;
    }
    *Q = QT.trans() * (-1);
    *R = A * (-1);
}

/* VECTOR */

Vector Vector::operator -(Vector b)
{
    Vector c(n);

    assert(n == b.dim());

    for (int i = 0; i < n; ++i)
        c[i] = values[i] - b[i];
    return c;
}

double Vector::operator *(Vector b)
{
    double c = 0;

    assert(n == b.dim());

    for (int i = 0; i < n; i++)
    {
        c += values[i] * b[i];
    }
    return c;
}

Matrix Vector::operator ()(Vector b)
{
    Matrix c(n);

    assert(n == b.dim());

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            c[i][j] = values[i] * b[j];
        }
    }
    return c;
}

Vector Vector::operator *(double b)
{
    Vector c(n);

    for (int i = 0; i < n; ++i)
        c[i] = values[i] * b;
    return c;
}

void Vector::zero()
{
    for (int i = 0; i < n; ++i)
    {
        values [i] = 0;
    }
}

Vector Vector::operator =(QVector<double> b)
{
    assert(n <= b.size());

    for (int i = 0; i < n; ++i)
        values[i] = b[i];
    return *this;
}

double Vector::norm()
{
    double norm = 0;

    for (int i = 0; i < n; i++)
    {
        norm += values[i] * values[i];
    }
    return sqrt(norm);
}

Vector Vector::operator +(Vector b)
{
    Vector c(n);

    assert(n == b.dim());

    for (int i = 0; i < n; ++i)
        c[i] = values[i] + b[i];
    return c;
}

void Vector::print()
{
    for (int i = 0; i < n; ++i)
    {
        QDebug d = qDebug();
        d << values[i];
    }
}

/* Matrix as SLAE methods */

Vector Matrix::reflection(Vector b)
{
    Matrix QT(n), Q(n), R(n);
    Vector c(n);

    assert(n == b.dim());

    QR(&Q, &R);
    QT = Q.trans();

    b = QT * b;


    for (int i = n - 1; i >= 0; i--)
    {
        c[i] = b[i];
        for (int j = i + 1; j < n; j++)
        {
            c[i] -= R[i][j] * c[j];
        }
        c[i] = c[i] / R[i][i];
    }

    return c;
}
