# run with: python readhist.py

import ROOT as r

file25 = r.TFile("histo25.root")
h25 = file25.Get("randomHist1")
file1k = r.TFile("histo1k.root")
h1k = file1k.Get("randomHist1")
file10k = r.TFile("histo10k.root")
h10k = file10k.Get("randomHist1")

tc=r.TCanvas()
tc.Divide(3,1)
tc.cd(1)
h25.Draw()
tc.cd(2)
h1k.Draw()
tc.cd(3)
h10k.Draw()
tc.Update() # force refresh (optional)

input("hit return to exit")

