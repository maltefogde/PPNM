#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstring>
#include <string>
#include <limits>

#include "integrator.h"

const double pi = std::acos(-1.0);

struct IntegralTest {
    std::string name;
    double a;
    double b;
    double exact;
};

// "test_integral" written using ChatGPT
template<class F>
void test_integral(
    const std::string& name,
    F f,
    double a,
    double b,
    double exact,
    double acc,
    double eps
) {
    int ncalls = 0;

    auto counted_f = [&f, &ncalls](double x) {
        ncalls++;
        return f(x);
    };

    auto result = integrate(counted_f, a, b, acc, eps);

    double q = result.value;
    double estimated_error = result.error;
    double actual_error = std::abs(q - exact);
    double goal = acc + eps*std::abs(q);

    std::cout << name << "\n";
    std::cout << "  result          = " << q << "\n";
    std::cout << "  exact           = " << exact << "\n";
    std::cout << "  estimated error = " << estimated_error << "\n";
    std::cout << "  actual error    = " << actual_error << "\n";
    std::cout << "  goal            = " << goal << "\n";
    std::cout << "  ncalls          = " << ncalls << "\n";

    if(actual_error <= goal) {
        std::cout << "  status          = OK\n\n";
    } else {
        std::cout << "  status          = NOT OK\n\n";
    }
}

// "erf_integral" written myself and improved using ChatGPT
double erf_integral(double z, double acc = 1e-6, double eps = 1e-6) {
    const double factor = 2.0/std::sqrt(pi);

    if(z < 0.0) {
        return -erf_integral(-z, acc, eps);
    }

    if(z <= 1.0) {
        auto f = [](double x) {
            return std::exp(-x*x);
        };

        return factor*integrate(f, 0.0, z, acc/factor, eps).value;
    }

    auto tail = [z](double t) {
        if(t == 0.0) {
            return 0.0;
        }

        double x = z + (1.0 - t)/t;

        if(x > 26.0) {
            return 0.0;
        }

        return std::exp(-x*x)/(t*t);
    };

    return 1.0 - factor*integrate(tail, 0.0, 1.0, acc/factor, eps).value;
}

// "write_erf_data" written using ChatGPT
void write_erf_data() {
    std::ofstream file("erf.dat");

    file << "# z erf_integral std_erf\n";

    for(int i = 0; i <= 120; i++) {
        double z = -3.0 + i*6.0/120.0;

        file << z << " "
             << erf_integral(z, 1e-7, 1e-7) << " "
             << std::erf(z) << "\n";
    }

    std::cout << "Wrote erf.dat\n";
}

// "write_erf_accuracy_data" written using ChatGPT
void write_erf_accuracy_data() {
    std::ofstream file("erf_acc.dat");

    file << "# acc error\n";

    double exact = std::erf(1.0);

    for(int k = 1; k <= 10; k++) {
        double acc = std::pow(10.0, -k);
        double y = erf_integral(1.0, acc, 0.0);
        double error = std::abs(y - exact);

        file << acc << " " << error << "\n";
    }

    std::cout << "Wrote erf_acc.dat\n";
}

// "compare_plain_and_cc" written myself and improved using ChatGPT
template<class F>
void compare_plain_and_cc(
    const std::string& name,
    F f,
    double a,
    double b,
    double exact,
    double acc,
    double eps
) {
    std::cout << name << "\n";
    std::cout << "  exact = " << exact << "\n\n";

    int ncalls_plain = 0;
    double q_plain = std::numeric_limits<double>::quiet_NaN();
    double error_plain = std::numeric_limits<double>::quiet_NaN();

    auto f_plain = [&f, &ncalls_plain](double x) {
        ncalls_plain++;
        return f(x);
    };

    try {
        auto result_plain = integrate(f_plain, a, b, acc, eps);
        q_plain = result_plain.value;
        error_plain = std::abs(q_plain - exact);

        std::cout << "  ordinary integral = " << q_plain << "\n";
        std::cout << "  ordinary error    = " << error_plain << "\n";
        std::cout << "  ordinary est err = " << result_plain.error << "\n";
        std::cout << "  ordinary ncalls   = " << ncalls_plain << "\n";
        std::cout << "  ordinary status   = OK\n\n";
    } catch(const std::exception& e) {
        std::cout << "  ordinary integral = failed\n";
        std::cout << "  ordinary reason   = " << e.what() << "\n";
        std::cout << "  ordinary ncalls   = " << ncalls_plain << "\n";
        std::cout << "  ordinary status   = NOT OK\n\n";
    }

    int ncalls_cc = 0;
    double q_cc = std::numeric_limits<double>::quiet_NaN();
    double error_cc = std::numeric_limits<double>::quiet_NaN();

    auto f_cc = [&f, &ncalls_cc](double x) {
        ncalls_cc++;
        return f(x);
    };

    try {
        auto result_cc = integrate_cc(f_cc, a, b, acc, eps);
        q_cc = result_cc.value;
        error_cc = std::abs(q_cc - exact);

        std::cout << "  CC integral       = " << q_cc << "\n";
        std::cout << "  CC error          = " << error_cc << "\n";
        std::cout << "  CC est err       = " << result_cc.error << "\n";
        std::cout << "  CC ncalls         = " << ncalls_cc << "\n";
        std::cout << "  CC status         = OK\n\n";
    } catch(const std::exception& e) {
        std::cout << "  CC integral       = failed\n";
        std::cout << "  CC reason         = " << e.what() << "\n";
        std::cout << "  CC ncalls         = " << ncalls_cc << "\n";
        std::cout << "  CC status         = NOT OK\n\n";
    }
}

