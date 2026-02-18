#include<iostream>
#include<cstdio>
#include <vector>
#include"hello.h"
#include"vec.h"
int main(){
    hello();
    double a=1.0;
    double b=a;
    if(a==b) {printf("a==b\n");}
    else {printf("a!=b\n");}
    std::vector<double> v {1,2,3};
    for(size_t i=0;i<v.size();i++)printf("%g ",v[i]); //size_t is an unsigned integer type optimized using RAM
    printf("\n");
    for(auto vi : v){printf("%g ",vi);}
    printf("\n");
    for(double vi : v){printf("%g ",vi);}
    printf("\n");
    for(auto vi : v) vi=6; //vi is a copy of each element in v
    for(auto vi : v) printf("%g ",vi);
    printf("\n");
    for(auto& vi : v) vi=6; //vi is a copy of each element in v
    for(auto& vi : v) printf("%g ",vi);
    printf("\nNow coming while loop:");
    size_t i=0;
    while(i<v.size()){
        printf("v[%d]=%g",i,v[i]);
        i++;
    }
    printf("\nNow coming do-while loop:");
    i=0;
    do{
        printf("v[%d]=%g",i,v[i]);
        i++;
    } while(i<v.size());
    printf("\n");
    // auto u=v; //u is a copy of v
    // auto& w=v; //w is a reference to v
    pp::vec α(1,2,3);
    α.x = 6;
    std::cout<< α.x <<","<< α.y <<","<< α.z <<"\n";
return 0;
}