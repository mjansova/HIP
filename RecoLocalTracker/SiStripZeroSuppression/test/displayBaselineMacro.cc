#include "TH1F.h"
#include "TCanvas.h"
#include "TObject.h"
#include "TFile.h"
#include "TPaveStats.h"
#include "TGraphErrors.h"
#include "TGaxis.h"
#include "TROOT.h"
#include "TF1.h"
#include "TLegend.h"
#include "TKey.h"
#include "TClass.h"
#include "TROOT.h"
#include "TStyle.h"


#include "iostream"
#include "vector"
#include "math.h"
#include "map"

    
using namespace std;

   
int main(int argc, char *argv[])
{

gStyle->SetOptStat(0);    
gStyle->SetCanvasColor(-1);
gStyle->SetPadColor(-1);
gStyle->SetFrameFillColor(-1);
gStyle->SetHistFillColor(-1);
gStyle->SetTitleFillColor(-1);
gStyle->SetFillColor(-1);
gStyle->SetFillStyle(4000);
gStyle->SetStatStyle(0);
gStyle->SetTitleStyle(0);
gStyle->SetCanvasBorderSize(0);
gStyle->SetFrameBorderSize(0);
gStyle->SetLegendBorderSize(0);
gStyle->SetStatBorderSize(0);
gStyle->SetTitleBorderSize(0);

gROOT->ForceStyle();

    if(argc != 4)
    {
        cout << "incorrect number of coommand line parametres!" << endl;
        return -1;
    }

   TString file = argv[1];
   string slimit = argv[2];
   int limit = stoi(slimit);
   string slayers = argv[3];
   int layers = stoi(slayers);


   TFile *f;//, *fo;
  TString BaseDir;
  TString dir[4];
  TString fullPath, title, subDet, genSubDet;
  TCanvas *C;
    C = new TCanvas();

    f = new TFile(file);
//    fo = new TFile(ofile, "RECREATE");
    //BaseDir="DQMData/Results/SpyChannel/";
    dir[0]="baselineAna/ProcessedRawDigis";
    dir[1]="baselineAna/Baseline";
    dir[2]="baselineAna/Clusters";
    dir[3]="baselineAna/RawDigis";
    f->cd();
//	fo->Write();
//	fo->Close();
    f->cd(dir[0]);
    
    TIter nextkey(gDirectory->GetListOfKeys());
    TKey *key;
	int objcounter=1;
	int histolimit =0;
    while ((key = (TKey*)nextkey())) {
      	if(histolimit != limit-1)
		histolimit++;
          else
          {
	  TObject *obj = key->ReadObj();
      
      TH1F* empty = NULL;

      if ( obj->IsA()->InheritsFrom( "TH1" ) ) {
	
	std::cout << "Found object n: " << objcounter << " Name: " << obj->GetName() << " Title: " << obj->GetTitle()<< std::endl;
	++objcounter;
	//if (strstr(obj->GetTitle(),"470116592")!=NULL)
	//  continue;

	C->Clear();
	TH1F* h = (TH1F*)key->ReadObj();

        TLegend leg(0.5,0.6,0.9,0.9);


	TH1F* hd = (TH1F*) f->Get(dir[3]+"/"+obj->GetName());
	
	if(hd!=0){

          hd->SetTitle("");
	  hd->SetXTitle("StripNumber");
          hd->SetYTitle("Charge (ADC counts)");
	  hd->SetLineWidth(2);
	  //hd->SetLineStyle(2);
	  hd->SetLineColor(6);
	  leg.AddEntry(hd,"Raw digis              ","l");
	  hd->Draw("hist p l");
	}

        if(layers>1)
        {
	h->SetLineWidth(2);
        h->SetTitle("");
	h->SetXTitle("StripNumber");
	h->SetYTitle("Charge (ADC counts)");
	leg.AddEntry(h,"Pedestal subtracted digis","l");
	h->Draw("hist p l same");
        }

	f->cd();


	//f->cd(dir[1]);
	TH1F* hb = (TH1F*) f->Get(dir[1]+"/"+obj->GetName());

	
	if(hb!=0 && layers >2){

	  hb->SetLineWidth(2);
	  hb->SetLineStyle(2);
	  hb->SetLineColor(2);
	  leg.AddEntry(hb,"Baselines","l");
	  hb->Draw("hist p l same");

	}
	
	f->cd();
	//f->cd(dir[1]);
	TH1F* hc = (TH1F*) f->Get(dir[2]+"/"+obj->GetName());
	
	if(hc!=0 && layers>3){

	  hc->SetLineWidth(2);
	  hc->SetLineStyle(2);
	  hc->SetLineColor(3);
	  leg.AddEntry(hc,"Clusters","l");
	  hc->Draw("hist p l same");
	}
	
          for(uint32_t l=0; l< 4-layers;l++)
          {
	      leg.AddEntry(empty,"","");
          }

        

	//else
	  //std::cout << "not found " << obj->GetName()<< std::endl;


      leg.Draw();
    
	
	C->Update();
//	fo->cd();
//        C->Write();
	
	C->SaveAs(TString("event"+slimit+"_layer"+slayers+".eps"));

       break;	
	
      }
    }  
  }
}
