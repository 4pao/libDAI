/*  Copyright (C) 2006-2008  Joris Mooij  [j dot mooij at science dot ru dot nl]
    Radboud University Nijmegen, The Netherlands

    This file is part of libDAI.

    libDAI is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    libDAI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libDAI; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef __defined_libdai_mr_h
#define __defined_libdai_mr_h


#include <vector>
#include <string>
#include <dai/factorgraph.h>
#include <dai/daialg.h>
#include <dai/enum.h>
#include <dai/properties.h>
#include <dai/exceptions.h>
#include <boost/dynamic_bitset.hpp>


namespace dai {


class MR : public DAIAlgFG {
    private:
        bool supported;                                            // is the underlying factor graph supported?

        std::vector<size_t>                             con;       // con[i] = connectivity of spin i
        std::vector<std::vector<size_t> >               nb;        // nb[i] are the neighbours of spin i
        std::vector<std::vector<double> >               tJ;        // tJ[i][_j] is the tanh of the interaction between spin i and its neighbour nb[i][_j]
        std::vector<double>                             theta;     // theta[i] is the local field on spin i
        std::vector<std::vector<double> >               M;         // M[i][_j] is M^{(i)}_j
        std::vector<std::vector<size_t> >               kindex;    // the _j'th neighbour of spin i has spin i as its kindex[i][_j]'th neighbour
        std::vector<std::vector<std::vector<double> > > cors;
    
        static const size_t kmax = 31;
        typedef boost::dynamic_bitset<> sub_nb;
        
        size_t N;

        std::vector<double> Mag;

        double _maxdiff;
        size_t _iters;

    public:
        struct Properties {
            size_t verbose;
            double tol;
            DAI_ENUM(UpdateType,FULL,LINEAR)
            DAI_ENUM(InitType,RESPPROP,CLAMPING,EXACT)
            UpdateType updates;
            InitType inits;
        } props;
        static const char *Name;

    public:
        /// Default constructor
        MR() : DAIAlgFG(), supported(), con(), nb(), tJ(), theta(), M(), kindex(), cors(), N(), Mag(), _maxdiff(), _iters(), props() {}

        /// Construct from FactorGraph fg and PropertySet opts
        MR( const FactorGraph &fg, const PropertySet &opts );

        /// Copy constructor
        MR( const MR &x ) : DAIAlgFG(x), supported(x.supported), con(x.con), nb(x.nb), tJ(x.tJ), theta(x.theta), M(x.M), kindex(x.kindex), cors(x.cors), N(x.N), Mag(x.Mag), _maxdiff(x._maxdiff), _iters(x._iters), props(x.props) {}

        /// Clone *this (virtual copy constructor)
        virtual MR* clone() const { return new MR(*this); }

        /// Create (virtual default constructor)
        virtual MR* create() const { return new MR(); }

        /// Assignment operator
        MR& operator=( const MR &x ) {
            if( this != &x ) {
                DAIAlgFG::operator=(x);
                supported = x.supported;
                con       = x.con; 
                nb        = x.nb;
                tJ        = x.tJ;
                theta     = x.theta;
                M         = x.M;
                kindex    = x.kindex;
                cors      = x.cors;
                N         = x.N;
                Mag       = x.Mag;
                _maxdiff  = x._maxdiff;
                _iters    = x._iters;
                props     = x.props;
            }
            return *this;
        }

        /// Identifies itself for logging purposes
        virtual std::string identify() const;

        /// Get single node belief
        virtual Factor belief( const Var &n ) const;

        /// Get general belief
        virtual Factor belief( const VarSet &/*ns*/ ) const { 
            DAI_THROW(NOT_IMPLEMENTED);
            return Factor(); 
        }

        /// Get all beliefs
        virtual std::vector<Factor> beliefs() const;

        /// Get log partition sum
        virtual Real logZ() const { 
            DAI_THROW(NOT_IMPLEMENTED);
            return 0.0; 
        }

        /// Clear messages and beliefs
        virtual void init() {}

        /// Clear messages and beliefs corresponding to the nodes in ns
        virtual void init( const VarSet &/*ns*/ ) {
            DAI_THROW(NOT_IMPLEMENTED);
        }

        /// The actual approximate inference algorithm
        virtual double run();

        /// Return maximum difference between single node beliefs in the last pass
        virtual double maxDiff() const { return _maxdiff; }

        /// Return number of passes over the factorgraph
        virtual size_t Iterations() const { return _iters; }


        void init(size_t Nin, double *_w, double *_th);
        void makekindex();
        void init_cor();
        double init_cor_resp();
        void solvemcav();
        void solveM();

        double _tJ(size_t i, sub_nb A);

        double Omega(size_t i, size_t _j, size_t _l);
        double T(size_t i, sub_nb A);
        double T(size_t i, size_t _j);
        double Gamma(size_t i, size_t _j, size_t _l1, size_t _l2);
        double Gamma(size_t i, size_t _l1, size_t _l2);

        double appM(size_t i, sub_nb A);
        void sum_subs(size_t j, sub_nb A, double *sum_even, double *sum_odd);

        double sign(double a) { return (a >= 0) ? 1.0 : -1.0; }
        
        void setProperties( const PropertySet &opts );
        PropertySet getProperties() const;
        std::string printProperties() const;
}; 


} // end of namespace dai


#endif
