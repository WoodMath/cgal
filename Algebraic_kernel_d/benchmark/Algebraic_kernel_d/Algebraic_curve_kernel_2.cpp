 // TODO: Add licence
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL:$
// $Id: Algebraic_curve_kernel_2.C,v 1.2 2007/10/12 11:43:30 emeliyan Exp $
// 
//
// Author(s)     : Pavel Emeliyanenko <asm@mpi-sb.mpg.de>
//
// ============================================================================

// Benchmark for Algebraic_curve_kernel_2::Solve_2

#include <CGAL/basic.h>

#include <CGAL/Benchmark/Benchmark.hpp>
#include <CGAL/Benchmark/Option_parser.hpp>

#include <NiX/Arithmetic_traits.h>
#include <NiX/NT_traits.h>
#include <AcX/Algebraic_curve_2.h>
#include <AcX/Algebraic_curve_pair_2.h>

#include <CGAL/Algebraic_kernel_1.h>
#include <CGAL/Algebraic_curve_kernel_2.h>

#define BENCH_DEBUG_OUT
#ifdef BENCH_DEBUG_OUT
#define BENCH_OUT(x) std::cerr << x;
#else
#define BENCH_OUT(x)
#endif

template <class Integer>
struct Max_bit_size
{
    template <class X>
    int operator()(const NiX::Polynomial<X>& p) const
    {
        typename NiX::Polynomial<X>::const_iterator it = p.begin();
        Max_bit_size<Integer> max_bit_size;
        int max = max_bit_size(*it);
        while(++it != p.end()) 
            max = std::max(max, max_bit_size(*it));
        return max;
    }
    
    int operator()(const Integer& x) const
    { return CGAL::CGALi::ceil_log2_abs(x); }
};

struct Benchmark_result {
    int number_of_polys;
    double bits;
    int number_of_real_roots_found;
    int degree_of_polys;
    
    float solve_time;
    float sort_time;
    float to_double_time;
    float total_time;
};

std::ostream& operator <<(std::ostream& os, const Benchmark_result& res) {

    os << "n polys: " << res.number_of_polys << "\tn_bits: " << res.bits 
       << "\tn_real_roots: " << res.number_of_real_roots_found 
       << "\tdegre of polys: " << res.degree_of_polys << "\tsolve time: " <<
            res.solve_time << "\tsort time: " << res.sort_time <<
            "\t to_double time: " << res.to_double_time << 
            "\t total time: " << res.total_time << std::endl;
    return os;
}

template< class AlgebraicCurveKernel_2 >
class Bench_solve_2 {

public:
    //! this instance's first template argument    
    typedef AlgebraicCurveKernel_2 AK_2;
    //! in this setting Curve_2 == Polynomial_2
    typedef typename AK_2::Curve_2 Curve_2;
    //! type of result (bivariate polynomials solution)
    typedef typename AK_2::Xy_coordinate_2 Xy_coordinate_2;
        
    // shall we pass as a parameter internal polynomials instead of curves  ?
    //! container of input curves
    typedef std::vector<Curve_2> Curve_vector;
    //! container of resulting solutions
    typedef std::vector<Xy_coordinate_2> Root_vector;
    //! container of multiplicities
    typedef std::vector<int> Mult_vector;
    
public:
        
    void setup(Curve_vector curves) { 
//                    typename Root_vector::iterator roots_,
//                    typename Mult_vector::iterator mults_) {
        _m_curves.clear();
        _m_curves = curves;
        _m_roots.clear();
        _m_mults.clear();
    }
    
    Root_vector& get_roots() {
        return _m_roots;    
    }
    
    Mult_vector& get_multiplicities() {
        return _m_mults;    
    }
        
    int init() { return 0; }
    void clean() {}
    void sync() {}
    
    void op() 
    {
        int n = static_cast<int>(_m_curves.size());
        if(n < 2)
            return;
          
        _m_roots.clear();
        _m_mults.clear();    
        typename AK_2::Solve_2 solve_2;
        
        BENCH_OUT("number of curves: " << n << std::endl);
        for(int i = 0; i < n-1; i++)
        for(int j = i+1; j < n; j++) {

            BENCH_OUT("i = " << i << "; j = " << j << "\n");
              
            // CGAL::remove_scalar_factor( poly ); ??
            solve_2(_m_curves[i], _m_curves[j], std::back_inserter(_m_roots),
                  std::back_inserter(_m_mults));
        }
        
    }

private:
    