// "test_infinite_integral" written using ChatGPT
template<class F>
void test_infinite_integral(
    const std::string& name,
    F f,
    double a,
    double b,
    double exact,
    double acc,
    double eps
) {
    int ncalls = 0;

    auto counted_f = [&f, &ncalls](double x) {
        ncalls++;
        return f(x);
    };

    auto result = integrate_cc(counted_f, a, b, acc, eps);
    double q = result.value;
    double estimated_error = result.error;
    double actual_error = std::abs(q - exact);
    double goal = acc + eps*std::abs(q);

    std::cout << name << "\n";
    std::cout << "  result = " << q << "\n";
    std::cout << "  exact  = " << exact << "\n";
    std::cout << "  estimated error = " << estimated_error << "\n";
    std::cout << "  actual error    = " << actual_error << "\n";
    std::cout << "  goal   = " << goal << "\n";
    std::cout << "  ncalls = " << ncalls << "\n";

    if(actual_error <= goal) {
        std::cout << "  status = OK\n\n";
    } else {
        std::cout << "  status = NOT OK\n\n";
    }
}

// "test_error_estimate" written using ChatGPT
template<class F>
void test_error_estimate(
    const std::string& name,
    F f,
    double a,
    double b,
    double exact,
    double acc,
    double eps
) {
    int ncalls = 0;

    auto counted_f = [&f, &ncalls](double x) {
        ncalls++;
        return f(x);
    };

    auto result = integrate(counted_f, a, b, acc, eps);

    double q = result.value;
    double estimated_error = result.error;
    double actual_error = std::abs(q - exact);

    double ratio = std::numeric_limits<double>::quiet_NaN();

    if(estimated_error > 0.0) {
        ratio = actual_error / estimated_error;
    }

    std::cout << name << "\n";
    std::cout << "  result             = " << q << "\n";
    std::cout << "  exact              = " << exact << "\n";
    std::cout << "  estimated error    = " << estimated_error << "\n";
    std::cout << "  actual error       = " << actual_error << "\n";
    std::cout << "  actual/estimated   = " << ratio << "\n";
    std::cout << "  ncalls             = " << ncalls << "\n";

    if(actual_error <= estimated_error) {
        std::cout << "  estimate status    = conservative\n\n";
    } else {
        std::cout << "  estimate status    = underestimates actual error\n\n";
    }
}

