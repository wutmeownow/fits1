#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TRandom2.h"
#include <cmath>
#include "TROOT.h"
#include "TMath.h"
#include "TLine.h"

double calcNLL(int nbins, TH1F* h, TF1* f) {
  double nll = 0;
  for (int bin = 1; bin <= nbins; bin++) {
    double x = h->GetBinCenter(bin);
    double y = h->GetBinContent(bin);
    double lambda = f->Eval(x);

    nll += y*log(lambda)-lambda-TMath::LnGamma(y+1);
  }
  nll = -2.*nll;
  // cout << "Fit NLL: " << nll << endl;
  return nll;
}


// fit1.C
// entries is the number of random samples filled into the histogram
void fit1d(int ntrials=10000) {
   //Simple histogram fitting examples
  gROOT->Reset();  // useful to reset ROOT to a cleaner state

  // Open the ROOT file
  auto file25 = new TFile("histo25.root");
  auto h25 = (TH1F*) file25->Get("randomHist1");

  // Fit with Gaussian using NLL minimization
  h25->Fit("gaus","L");
  TF1 *fitfunc = h25->GetFunction("gaus");

  // loop through bins and calculate NLL
  int nbins = h25->GetNbinsX();
  double nll = calcNLL(nbins, h25, fitfunc);

  // mean and its error from this fit
  double mean_err = fitfunc->GetParError(1);
  double mean = fitfunc->GetParameter(1);

  // define graph for NLL versus mean value
  TGraph *g = new TGraph();
  g->SetTitle(";Mean;NLL");

  double maximum_nll = nll+4.;
  double nll_upper=nll; // nll from increasing mean
  double nll_lower=nll; // nll from decreasing mean
  double step = mean_err/4; // step in mean
  int i = 1; // how many times to add step
  int n_points = 0;
  g->SetPoint(n_points, mean,nll);
  n_points += 1; // number of points in graph

  while (nll_lower<maximum_nll && nll_upper<maximum_nll) {
    fitfunc->SetParameter(1,mean+step*i);
    nll_upper = calcNLL(nbins, h25, fitfunc);
    g->SetPoint(n_points, mean+step*i,nll_upper);
    n_points += 1;

    fitfunc->SetParameter(1,mean-step*i);
    nll_lower = calcNLL(nbins, h25, fitfunc);
    g->SetPoint(n_points, mean-step*i,nll_lower);
    n_points += 1;

    i+=1;
  }


  TCanvas *tc1 = new TCanvas("c1","Experiments Results",800,600);
  tc1->Divide(1,1);
  g->Sort();
  tc1->cd(1); g->Draw("AL");

  // Get y-axis maximum to determine line height
  // double ymax = g->GetMaximum();
  TLine* line = new TLine(mean+2*mean_err, 89, mean+2*mean_err, 93.5);
  line->SetLineColor(kRed);
  line->SetLineWidth(1);
  line->Draw("same");

  TLine* line2 = new TLine(mean-2*mean_err, 89, mean-2*mean_err, 93.5);
  line2->SetLineColor(kRed);
  line2->SetLineWidth(1);
  line2->Draw("same");

  tc1->Update();
  tc1->SaveAs("result4.pdf");


  // clean up
  // delete meannllHist;
  // delete meanchi2Hist;
  delete fitfunc;
  delete h25;
  delete g;
  cout << "Use .q to exit root" << endl;
}
