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

double calcChi2(TH1* h, TF1* f) {
    double chi2 = 0.0;
    int nbins = h->GetNbinsX();

    for (int i = 1; i <= nbins; ++i) {
        double x = h->GetBinCenter(i);
        double y = h->GetBinContent(i);
        double err = h->GetBinError(i);
        if (err <= 0) continue; // avoid division by zero

        double yfit = f->Eval(x);
        chi2 += std::pow((y - yfit) / err, 2);
    }

    return chi2;
}


// fit1.C
// entries is the number of random samples filled into the histogram
void fit1d(int ntrials=10000) {
  //Simple histogram fitting examples
  gROOT->Reset();  // useful to reset ROOT to a cleaner state

  // Open the ROOT files
  auto file25 = new TFile("histo25.root");
  auto h25 = (TH1F*) file25->Get("randomHist1");
  auto file1k = new TFile("histo1k.root");
  auto h1k = (TH1F*) file1k->Get("randomHist1");

  // Fit with Gaussian using NLL minimization
  h25->Fit("gaus","L");
  TF1 *fitfunc = h25->GetFunction("gaus");

  // Fit 1k histo with Gaussian using chi2 minimization
  h1k->Fit("gaus", "");
  TF1 *fitfunc2 = h1k->GetFunction("gaus");

  // loop through bins and calculate NLL
  int nbins = h25->GetNbinsX();
  double nll = calcNLL(nbins, h25, fitfunc);

  // get chi2 of the 1k fit
  double chi2 = fitfunc2->GetChisquare();

  // mean and its error from NLL fit
  double mean_err = fitfunc->GetParError(1);
  double mean = fitfunc->GetParameter(1);

  // mean and its error from chi2 fit
  double mean1k_err = fitfunc2->GetParError(1);
  double mean1k = fitfunc2->GetParameter(1);

  // define graph for NLL versus mean value and chi2 vs mean value
  TGraph *g = new TGraph();
  g->SetTitle(";Mean;-2lnL");
  TGraph *g2 = new TGraph();
  g2->SetTitle(";Mean;Chi2");

  // increment nll on both sides until nll+4 is reached
  double maximum_nll = nll+4.;
  double nll_upper=nll; // nll from increasing mean
  double nll_lower=nll; // nll from decreasing mean
  double step = mean_err/10; // step in mean
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

  
  // increment chi2 on both sides until chi2+2 is reached
  double maximum_chi2 = chi2+2.;
  double chi2_upper=chi2; // chi2 from increasing mean
  double chi2_lower=chi2; // chi2 from decreasing mean
  step = mean1k_err/10; // step in mean
  i = 1; // how many times to add step
  n_points = 0;
  g2->SetPoint(n_points, mean1k,chi2); // add first point
  n_points += 1; // number of points in graph

  while (chi2_lower<maximum_chi2 && chi2_upper<maximum_chi2) {
    fitfunc2->SetParameter(1,mean1k+step*i);
    chi2_upper = calcChi2(h1k, fitfunc2);
    g2->SetPoint(n_points, mean1k+step*i,chi2_upper);
    n_points += 1;

    fitfunc2->SetParameter(1,mean1k-step*i);
    chi2_lower = calcChi2(h1k, fitfunc2);
    g2->SetPoint(n_points, mean1k-step*i,chi2_lower);
    n_points += 1;
    i+=1;
  }


  TCanvas *tc1 = new TCanvas("c1","Experiments Results",800,600);
  tc1->Divide(1,2);
  g->Sort();
  g2->Sort();
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

  tc1->cd(2); g2->Draw("AL");

  TLine* line3 = new TLine(mean1k+1*mean1k_err, 50.7, mean1k+1*mean1k_err, 53);
  line3->SetLineColor(kRed);
  line3->SetLineWidth(1);
  line3->Draw("same");

  TLine* line4 = new TLine(mean1k-1*mean1k_err, 50.7, mean1k-1*mean1k_err, 53);
  line4->SetLineColor(kRed);
  line4->SetLineWidth(1);
  line4->Draw("same");

  tc1->Update();
  tc1->SaveAs("result4.pdf");


  // clean up
  // delete meannllHist;
  // delete meanchi2Hist;
  delete fitfunc;
  delete fitfunc2;
  delete h25;
  delete g;
  delete g2;
  cout << "Use .q to exit root" << endl;
}
