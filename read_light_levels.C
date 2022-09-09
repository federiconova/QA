#include "TSystemDirectory.h"

int read_light_levels(){

  TFile *f = new TFile("file_PMT_light_levels.root","RECREATE");

  string folder="PMT_data/LIGHT_LEVELS/Autosave/";
  //  string folder="PMT_data/GAIN_TEST/Autosave/";

  //  string file_name="C3--R6091_V1100_leddriver8ns_400nm_p1--00000.txt";
  //  string file_name="C4--R6091_V1300_led8ns_400nm_gain--00112.txt";
  //   C3--R6091_V1100_leddriver8ns_400nm_p3--00111.txt
  TString file_name_1="C3--R6091_V1100_leddriver8ns_400nm_p";
  TString file_name_2;
  TString file_name_3="--0";
  TString file_name_4;
  TString file_name_5=".txt";

  int n_lines_to_skip=5;
  double tmin = - 10.e-9;
  double tmax = 40.e-9;

  Int_t point;
  float charge;
  float charge_baseline;
  TTree *tree = new TTree("PMT","PMT data");
  tree->Branch("point",&point,"point/I");
  tree->Branch("charge",&charge,"charge/F");


  TSystemDirectory dir(folder.c_str(),folder.c_str()); 
  TList *files = dir.GetListOfFiles(); 
  if (files) { 
    TSystemFile *file; 
    TString fname; 
    TIter next(files); 
    while ((file=(TSystemFile*)next())) { 
      fname = file->GetName(); 
      if (!file->IsDirectory() && fname.EndsWith(".txt")) { 
	cout << fname.Data() << endl; 
	size_t ip1 = fname.Index("p");
	size_t ip2 = fname.Index("--0");
	file_name_2=fname(ip1+1,ip2-ip1-1);
	point = atoi(file_name_2.Data());
	size_t iN1 = ip2;
	size_t iN2 = fname.Index(".txt");
	file_name_4=fname(iN1+3,iN2-iN1-3);
	int N = atoi(file_name_4.Data());
	TString file_name = file_name_1 + file_name_2 + file_name_3 + file_name_4 + file_name_5;

	charge = 0.;
	charge_baseline = 0.;
	double tsignal = 0.;
	double tbaseline = 0.;
	float time = 0.;
	float time_old = 0.;
	float current;
	string line;
	ifstream myfile (folder+file_name.Data());
	if (myfile.is_open()) {
	  int counter = 0;
	  while ( getline (myfile,line) )
	    {
	      counter ++;
	      if( counter < n_lines_to_skip ) continue; // skip first lines
	      //if( counter > n_lines_to_skip + 1 ) break;
	      size_t iC = line.find(",");
	      string stime=line.substr(0,iC);
	      string scurrent=line.substr(iC+1);
	      time_old=time;
	      time = atof(stime.c_str());
	      current = atof(scurrent.c_str());
	      if( counter > n_lines_to_skip ){
		//		std::cout << " time " << time << " tmin " << tmin << " tmax " << tmax << " yes " << (bool)(time > tmin && time < tmax) << endl;
		if( time > tmin && time < tmax ){
		  charge += (time-time_old)*current;
		  tsignal += (time-time_old);
		}
		else{
		  charge_baseline += (time-time_old)*current;
		  tbaseline += (time-time_old);
		}
	      }
	    }
	  myfile.close();
	}

	if( tbaseline )
	  charge -= charge_baseline*tsignal/tbaseline;
	cout << " point " << point << " N " << N << " Q " << charge << " charge_baseline " << charge_baseline << " tsignal " << tsignal << " tbaseline " << tbaseline << endl;
	
	tree->Fill();
      }
    } 
  } 
  
  tree->Write();
  f->Close();

  exit(0);
  return 1;

}
