#include"matrix.hpp"
int main(){
	int n=5;
	pp::vector u(n);
	pp::vector v(n);
	for(int i=0;i<n;i++)v[i]=i+1;
	for(int i=0;i<n;i++)u[i]=i+100;
	v.print("v=");
	u.print("u=");
	v+=u;
	v.print("new v=");
	u*=1000;
	u.print("even newer u=");
}