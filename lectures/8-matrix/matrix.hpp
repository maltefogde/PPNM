#pragma once
#include<vector>
#include<iostream>
#include<string>
namespace pp{

struct vector{
	std::vector<double> data; 
	vector() = default;
	vector(int n) : data(n) {}
	vector(vector&) = default;
	vector(vector&&) = default;
	~vector() = default;

	int size() const {return data.size();}
	double& operator[](int i) {return data[i];}
	const double& operator[](int i) const {return data[i];}

	void print(std::string s="") const {
		std::cout<<s<<" ";
		for(auto &d : data)std::cout<<d<<" ";
		std::cout<<"\n";
	}

	vector& operator+=(const vector& other){
		for(int i=0;i<size();i++)(*this)[i]+=other[i];
		return (*this);
	}

	vector& operator-=(const vector& other){
		for(int i=0;i<size();i++)(*this)[i]-=other[i];
		return (*this);
	}

	vector& operator*=(const double c){
		for(int i=0;i<size();i++)(*this)[i]*=c;
		return (*this);
	}

	vector& operator/=(const double c){
		for(int i=0;i<size();i++)(*this)[i]/=c;
		return (*this);
	}

};

struct matrix{
	std::vector< pp::vector > cols;

	double& operator[](int i,int j){return cols[j][i];}
	double& operator()(int i,int j){return cols[j][i];}
};

}