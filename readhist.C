// in root run me with
// [root] .L readhist.C
// [root] readhist()
//

void readhist(){
  auto file25 = new TFile("histo25.root");
  auto h25 = (TH1F*) file25->Get("randomHist1");
  auto file1k = new TFile("histo1k.root");
  auto h1k = (TH1F*) file1k->Get("randomHist1");
  auto file10k = new TFile("histo10k.root");
  auto h10k = (TH1F*) file10k->Get("randomHist1");
  auto tc = new TCanvas();
  tc->Divide(3,1);
  tc->cd(1);
  h25->Draw();
  tc->cd(2);
  h1k->Draw();
  tc->cd(3);
  h10k->Draw();
  tc->Update();  // force refresh (optional)
}
