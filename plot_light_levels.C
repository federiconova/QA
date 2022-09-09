
int plot_light_levels(){

  TFile *in = new TFile("file_PMT_light_levels.root","READ");
  TFile *out = new TFile("out_PMT_light_levels.root","RECREATE");

  Int_t point;
  float charge;
  double resistance = 50.;
  double eElectron=1.602177e-19;

  TTree * t = (TTree*)in->Get("PMT");

  t->SetBranchAddress("point",&point);
  t->SetBranchAddress("charge",&charge);

  const int np=5;
  int points[np]={1,2,3,4,5};

  TH1F * h[np];
  for(int j=0; j<np; j++){
    h[j] = new TH1F(Form("h_%d",points[j]),Form("p = %d; charge",points[j]),100,0,1e-10);
    h[j]->SetLineColor(j+1);
    h[j]->SetFillColor(j+1);
    h[j]->SetLineWidth(2);
  }

  TH1F * h_charge_vs_V = new TH1F("h_charge_vs_V","h_charge_vs_V;point;charge",np+2,points[0]-1,points[np-1]+1);
  h_charge_vs_V->SetMarkerStyle(8);

  TH1F * h_n0_vs_V = new TH1F("h_n0_vs_V","h_n0_vs_V;point;#left(#frac{mean}{sigma}#right)^{2}",np+2,points[0]-1,points[np-1]+1);
  h_n0_vs_V->SetMarkerStyle(8);

  TGraphErrors * g_Gn0_vs_n0;

  for(int i=0; i<t->GetEntries(); i++){
    t->GetEntry(i);
    for(int j=0; j<np; j++){
      if( points[j] == point ){
	h[j]->Fill(fabs(charge)/resistance);
      }
    }
  }

  double means[np];
  double meansE[np];
  double n0s[np];
  double n0sE[np];
  double mean, sigma;
  for(int j=0; j<np; j++){
    h[j]->Scale(1./h[j]->Integral(),"nosw2");
    mean = h[j]->GetMean();
    sigma = h[j]->GetRMS();
    h_charge_vs_V->SetBinContent(h_charge_vs_V->GetXaxis()->FindBin(points[j]),mean);
    h_charge_vs_V->SetBinError(h_charge_vs_V->GetXaxis()->FindBin(points[j]),sigma);
    h_n0_vs_V->SetBinContent(h_charge_vs_V->GetXaxis()->FindBin(points[j]),pow(mean/sigma,2));
    means[j]=mean/eElectron;
    meansE[j]=sigma/eElectron;
    n0s[j]=pow(mean/sigma,2);
    n0sE[j]=mean/sigma;
  }
  
  g_Gn0_vs_n0 = new TGraphErrors(np,n0s,means,n0sE,meansE);
  g_Gn0_vs_n0->SetName("g_Gn0_vs_n0");
  g_Gn0_vs_n0->SetTitle("g_Gn0_vs_n0;#left(#frac{mean}{sigma}#right)^{2};#frac{mean}{e}");
  g_Gn0_vs_n0->SetMarkerStyle(8);

  out->cd();
  for(int j=0; j<np; j++){
    h[j]->Write();
  }
  h_charge_vs_V->Write();
  h_n0_vs_V->Write();
  g_Gn0_vs_n0->Write();
  out->Close();
  in->Close();
  

  exit(0);
  return 1;
}
