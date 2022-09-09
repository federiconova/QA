
int read_waveform_light_levels(){

  TFile *f = new TFile("file_waveform_light_levels.root","RECREATE");

  ifstream infile;
  infile.open("C3--R6091_V1100_leddriver8ns_400nm_p3--00111.txt");
  if(infile.fail()) { 
    cout << "error" << endl; 
    return 1;
  } 

  double t, V;
  std::vector<double> ts, Vs;
  while(!infile.eof()) {
    infile >> t;
    infile >> V;
    ts.push_back(t);
    Vs.push_back(V);
  } 
  infile.close();

  TGraph * g = new TGraph(ts.size(), &ts[0], &Vs[0]);
  g->SetName("waveform");
  g->SetTitle("waveform");
  g->GetXaxis()->SetTitle("time [s]");
  g->GetYaxis()->SetTitle("voltage [V]");

  g->Write();
  f->Close();

  exit(0);
  return 1;

}
