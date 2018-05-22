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
#include "tdrstyle.C"
    
using namespace std;

   
int main(int argc, char *argv[])
{

//gStyle->SetOptStat(0);    
/*gStyle->SetCanvasColor(-1);
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
*/

    //setTDRStyle();
    Modified_tdr_style();
//gROOT->ForceStyle();
    
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
    TKey *key2;
    uint32_t keyCount = 0;
	int objcounter=1;
	int histolimit =0;

    /*while ((key2 = (TKey*)nextkey())) {
        keyCount++;
    }*/

    cout << " nr of overshoots " << keyCount << endl;

    while ((key = (TKey*)nextkey())) {

        if(false) 
            cout << "nonsense" << endl;
      	//if(histolimit != limit-1)
	//	histolimit++;
          else
          {
	  TObject *obj = key->ReadObj();
      
      TH1F* empty = NULL;

      if ( obj->IsA()->InheritsFrom( "TH1" ) ) {
	
	std::cout << "Found object n: " << objcounter << " Name: " << obj->GetName() << " Title: " << obj->GetTitle()<< std::endl;
	++objcounter;
        continue;
	//if (strstr(obj->GetTitle(),"470116592")!=NULL)
	//  continue;

	C->Clear();
	TH1F* h = (TH1F*)key->ReadObj();
	TH1F* hoff = (TH1F*)h->Clone();
        for(uint32_t bx=0; bx<h->GetNbinsX(); bx++ )
        {
            hoff->SetBinContent(bx+1,128);
        }

        TLegend leg(0.5,0.6,0.9,0.9);


	TH1F* hd = (TH1F*) f->Get(dir[3]+"/"+obj->GetName());
	
	if(hd!=0){

          for(uint32_t rh=0; rh<hd->GetNbinsX(); rh++ )
          {
            vector<float> rms;
            if(rh == 0 || rh%128==0)
            {
                for(uint32_t a = 0; a<128; a++ )
                {
                    rms.push_back(hd->GetBinContent(rh+1+a));
                }
                std::sort(rms.begin(), rms.end());
                float sum = 0;
                float av = 0;
                float RMSstrips = rms.size()-26; //20%
                for(uint32_t r=0; r<RMSstrips; r++)
                {
                    sum +=rms.at(r);
                }
                av = sum/RMSstrips;

                float rmsSum =0;
                float rmsRes =0;
                for(uint32_t v=0; v<RMSstrips; v++)
                {
                    rmsSum += pow(rms.at(v)-av,2);
                }

                rmsRes = sqrt(rmsSum/RMSstrips);
                cout << "rms " << rmsRes << endl;
            }
          }
          hd->SetTitle("");
	  hd->SetXTitle("strip");
          hd->SetYTitle("charge [ADC]");
	  hd->SetLineWidth(2);
	  //hd->SetLineStyle(2);
	  hd->SetLineColor(6);
	  hd->SetMinimum(-300);
	  hd->SetMaximum(1000);
	  hd->GetYaxis()->SetTitleOffset(1.7);
	  leg.AddEntry(hd,"Raw digis              ","l");
    //Modified_tdr_style();
	  hd->Draw("hist p l");
	}

        if(layers>1)
        {
        //h->Add(hoff);
	h->SetLineWidth(2);
        h->SetTitle("");
	h->SetXTitle("strip");
	h->SetYTitle("charge [ADC]");
	leg.AddEntry(h,"Pedestal subtracted digis","l");
	h->Draw("hist p l same");
        }

	f->cd();


	//f->cd(dir[1]);
	TH1F* hb = (TH1F*) f->Get(dir[1]+"/"+obj->GetName());

	
	if(hb!=0 && layers >2){

          //hb->Add(hoff);
          for(uint32_t bl=0; bl<hb->GetNbinsX(); bl++ )
          {
            if(bl == 0 || bl%128==0)
                cout << "baseline " << hb->GetBinContent(bl+1) << endl;
          }
          
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
    
	
	//C->Update();
//	fo->cd();
//        C->Write();
	C->SaveAs(TString("event"+slimit+"_layer"+slayers+".eps"));
	C->SaveAs(TString("event"+slimit+"_layer"+slayers+".root"));

       break;	
	
      }
    }  
  }
}