    Curve_vector _m_curves;
    Root_vector  _m_roots;
    Mult_vector  _m_mults;
};

template< class AlgebraicCurveKernel_2 >
class Bench_sort {

public:
    //! this instance's first template argument    
    typedef AlgebraicCurveKernel_2 AK_2;
    //! in this setting Curve_2 == Polynomial_2
    typedef typename AK_2::Curve_2 Curve_2;
    //! type of result (bivariate polynomials solution)
    typedef typename AK_2::Xy_coordinate_2 Xy_coordinate_2;
        
    // shall we pass as a parameter internal polynomials instead of curves  ?
    //! container of input curves
    typedef std::vector<Curve_2> Curve_vector;
    //! container of resulting solutions
    typedef std::vector<Xy_coordinate_2> Root_vector;
    //! container of multiplicities
    typedef std::vector<int> Mult_vector;
    
public:
        
    void setup(Root_vector *p_roots) { 
        
        CGAL_precondition(p_roots != NULL);
        _m_proots = p_roots;
    }
    
    int init() { return 0; }
    void clean() {}
    void sync() {}
    
    Root_vector *get_proots(){
        return _m_proots;
    }
 
    void op() 
    {
        // sort them out
        std::sort(_m_proots->begin(), _m_proots->end());
    }
    
private:

    Root_vector *_m_proots;
};

template< class AlgebraicCurveKernel_2 >
class Bench_to_double {

public:
    //! this instance's first template argument    
    typedef AlgebraicCurveKernel_2 AK_2;
    //! in this setting Curve_2 == Polynomial_2
    typedef typename AK_2::Curve_2 Curve_2;
    //! type of result (bivariate polynomials solution)
    typedef typename AK_2::Xy_coordinate_2 Xy_coordinate_2;
        
    // shall we pass as a parameter internal polynomials instead of curves  ?
    //! container of input curves
    typedef std::vector<Curve_2> Curve_vector;
    //! container of resulting solutions
    typedef std::vector<Xy_coordinate_2> Root_vector;
    //! container of multiplicities
    typedef std::vector<int> Mult_vector;
    
public:
        
    void setup(Root_vector *p_roots, Mult_vector *p_mults) { 
        
        CGAL_precondition(p_roots != NULL && p_mults != NULL);
        _m_proots = p_roots;
        _m_pmults = p_mults;
    }
    
    int init() { return 0; }
    void clean() {}
    void sync() {}
    
    Root_vector *get_proots(){
        return _m_proots;
    }
 
    void op() 
    {
        typedef typename Xy_coordinate_2::Boundary Boundary;
        typedef typename Xy_coordinate_2::Boundary_interval
                Boundary_interval;

        ::CGAL::set_mode(std::cerr, ::CGAL::IO::PRETTY);
        BENCH_OUT("Roots in ascending order: \n");
        
        int i = 0;
        typename Root_vector::const_iterator rit;
        typename Mult_vector::const_iterator mit;

        typedef typename AK_2::X_real_traits_1 X_traits;
        typedef typename AK_2::Y_real_traits_1 Y_traits;
        
        typename Y_traits::Lower_boundary lower_2;
        typename Y_traits::Upper_boundary upper_2;
        typename Y_traits::Refine refine_2;
            
        Boundary eps = Boundary(1)/CGAL::POLYNOMIAL::ipower(Boundary(2), 53);
        
        for(rit = _m_proots->begin(), mit = _m_pmults->begin(); rit !=
            _m_proots->end(); rit++, mit++, i++) {

            Boundary prec_x = (rit->x().high() - rit->x().low())*eps,
                     prec_y = (upper_2(*rit) - lower_2(*rit))*eps;
            while(1) {
                Boundary x_len = rit->x().high() - rit->x().low(),
                         y_len = upper_2(*rit) - lower_2(*rit);
                
                if(x_len <= prec_x && y_len <= prec_y)
                    break;
                // keep x/y-intervals comparable in size
                (x_len > y_len) ? rit->x().refine() : refine_2(*rit);
            }
            BENCH_OUT(i << ":\n (" <<
                NiX::to_double((rit->x().high() + rit->x().low())/2) << ", " <<
                NiX::to_double((upper_2(*rit) + lower_2(*rit))/2) <<
                "); multiplicity: " << *mit << "\n\n");
        }
    }
    
private:

