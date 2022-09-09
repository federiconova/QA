
int plot_voltages(){

  TFile *in = new TFile("file_PMT_voltages.root","READ");
  TFile *out = new TFile("out_PMT_voltages.root","RECREATE");

  Int_t voltage;
  float charge;
  double resistance = 50.;
  double eElectron=1.602177e-19;

  TTree * t = (TTree*)in->Get("PMT");

  t->SetBranchAddress("voltage",&voltage);
  t->SetBranchAddress("charge",&charge);

  const int nv=7;
  int voltages[nv]={900,1000,1100,1200,1300,1400,1500};

  TH1F * h[nv];
  for(int j=0; j<nv; j++){
    h[j] = new TH1F(Form("h_%d",voltages[j]),Form("V = %d; charge",voltages[j]),100,1,-1);
    h[j]->SetLineColor(j+1);
    h[j]->SetFillColor(j+1);
    h[j]->SetLineWidth(2);
  }

  TH1F * h_charge_vs_V = new TH1F("h_charge_vs_V","h_charge_vs_V;voltage;charge",nv,voltages[0]-50,voltages[nv-1]+50);
  h_charge_vs_V->SetMarkerStyle(8);

  TH1F * h_n0_vs_V = new TH1F("h_n0_vs_V","h_n0_vs_V;voltage;#left(#frac{mean}{sigma}#right)^{2}",nv+2,voltages[0]-100,voltages[nv-1]+100);
  h_n0_vs_V->SetMarkerStyle(8);

  TGraph * g_G_vs_V;

  for(int i=0; i<t->GetEntries(); i++){
    t->GetEntry(i);
    for(int j=0; j<nv; j++){
      if( voltages[j] == voltage ){
	h[j]->Fill(fabs(charge)/resistance);
      }
    }
  }

  double Gs[nv];
  double Vs[nv];
  double mean, sigma;
  for(int j=0; j<nv; j++){
    mean = h[j]->GetMean();
    sigma = h[j]->GetRMS();
    TFitResultPtr r = h[j]->Fit("gaus","S");
    //    cout << " mean " << mean << " or " << r->Parameter(1) << " sigma " << sigma << " or " << r->Parameter(2) << endl;
    mean = r->Parameter(1);
    sigma = r->Parameter(2);
    h_charge_vs_V->SetBinContent(h_charge_vs_V->GetXaxis()->FindBin(voltages[j]),mean);
    h_charge_vs_V->SetBinError(h_charge_vs_V->GetXaxis()->FindBin(voltages[j]),sigma);
    h_n0_vs_V->SetBinContent(h_charge_vs_V->GetXaxis()->FindBin(voltages[j]),pow(mean/sigma,2));
    Gs[j]=pow(sigma,2)/(mean*eElectron);
    Vs[j]=voltages[j];
  }
  
  g_G_vs_V = new TGraph(nv,Vs,Gs);
  g_G_vs_V->SetName("g_G_vs_V");
  g_G_vs_V->SetTitle("g_G_vs_V;V;#frac{sigma^{2}}{mean}");
  g_G_vs_V->SetMarkerStyle(8);

  TH1F * empty = new TH1F("empty","empty;charge",100,-1e-11,1e-9);

  out->cd();
  for(int j=0; j<nv; j++){
    h[j]->Write();
  }
  h_charge_vs_V->Write();
  h_n0_vs_V->Write();
  g_G_vs_V->Write();
  empty->Write();
  out->Close();
  in->Close();
  

  exit(0);
  return 1;
}
