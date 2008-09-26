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


#ifndef __defined_libdai_bp_h
#define __defined_libdai_bp_h


#include <string>
#include <dai/daialg.h>
#include <dai/factorgraph.h>
#include <dai/properties.h>
#include <dai/enum.h>


namespace dai {


class BP : public DAIAlgFG {
    private:
        typedef std::vector<size_t> ind_t;
        struct EdgeProp {
            ind_t  index;
            Prob   message;
            Prob   newMessage;
            double residual;
        };
        std::vector<std::vector<EdgeProp> > _edges;
        /// Maximum difference encountered so far
        double _maxdiff;
        /// Number of iterations needed
        size_t _iters;
    
    public:
        struct Properties {
            size_t verbose;
            size_t maxiter;
            double tol;
            bool logdomain;
            double damping;
            DAI_ENUM(UpdateType,SEQFIX,SEQRND,SEQMAX,PARALL)
            UpdateType updates;
        } props;
        static const char *Name;

    public:
        /// Default constructor
        BP() : DAIAlgFG(), _edges(), _maxdiff(0.0), _iters(0U), props() {}

        /// Construct from FactorGraph fg and PropertySet opts
        BP( const FactorGraph & fg, const PropertySet &opts ) : DAIAlgFG(fg), _edges(), _maxdiff(0.0), _iters(0U), props() {
            setProperties( opts );
            construct();
        }

        /// Copy constructor
        BP( const BP & x ) : DAIAlgFG(x), _edges(x._edges), _maxdiff(x._maxdiff), _iters(x._iters), props(x.props) {}

        /// Clone *this (virtual copy constructor)
        virtual BP* clone() const { return new BP(*this); }

        /// Create (virtual constructor)
        virtual BP* create() const { return new BP(); }

        /// Assignment operator
        BP& operator=( const BP & x ) {
            if( this != &x ) {
                DAIAlgFG::operator=( x );
                _edges = x._edges;
                _maxdiff = x._maxdiff;
                _iters = x._iters;
                props = x.props;
            }
            return *this;
        }

        /// Return number of passes over the factorgraph
        size_t Iterations() const { return _iters; }

        /// Return maximum difference between single node beliefs for two consecutive iterations
        double maxDiff() const { return _maxdiff; }

        /// Identifies itself for logging purposes
        std::string identify() const;

        /// Get single node belief
        Factor belief( const Var &n ) const;

        /// Get general belief
        Factor belief( const VarSet &n ) const;

        /// Get all beliefs
        std::vector<Factor> beliefs() const;

        /// Get log partition sum
        Real logZ() const;

        /// Clear messages and beliefs
        void init();

        /// Clear messages and beliefs corresponding to the nodes in ns
        virtual void init( const VarSet &ns );

        /// The actual approximate inference algorithm
        double run();

        Factor beliefV( size_t i ) const;
        Factor beliefF( size_t I ) const;

    private:
        const Prob & message(size_t i, size_t _I) const { return _edges[i][_I].message; }
        Prob & message(size_t i, size_t _I) { return _edges[i][_I].message; }
        Prob & newMessage(size_t i, size_t _I) { return _edges[i][_I].newMessage; }
        const Prob & newMessage(size_t i, size_t _I) const { return _edges[i][_I].newMessage; }
        ind_t & index(size_t i, size_t _I) { return _edges[i][_I].index; }
        const ind_t & index(size_t i, size_t _I) const { return _edges[i][_I].index; }
        double & residual(size_t i, size_t _I) { return _edges[i][_I].residual; }
        const double & residual(size_t i, size_t _I) const { return _edges[i][_I].residual; }

        void calcNewMessage( size_t i, size_t _I );
        void updateMessage( size_t i, size_t _I ) {
            if( props.damping == 0.0 )
                message(i,_I) = newMessage(i,_I);
            else
                message(i,_I) = (message(i,_I) ^ props.damping) * (newMessage(i,_I) ^ (1.0 - props.damping));
        }
        void findMaxResidual( size_t &i, size_t &_I );

        /// Set Props according to the PropertySet opts, where the values can be stored as std::strings or as the type of the corresponding Props member
        void construct();
        void setProperties( const PropertySet &opts );
        PropertySet getProperties() const;
        std::string printProperties() const;
};


} // end of namespace dai


#endif
