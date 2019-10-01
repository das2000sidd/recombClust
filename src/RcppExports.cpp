// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <RcppEigen.h>
#include <Rcpp.h>

using namespace Rcpp;

// cLDmixtureModel
List cLDmixtureModel(RObject dat, Nullable<int> maxSteps, Nullable<double> prob0, Nullable<int> blocksize);
RcppExport SEXP _recombClust_cLDmixtureModel(SEXP datSEXP, SEXP maxStepsSEXP, SEXP prob0SEXP, SEXP blocksizeSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< RObject >::type dat(datSEXP);
    Rcpp::traits::input_parameter< Nullable<int> >::type maxSteps(maxStepsSEXP);
    Rcpp::traits::input_parameter< Nullable<double> >::type prob0(prob0SEXP);
    Rcpp::traits::input_parameter< Nullable<int> >::type blocksize(blocksizeSEXP);
    rcpp_result_gen = Rcpp::wrap(cLDmixtureModel(dat, maxSteps, prob0, blocksize));
    return rcpp_result_gen;
END_RCPP
}
// timesTwo
NumericVector timesTwo(NumericVector x);
RcppExport SEXP _recombClust_timesTwo(SEXP xSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericVector >::type x(xSEXP);
    rcpp_result_gen = Rcpp::wrap(timesTwo(x));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_recombClust_cLDmixtureModel", (DL_FUNC) &_recombClust_cLDmixtureModel, 4},
    {"_recombClust_timesTwo", (DL_FUNC) &_recombClust_timesTwo, 1},
    {NULL, NULL, 0}
};

RcppExport void R_init_recombClust(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}