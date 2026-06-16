#include <iostream>
#include <random>
#include <string>
#include <cstring>
#include <cmath>
#include "matrix.h"
#include "qr.h"

using namespace pp;

inline bool approx(double x, double y, double acc=1e-6, double eps=1e-6){
	if(std::abs(x-y) < acc) return true;
	if(std::abs(x-y) < eps*std::max(std::abs(x),std::abs(y))) return true;
	return false;
	}

inline bool approx(const vector& a, const vector& b, double acc=1e-6, double eps=1e-6){
	if(a.size() != b.size()) return false;
	for(size_t i=0;i<a.size();i++)
		if(!approx(a[i],b[i],acc,eps)) return false;
	return true;
	}

inline matrix eye(int n){
    matrix I(n,n);
    for(int i=0; i<n; i++) I(i,i)=1.0;
    return I;
}

inline bool approx(const matrix& A, const matrix& B, double acc=1e-6, double eps=1e-6){
    if(A.size1() != B.size1() || A.size2() != B.size2()) return false;
    for(size_t i=0; i<A.size1(); i++)
        for(size_t j=0; j<A.size2(); j++)
            if(!approx(A(i,j), B(i,j), acc, eps)) return false;
    return true;
}

inline bool is_upper_triangular(const matrix& R, double eps = 1e-10){
    for(size_t i=0; i<R.size1(); i++)
        for(size_t j=0; j<i && j<R.size2(); j++)
            if(std::abs(R(i,j)) > eps) return false;
    return true;
}

matrix random_matrix(int n, int m, std::mt19937& rng){
    std::uniform_real_distribution<double> dist(-1.0,1.0);
    matrix A(n,m);
    for(int i=0;i<n;i++)
        for(int j=0;j<m;j++)
            A(i,j)=dist(rng);
    return A;
}

vector random_vector(int n, std::mt19937& rng){
    std::uniform_real_distribution<double> dist(-1.0,1.0);
    vector v(n);
    for(int i=0;i<n;i++)
        v[i]=dist(rng);
    return v;
}

int main(int argc, char** argv){
    std::mt19937 rng(1);

	for(int i=1; i<argc-1; i++){
        if(std::strcmp(argv[i], "-size") == 0){
            int N = std::stoi(argv[i+1]);
            matrix A = random_matrix(N, N, rng);
            QRdec QR(A);
            return 0;
        }
    }

	std::uniform_int_distribution<int> dist_n(2, 8);
	std::uniform_int_distribution<int> dist_m(1, 7);
	int n = dist_n(rng);
	int m = dist_m(rng);
	while (n <= m) {
		n = m + 1;
	}

	matrix A_nm = random_matrix(n,m,rng);
    QRdec QR_nm(A_nm);

	std::cout << "\n------ TASK A.1 and A.2 ------ \n";
    std::cout << "Generating "<<n<<"x"<<m<<" matrix A:\n";
	A_nm.print("A =");
	std::cout << "\nQR decomposition of A:\n";
	QR_nm.Q.print("Q =");
	QR_nm.R.print("R =");
	// check that R is upper triangular; check that QTQ=1; check that QR=A;
	std::cout << "R upper triangular: " << is_upper_triangular(QR_nm.R) << "\n";
	std::cout << "Q^T Q = I:          " << approx(QR_nm.Q.transpose()*QR_nm.Q, eye(m)) << "\n";
	std::cout << "Q R = A:            " << approx(QR_nm.Q*QR_nm.R, A_nm) << "\n";


	std::cout << "\n\n------ TASK A.3 ------ \n";
	matrix A_nn = random_matrix(n,n,rng);
    QRdec QR_nn(A_nn);
	vector b = random_vector(n,rng);
	std::cout << "Generating "<<n<<"x"<<n<<" matrix A:\n";
	A_nn.print("A =");
	std::cout << "Generating "<<n<<"-dimensional vector b:\n";
	b.print("b =");

	std::cout << "Solving A x = b:\n";
	vector x = QR_nn.solve(b);
	std::cout << "Solution x:\n";
	x.print("x =");
	std::cout << "Check A x = b: " << approx(A_nn*x, b) << "\n";

	std::cout << "\n\n------ TASK A.4 ------ \n";
	std::cout << "Determinant of "<<n<<"x"<<n<<" matrix A:\n";
	std::cout << "det(R) = det(A): " << QR_nn.det() << "\n";

	std::cout << "\n\n------ TASK B ------ \n";
	matrix B = QR_nn.inverse();
    matrix I = eye(n);

    std::cout << "A B = I: " << approx(A_nn*B,I) << "\n";
    std::cout << "B A = I: " << approx(B*A_nn,I) << "\n";

	std::cout << "\n\n------ TASK C ------ \n";
	std::cout << "See times.svg\n";
	std::cout << "Notice that I use 'gtime' instead of 'time'\n";

    return 0;
}