    Root_vector *_m_proots;
    Mult_vector *_m_pmults;
};
    

template<class AlgebraicCurveKernel_2>
Benchmark_result do_benchmark(std::string filename, int n_samples = 5) 
{
    typedef AlgebraicCurveKernel_2 AK_2;

    typedef typename AK_2::Xy_coordinate_2 Xy_coordinate_2;
    
    typedef typename AK_2::Curve_2 Curve_2;
    typedef typename Curve_2::Poly_d Internal_polynomial_2;
    typedef typename NiX::Polynomial_traits<Internal_polynomial_2>::
        Innermost_coefficient Integer;
    
    typedef Bench_solve_2<AK_2> Bench_solve_ak_2;
    typedef typename Bench_solve_ak_2::Curve_vector Curve_vector;
    typedef typename Bench_solve_ak_2::Root_vector Root_vector;
    typedef typename Bench_solve_ak_2::Mult_vector Mult_vector;
    
    Benchmark_result result;
    
    CGAL::benchmark::Benchmark<Bench_solve_ak_2> 
        bench_solve_2(filename, 0, true);
        
    CGAL::benchmark::Benchmark<Bench_sort<AK_2> > 
        bench_sort(filename, 0, true);

    CGAL::benchmark::Benchmark<Bench_to_double<AK_2> >
        bench_to_double(filename, 0, true);
        
    bench_solve_2.set_samples(n_samples);
    bench_sort.set_samples(n_samples);
    bench_to_double.set_samples(n_samples);
        
    // read in the polynomials for testing
    std::ifstream file(filename.c_str());
    CGAL_assertion_msg(file, 
        (std::string("File not found: ") + filename).c_str());
     
    int n_polys;
    file >> n_polys; 
    
    AK_2 kernel_2;
    Curve_vector curves;
    Internal_polynomial_2 tmp;   
    for(int i = 0; i < n_polys; i++) {
       file >> tmp;
       // CGAL::remove_scalar_factor(poly); ??
       Curve_2 c = kernel_2.construct_curve_2_object()
            (NiX::canonicalize_polynomial(tmp));
       curves.push_back(c);
       CGAL_assertion(!file.eof());
    }
    file.close();       
    
    result.number_of_polys = n_polys;    
    result.degree_of_polys = curves.front().f().degree();
    
    Max_bit_size<Integer> max_bit_size;
    double total_bits = 0.0;
    for(typename Curve_vector::iterator it = curves.begin(); 
            it != curves.end(); it++)
        total_bits += max_bit_size(it->f());
        
    result.bits = total_bits / n_polys;
        
    bench_solve_2.get_benchable().setup(curves);
    bench_solve_2();
    result.solve_time = bench_solve_2.get_period() / n_samples;
    
    Root_vector& roots = bench_solve_2.get_benchable().get_roots();
    Mult_vector& mults = bench_solve_2.get_benchable().get_multiplicities();
    result.number_of_real_roots_found = roots.size();

    bench_sort.get_benchable().setup(&roots);
    bench_sort();
    result.sort_time = bench_sort.get_period() / n_samples;

    bench_to_double.get_benchable().setup(&roots, &mults);
    bench_to_double();
    result.to_double_time = bench_to_double.get_period() / n_samples;

    result.total_time = result.solve_time + result.sort_time +
        result.to_double_time;
    return result;
}


int main( int argc, char** argv ) {

    if(argc > 1) {
        CGAL_assertion(argc >= 2);
        int n_samples = (argc >= 3) ? std::atoi(argv[2]) : 5;

#ifdef LiS_HAVE_CORE
        typedef NiX::CORE_arithmetic_traits AT;
#else
#ifdef CGAL_USE_LEDA
        typedef NiX::LEDA_arithmetic_traits AT;
#endif
#endif
        typedef AT::Integer Coefficient;
        
        typedef AcX::Algebraic_curve_2<AT> Curve_2;
        typedef AcX::Algebraic_curve_pair_2<Curve_2> Curve_pair_2;
            
        typedef CGAL::Algebraic_kernel_1<Coefficient> Kernel_1;
        typedef CGAL::Algebraic_curve_kernel_2<Curve_pair_2, Kernel_1>
            Kernel_2;
                
        std::cerr << do_benchmark<Kernel_2>(argv[1], n_samples);
    
    } else {
        std::cerr << "No parameters found" << std::endl;    
    }
            
    return 0;    
}
