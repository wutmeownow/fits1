#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TRandom2.h"
#include "TROOT.h"


// fit1.C
// entries is the number of random samples filled into the histogram
void fit1a(int entries=1000, int ntrials=10000, bool save=false) {
   //Simple histogram fitting examples
  gROOT->Reset();  // useful to reset ROOT to a cleaner state

  TFile *tf=0;
  if (save) tf=new TFile("histo.root","recreate");

  TGraph *g = new TGraph();
  g->SetTitle(";Red Chi Sqr;Chi Sqr Probability");
  // g->SetMarkerStyle(20); // filled circle
  // g->SetMarkerSize(1.2);

  TH1F *chisqrprobHist = new TH1F("chisqrprobHist", "chi sqr prob;prob;freq", 150, 0, 1);
  TH1F *meanHist = new TH1F("meanHist", "Mean (Par[1]);mean;freq", 100, 48, 52);
  TH1F *redchi2Hist = new TH1F("redchi2Hist", "Reduced Chi Sqr;Reduced chi sqr;freq", 100, 0, 2.5);
  TH1F *randomHist1 = new TH1F("randomHist1", "Random Histogram", 100, 0, 100);
  TRandom2 *generator=new TRandom2(0);  // parameter == seed, 0->use clock

  for (int i=0; i<ntrials; i++){
    randomHist1->Reset();   // reset histogram bin content to 0
    for (int i=0 ; i<entries ; i++){
        randomHist1->Fill(generator->Gaus(50,10)); // params: mean, sigma
      }
    randomHist1->Fit("gaus","Q");
    // do more stuff
    TF1 *fitfunc = randomHist1->GetFunction("gaus");
    // cout << "\nFit Params and errors" << endl;
    // cout << fitfunc->GetParameter(0) << " +- " << fitfunc->GetParError(0) << endl;
    // cout << fitfunc->GetParameter(1) << " +- " << fitfunc->GetParError(1) << endl;
    // cout << fitfunc->GetParameter(2) << " +- " << fitfunc->GetParError(2) << endl;
    // cout << "Fit Probability: " << fitfunc->GetProb() << endl; // returns chi^2 p-value
    // cout << "Fit Chi Square: " << fitfunc->GetChisquare() << endl; // returns chi^2
    // cout << "Fit NDF: " << fitfunc->GetNDF() << endl; // returns # DoF
    // cout << "Fit Reduced Chi Square: " << fitfunc->GetChisquare()/fitfunc->GetNDF() << endl; // returns # DoF
    redchi2Hist->Fill(fitfunc->GetChisquare()/fitfunc->GetNDF()); // add reduced chi sqr of this fit to histogram
    meanHist->Fill(fitfunc->GetParameter(1)); // add mean parameter to hist
    chisqrprobHist->Fill(fitfunc->GetProb()); // add fit probability to hist
    g->SetPoint(i, fitfunc->GetChisquare()/fitfunc->GetNDF(), fitfunc->GetProb()); // fill graph with prob vs red chi sqr
  }

  // simple fits may be performed automatically
  // gStyle->SetOptFit(111);  // show reduced chi2 and params
  // gStyle->SetOptFit(1111); // show reduced chi2, probability, and params
  // randomHist1->Fit("gaus");  
  // randomHist1->DrawCopy("e");  // "e" shows bin errors
  // Using DrawCopy vs Draw allows us to delete the original histogram
  // without removing it from the display.  If we save the histogran to a
  // file and close the file, it will be deleted from memory.


  // Above we used a built in function, gaus, in the fit
  // This function will be associated with the histogram
  // and may be retrieved to get parameter information
  // Refer to http://root.cern.ch/root/html/TF1.html
  // for a complete list of TF1 methods

  // TF1 *fitfunc = randomHist1->GetFunction("gaus");
  // cout << "\nFit Params and errors" << endl;
  // cout << fitfunc->GetParameter(0) << " +- " << fitfunc->GetParError(0) << endl;
  // cout << fitfunc->GetParameter(1) << " +- " << fitfunc->GetParError(1) << endl;
  // cout << fitfunc->GetParameter(2) << " +- " << fitfunc->GetParError(2) << endl;
  // cout << "Fit Probability: " << fitfunc->GetProb() << endl; // returns chi^2 p-value

  TCanvas *tc1 = new TCanvas("c1","Experiments Results",800,600);
  tc1->Divide(2,2);

  tc1->cd(1); redchi2Hist->Draw();

  tc1->cd(2); meanHist->Draw();

  tc1->cd(3); chisqrprobHist->Draw();

  tc1->cd(4); g->Draw("AP");

  tc1->Update();
  tc1->SaveAs("result1.pdf");


  if (save) {
    tf->Write();
    tf->Close();
  }

  // clean up
  delete chisqrprobHist;
  delete meanHist;
  delete redchi2Hist;
  delete randomHist1;
  delete g;
  delete generator;
  delete tc1;
  cout << "Use .q to exit root" << endl;
}