int main(int argc, char** argv) {
    bool do_taskA = false;
    bool do_taskB = false;
    bool do_taskC = false;

    for(int i = 1; i < argc; ++i) {
        if(std::strcmp(argv[i], "-taskA") == 0) {
            do_taskA = true;
        } else if(std::strcmp(argv[i], "-taskB") == 0) {
            do_taskB = true;
        } else if(std::strcmp(argv[i], "-taskC") == 0) {
            do_taskC = true;
        }
    }

    if(!do_taskA && !do_taskB && !do_taskC) {
        do_taskA = do_taskB = do_taskC = true;
    }

    std::cout << std::setprecision(16);

    if(do_taskA) {
        std::cout << "\n------ TASK A ------\n\n";

        const double acc = 1e-6;
        const double eps = 1e-6;

        std::cout << "Testing the adaptive integrator on improper integrals.\n\n";

        test_integral(
            "Integral of sqrt(x) from 0 to 1",
            [](double x) { return std::sqrt(x); },
            0.0,
            1.0,
            2.0/3.0,
            acc,
            eps
        );

        test_integral(
            "Integral of 1/sqrt(x) from 0 to 1",
            [](double x) { return 1.0/std::sqrt(x); },
            0.0,
            1.0,
            2.0,
            acc,
            eps
        );

        test_integral(
            "Integral of sqrt(1 - x^2) from 0 to 1",
            [](double x) { return std::sqrt(1.0 - x*x); },
            0.0,
            1.0,
            pi/4.0,
            acc,
            eps
        );

        test_integral(
            "Integral of log(x)/sqrt(x) from 0 to 1",
            [](double x) { return std::log(x)/std::sqrt(x); },
            0.0,
            1.0,
            -4.0,
            acc,
            eps
        );

        std::cout << "Computing the error function from its integral definition.\n\n";

        for(double z = -3.0; z <= 3.0001; z += 0.5) {
            double y = erf_integral(z, 1e-8, 1e-8);
            double exact = std::erf(z);
            double error = std::abs(y - exact);

            std::cout << "z = " << z << "\n";
            std::cout << "  erf_integral = " << y << "\n";
            std::cout << "  std::erf     = " << exact << "\n";
            std::cout << "  error        = " << error << "\n\n";
        }

        write_erf_data();
        write_erf_accuracy_data();
    }

    if(do_taskB) {
        std::cout << "\n------ TASK B ------\n\n";

        const double acc = 1e-6;
        const double eps = 1e-6;

        std::cout << "Clenshaw-Curtis variable transformation.\n";
        std::cout << "Comparison with the ordinary adaptive integrator.\n\n";

        compare_plain_and_cc(
            "Integral of 1/sqrt(x) from 0 to 1",
            [](double x) { return 1.0/std::sqrt(x); },
            0.0,
            1.0,
            2.0,
            acc,
            eps
        );

        compare_plain_and_cc(
            "Integral of log(x)/sqrt(x) from 0 to 1",
            [](double x) { return std::log(x)/std::sqrt(x); },
            0.0,
            1.0,
            -4.0,
            acc,
            eps
        );

        compare_plain_and_cc(
            "Integral of 1/sqrt(1 - x) from 0 to 1",
            [](double x) { return 1.0/std::sqrt(1.0 - x); },
            0.0,
            1.0,
            2.0,
            acc,
            eps
        );

        std::cout << "Infinite interval integrals using variable transformations.\n\n";

        test_infinite_integral(
            "Integral of exp(-x) from 0 to infinity",
            [](double x) { return std::exp(-x); },
            0.0,
            std::numeric_limits<double>::infinity(),
            1.0,
            acc,
            eps
        );

        test_infinite_integral(
            "Integral of 1/(1 + x^2) from -infinity to infinity",
            [](double x) { return 1.0/(1.0 + x*x); },
            -std::numeric_limits<double>::infinity(),
            std::numeric_limits<double>::infinity(),
            pi,
            acc,
            eps
        );

        test_infinite_integral(
            "Integral of exp(-x^2) from -infinity to infinity",
            [](double x) { return std::exp(-x*x); },
            -std::numeric_limits<double>::infinity(),
            std::numeric_limits<double>::infinity(),
            std::sqrt(pi),
            acc,
            eps
        );
    }

    if(do_taskC) {
        std::cout << "\n------ TASK C ------\n\n";

        const double acc = 1e-6;
        const double eps = 1e-6;

        std::cout << "Testing the quality of the returned error estimate.\n";
        std::cout << "The actual error is abs(result - exact).\n";
        std::cout << "The ratio actual/estimated should ideally be around or below 1.\n\n";

        test_error_estimate(
            "Integral of sqrt(x) from 0 to 1",
            [](double x) { return std::sqrt(x); },
            0.0,
            1.0,
            2.0/3.0,
            acc,
            eps
        );

        test_error_estimate(
            "Integral of 1/sqrt(x) from 0 to 1",
            [](double x) { return 1.0/std::sqrt(x); },
            0.0,
            1.0,
            2.0,
            acc,
            eps
        );

        test_error_estimate(
            "Integral of log(x)/sqrt(x) from 0 to 1",
            [](double x) { return std::log(x)/std::sqrt(x); },
            0.0,
            1.0,
            -4.0,
            acc,
            eps
        );

        test_error_estimate(
            "Integral of sin(100x) from 0 to 1",
            [](double x) { return std::sin(100.0*x); },
            0.0,
            1.0,
            (1.0 - std::cos(100.0))/100.0,
            acc,
            eps
        );

        test_error_estimate(
            "Integral of narrow peak from 0 to 1",
            [](double x) {
                return 1.0/(1.0 + 10000.0*(x - 0.5)*(x - 0.5));
            },
            0.0,
            1.0,
            std::atan(50.0)/50.0,
            acc,
            eps
        );
    }

    return 0;
}