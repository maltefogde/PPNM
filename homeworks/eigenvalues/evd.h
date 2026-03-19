#pragma once
#include "matrix.h"
#include <cmath>

namespace pp {

struct EVD_slow {
    pp::vector w;
    pp::matrix V;

    static void timesJ(pp::matrix& A, int p, int q, double theta){
        double c = std::cos(theta), s = std::sin(theta);
        for(int i = 0; i < (int)A.size1(); i++){
            double aip = A(i,p), aiq = A(i,q);
            A(i,p) = c*aip - s*aiq;
            A(i,q) = s*aip + c*aiq;
        }
    }

    static void Jtimes(pp::matrix& A, int p, int q, double theta){
        double c = std::cos(theta), s = std::sin(theta);
        for(int j = 0; j < (int)A.size2(); j++){
            double apj = A(p,j), aqj = A(q,j);
            A(p,j) =  c*apj + s*aqj;
            A(q,j) = -s*apj + c*aqj;
        }
    }

    EVD_slow(matrix A) : w(A.size1()), V(A.size1(), A.size1()) {
        int n = (int)A.size1();
        double eps = 1e-12;
        V.setid();

        bool changed;
        do{
            changed = false;
            for(int p = 0; p < n-1; p++){
                for(int q = p+1; q < n; q++){
                    double apq = A(p,q);
                    if(std::abs(apq) > eps){
                        changed = true;
                        double app = A(p,p);
                        double aqq = A(q,q);
                        double theta = 0.5*std::atan2(2*apq, aqq-app);

                        timesJ(A, p, q,  theta);   // full A <- A J
                        Jtimes(A, p, q, -theta);   // full A <- J^T A
                        timesJ(V, p, q,  theta);   // V <- V J
                    }
                }
            }
        } while(changed);

        for(int i = 0; i < n; i++) w[i] = A(i,i);
    }
};

struct EVD_fast {
    pp::vector w;
    pp::matrix V;

    static void rotate_upper(pp::matrix& A, pp::matrix& V, int p, int q){
        double apq = A(p,q);
        if(std::abs(apq) < 1e-300) return;

        double app = A(p,p);
        double aqq = A(q,q);

        double theta = 0.5*std::atan2(2*apq, aqq-app);
        double c = std::cos(theta);
        double s = std::sin(theta);

        // V <- VJ
        for(int i = 0; i < (int)V.size1(); i++){
            double vip = V(i,p), viq = V(i,q);
            V(i,p) = c*vip - s*viq;
            V(i,q) = s*vip + c*viq;
        }

        // only upper triangle of A

        for(int i = 0; i < p; i++){
            double aip = A(i,p), aiq = A(i,q);
            A(i,p) = c*aip - s*aiq;
            A(i,q) = s*aip + c*aiq;
        }

        for(int i = p+1; i < q; i++){
            double api = A(p,i), aiq = A(i,q);
            A(p,i) = c*api - s*aiq;
            A(i,q) = s*api + c*aiq;
        }

        for(int i = q+1; i < (int)A.size1(); i++){
            double api = A(p,i), aqi = A(q,i);
            A(p,i) = c*api - s*aqi;
            A(q,i) = s*api + c*aqi;
        }

        A(p,p) = c*c*app - 2*s*c*apq + s*s*aqq;
        A(q,q) = s*s*app + 2*s*c*apq + c*c*aqq;
        A(p,q) = 0.0;
    }

    EVD_fast(matrix A) : w(A.size1()), V(A.size1(), A.size1()) {
        int n = (int)A.size1();
        double eps = 1e-12;
        V.setid();

        bool changed;
        do{
            changed = false;
            for(int p = 0; p < n-1; p++){
                for(int q = p+1; q < n; q++){
                    if(std::abs(A(p,q)) > eps){
                        changed = true;
                        rotate_upper(A, V, p, q);
                    }
                }
            }
        } while(changed);

        for(int i = 0; i < n; i++) w[i] = A(i,i);
    }
};

} // namespace pp