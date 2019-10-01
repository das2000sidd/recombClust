#include "include/CLDmixtureModel.h"

using namespace Rcpp;

//' Run LDmixture model to a pair of SNP-blocks
//'
//' @param dat Matrix with the genotype data
//' @param maxSteps Numerical with the maximum number of iterations run by the EM algorithm
//' @param prob0 Initial mixture probability.
//' @return A list with the LDmixture results
//' \itemize{
//'  \item{"logMix"}{Log-likelihood of mixture model}
//'  \item{"logLD"}{Log-likelihood of linkage model}
//'  \item{"logNoLD"}{Log-likelihood of recomb model}
//'  \item{"BIC"}{BIC of the mixture vs the base model}
//'  \item{"prob"}{Proportion of chromosomes belonging to recomb model}
//'  \item{"steps"}{Number of iterations until converge of the EM algorithm}
//'  \item{"pval"}{P-value of the Chi-square test}
//'  \item{"r1"}{Responsibilities for recomb population of each chromosomes. It is
//'  only available for selected models (BIC > 10, pval > 0.05)}
//' }
//' @export
// [[Rcpp::export]]
List cLDmixtureModel( RObject dat, Nullable<int> maxSteps = R_NilValue, Nullable<double> prob0 = R_NilValue,  Nullable<int> blocksize = R_NilValue) 
{
/* dat conté : 
              [,1] [,2]
    HG00100_1 "00" "00"
    HG00100_2 "11" "11"
    HG00101_1 "00" "00"
    HG00101_2 "00" "00"
    HG00102_1 "11" "11"
    HG00102_2 "00" "01"
*/
   
   int imaxSteps, iblocksize;
   double dprob0;
   CharacterMatrix cdat = transpose(as<CharacterMatrix>(dat));

   
   // Variable initialization default values
   if(maxSteps.isNotNull())  imaxSteps = as<int> (maxSteps);
   else    imaxSteps = 1000;
   
   if(prob0.isNotNull())  dprob0 = as<double> (prob0);
   else    dprob0 = 0.5;
   
   if(blocksize.isNotNull())  iblocksize = as<int> (blocksize);
   else    iblocksize = 2;

/* Volem obtenir ins :
    HG00100_1 HG00100_2 HG00101_1 HG00101_2 HG00102_1 HG00102_2 HG00103_1 
    "00+00"   "11+11"   "00+00"   "00+00"   "11+11"   "00+01"   "00+00" 
*/
   int nSNP = cdat(0,0).size();
   StringVector cnames = colnames(cdat);
   StringVector inds = concatenate( cdat(0,_), cdat(1,_), "+" );
   
   NumericVector r1(inds.length()); 
   r1.fill(1);
   
   NumericVector propsRecomb = CRecombFreq(r1, inds, iblocksize);
   NumericVector propsLink = CLinkageFreq(r1, inds, iblocksize);
   
  
   r1 = getNumericVectorfromStringVector( VectortoOrderedMap(propsRecomb), inds );
   NumericVector r2 = getNumericVectorfromStringVector( VectortoOrderedMap(propsLink), inds );
   
   r1.attr("names") = inds;
   r2.attr("names") = inds;
   
   double LoglikeRecomb = sum(log(r1));
   double LoglikeLinkage = sum(log(r2));
   
   r1 = dprob0*r1;
   r2 = (1-dprob0)*r2;
   
   
   List params =  List::create(Named("r1") = r1,  Named("r2") = r2,  
                               Named("props1") = propsRecomb, Named("props2") = propsLink,
                               Named("prob0") = dprob0, Named("inds") = inds, Named("blocksize") = iblocksize);
  
 
   // EM loop
   double tol =  1;
   int steps = 1;
   double MINTOL = .000000001;
   
   while(tol > MINTOL & steps <= imaxSteps)
   {
      // El código de updateModel se puede poner directamente dentro del bucle
      List newparams = CupdateModel(params);

      NumericVector parprops1 = as<NumericVector>(params["props1"]);
      NumericVector nparprops1 = as<NumericVector>(newparams["props1"]);
      
      NumericVector parprops2 = as<NumericVector>(params["props2"]);
      NumericVector nparprops2 = as<NumericVector>(newparams["props2"]);
      
      NumericVector difprops1 = (parprops1 - nparprops1);
      NumericVector difprops2 = (parprops2 - nparprops2);
      
      tol = sqrt( std::inner_product(difprops1.begin(), difprops1.end(), difprops1.begin(), 0.0) +
                  std::inner_product(difprops2.begin(), difprops2.end(), difprops2.begin(), 0.0) +
                  abs(as<double>(params["prob0"]) - as<double>(newparams["prob0"])) );
      
      params = clone(newparams);
      
      steps = steps+1;
      
      // If one of the populations have all samples, leave the loop
      if (as<double>(newparams["prob0"]) == 1 || as<double>(newparams["prob0"]) == 0){
         break;
      }
   }
      

   //get last values to compute likelihood of the complete inversion model
   
   r1 = as<double>(params["prob0"]) * getNumericVectorfromStringVector( VectortoOrderedMap(as<NumericVector>(params["props1"])), inds );
   r2 = (1 - as<double>(params["prob0"])) * getNumericVectorfromStringVector( VectortoOrderedMap(as<NumericVector>(params["props2"])), inds );

   double LoglikeMix = sum(log(r1 + r2));
   NumericVector R1 = r1/(r1 + r2);
   R1.attr("names") = inds;
         
         
   return  List::create(Named("logNoLD") = LoglikeRecomb,  
                        Named("prob") = as<double>(params["prob0"]),
                        Named("r1") = R1);
   

   
}   






 /***  
params <- list(r1 = r1, r2 = r2, props1 = propsRecomb, props2 = propsLink,  prob0 = prob0, inds = inds)
#EM loop
#control while loop
   tol<- 1
   MINTOL<- .000000001
   steps<-1
   
   while(tol > MINTOL & steps <= maxSteps)
   {
      
      
## El código de updateModel se puede poner directamente dentro del bucle
      newparams <- updateModel(params, recombFreq, linkageFreq)
         
         
         tol<-sqrt((params$props1 - newparams$props1)%*%(params$props1 - newparams$props1) +
            (params$props2 - newparams$props2)%*%(params$props2 - newparams$props2) +
            abs(params$prob0 - newparams$prob0) )
         
         params <- newparams
         steps <- steps+1
      
## If one of the populations have all samples, leave the loop
      if (newparams$prob0 == 1 | newparams$prob0 == 0){
         break
      }
   }
   

   
   
#get last values to compute likelihood of the complete inversion model
   r1 <- params$prob0*params$props1[params$inds]
   r2 <- (1 - params$prob0)*params$props2[params$inds]
   LoglikeMix <- sum(log(r1 + r2))
      R1 <- r1/(r1 + r2)
      
      ans <- list(logNoLD = LoglikeRecomb,
                  prob = params$prob0,
                  r1 = R1)
      
      ans
  ***/      
   
   
   
   
   



// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically 
// run after the compilation.
//

/*** R
timesTwo(42)

a <- cLDmixtureModel(dat)


library(microbenchmark)
res <- microbenchmark( a <- cLDmixtureModel(dat),
                       ans1 <- LDmixtureModel(dat),
                       times = 1L, unit = "s")
print(summary(res)[, c(1:7)],digits=3)



ans1$logNoLD
a$logNoLD
ans1$prob
a$prob
ans1$r1
a$r1


*/
