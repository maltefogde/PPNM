#pragma once
#include<iostream>
#include<cstdio>
#include<cassert>
#include<cmath>
#include<string>
#include<vector>
#include<initializer_list>
#include<functional>
#define SELF (*this)
#define FOR(i,V)  for(size_t i=0; i<(V).size();  ++i)
#define FOR1(i,A) for(size_t i=0; i<(A).size1(); ++i)
#define FOR2(j,A) for(size_t j=0; j<(A).size2(); ++j)

namespace pp{

struct vector {
	std::vector<double> data;

	vector() = default;
	vector(size_t n) : data(n) {}
	vector(std::initializer_list<double> list) : data(list) {}
	vector(const vector&) = default;
	vector(vector&&) noexcept = default;

	vector& operator=(const vector&) = default;
	vector& operator=(vector&&) noexcept = default;

	inline size_t size() const {return data.size();}
//	auto n(){return std::views::iota(0,size());}
//	void resize(size_t n) {data.resize(n);}
	inline double& operator[](size_t i) {return data[i];}
	inline const double& operator[](size_t i) const {return data[i];}

	vector& operator+=(const vector& other){
		assert(size()==other.size());
		FOR(i,SELF) SELF[i]+=other[i];
		//for(size_t i:n())SELF[i]+=other[i];
		return SELF;
		}

	vector& operator-=(const vector& other){
		assert(size()==other.size());
		FOR(i,SELF) SELF[i]-=other[i];
		return SELF;
		}

	vector& operator*=(double c){
		FOR(i,SELF) SELF[i]*=c;
		return SELF;
		}

	vector& operator/=(double c){
		FOR(i,SELF) SELF[i]/=c;
		return SELF;
		}

	double norm() const {
		double sum2=0;
		FOR(i,SELF) sum2+=SELF[i]*SELF[i];
		return std::sqrt(sum2);
	}

	void print(std::string s="") const {
		std::cout<<s;
//		for(auto &x : data) std::cout<<x<<" ";
		for(auto &x : data) printf("%10.3g ",x);
		std::cout<<"\n";
	}

	vector map(std::function<double(double)> f) const{
		vector r(size());
		FOR(i,SELF) r.data[i]=f(data[i]);
		return r;
	}

}; //vector

inline vector operator+(vector a, const vector& b){ a+=b ; return a; }
inline vector operator-(vector a)                 { a*=-1; return a; }
inline vector operator-(vector a, const vector& b){ a-=b ; return a; }
inline vector operator*(vector a, const double c) { a*=c ; return a; }
inline vector operator*(const double c, vector a) { a*=c ; return a; }
inline vector operator/(vector a, const double c) { a/=c ; return a; }

inline double operator*(const vector& a, const vector& b){
    assert(a.size()==b.size());
    double s = 0;
    for(size_t i=0; i<a.size(); i++) s += a[i]*b[i];
    return s;
}

struct matrix {
	std::vector<pp::vector> cols;
	matrix()=default;
	matrix(int n,int m) : cols(m, pp::vector(n)) {}
	matrix(const matrix& other)=default;
	matrix(matrix&& other)=default;
	matrix& operator=(const matrix& other)=default;
	matrix& operator=(matrix&& other)=default;
	inline size_t size1() const { return cols.empty() ? 0 : cols[0].size(); }
	inline size_t size2() const { return static_cast<size_t>(cols.size()); }
	inline double& operator()(size_t i, size_t j){ return cols[j][i]; }
	inline const double& operator()(size_t i, size_t j) const { return cols[j][i]; }
	inline vector& operator[](size_t j){ return cols[j]; }
	inline const vector& operator[](size_t j) const { return cols[j]; }
//	void resize(size_t n, size_t m);
	void setid(){
		assert(size1()==size2());
		for(size_t i=0;i<size1();i++){
			SELF(i,i)=1;
			for(size_t j=i+1;j<size1();j++) SELF(i,j)=SELF(j,i)=0;
			}
		}
	matrix transpose() const{
		matrix R(size2(),size1());
		FOR1(i,SELF)
		FOR2(j,SELF)
			R(j,i)=SELF(i,j);
		return R;
	}

	matrix T() const {return transpose();}
	
	double get (size_t i, size_t j) const {return cols[j][i];}
	void set(size_t i, size_t j, double value){cols[j][i] = value;}
//	vector get_col(size_t j);
//	void set_col(size_t j,vector& cj);

	matrix& operator+=(const matrix& B){
		assert(size1()==B.size1() && size2()==B.size2());
		for(size_t i=0;i<size2();i++)SELF[i]+=B[i];
		return *this;
		}

	matrix& operator-=(const matrix& B){
		assert(size1()==B.size1() && size2()==B.size2());
		for(size_t i=0;i<size2();i++)SELF[i]-=B[i];
		return *this;
		}
	matrix& operator*=(const double c){
		for(size_t i=0;i<size2();i++)SELF[i]*=c;
		return *this;
		}
	matrix& operator/=(const double c){
		for(size_t i=0;i<size2();i++)SELF[i]/=c;
		return *this;
		}
	matrix& operator*=(const matrix&);
	matrix  operator^(int);

	void print(std::string s="") const{
		printf("%s\n",s.c_str());
		for(size_t i=0;i<size1();i++){
			for(size_t j=0;j<size2();j++)printf("%10.3g ",SELF(i,j));
			printf("\n");
		}
	}
};

inline matrix operator+(matrix A, const matrix& B){
	for(size_t i=0;i<A.size2();i++)A[i]+=B[i];
	return A;
	}

inline matrix operator-(matrix A, const matrix& B){
	for(size_t i=0;i<A.size2();i++)A[i]-=B[i];
	return A;
	}

inline vector operator*(const matrix& A, const vector& v){
	assert(A.size2()==v.size());
	vector r(A.size1());
	FOR2(j,A){
		double vj=v[j];
		FOR1(i,A) r[i]+=A(i,j)*vj;
		}
	return r;
	}

inline matrix operator*(const matrix& A, const matrix& B){
	assert(A.size2()==B.size1());
	matrix R(A.size1(),B.size2());
	FOR2(j,R) R[j]=A*B[j];
	return R;
	}

inline matrix operator*(matrix A, const double c){
	for(auto &col : A.cols) col*=c;
	return A;
	}

inline matrix operator*(const double c, matrix A){
	for(auto &col : A.cols) col*=c;
	return A;
	}

inline matrix operator/(matrix A, const double c){
	for(auto &col : A.cols) col/=c;
	return A;
	}

inline void backsub(const matrix& U, vector& c){
    assert(U.size1()==U.size2());
    assert(U.size1()==c.size());
    for(int i=(int)U.size2()-1; i>=0; i--){
        c[i] /= U(i,i);
        for(int j=i-1; j>=0; j--) c[j] -= U(j,i)*c[i];
    }
}

}//pp