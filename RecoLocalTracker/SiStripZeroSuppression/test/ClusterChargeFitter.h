#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"


//--------------------------------------------------------------
// param is a vector which contain the 6 parameters of the fit
// fixParam is an option to fix the mean and rms of landau
//   only the normalization will be fitted
//   the input values are taken from a global fit (more stat)
// returned value: fraction + uncertainty
//--------------------------------------------------------------



pair<float,float> ClusterCharge1DFitter(TH1D* histo, vector<float>& param, bool fixParam = false){

	//------------------------
	// Create the fit function
	//------------------------
	
	// Start @ 30 to avoid the bias in the first bins
	// To not go beyound 50 because the tail is not well model and would bias the off-track cluster measurement
	TF1* fit = new TF1("SumLandau","landau(0)+landau(3)",30,500);
	
	//normalization
	fit->SetParameter(0,37);
	fit->SetParLimits(0,1,100000);
	//mean
	fit->SetParameter(1,200);
	fit->SetParLimits(1,180,250);
	//width
	fit->SetParameter(2,70);
	fit->SetParLimits(2,30,80);
	//normalization
	fit->SetParameter(3,160);
	fit->SetParLimits(3,1,100000);
	//mean
	fit->SetParameter(4,60);
	fit->SetParLimits(4,40,80);
	//width
	fit->SetParameter(5,15);
	fit->SetParLimits(5,13,20);

	if(fixParam){
		//mean
		fit->FixParameter(1,160); //uncertainty  <1 (0.6)
		//width
		fit->FixParameter(2,20);  // uncertainty < 1 (0.3)
		//mean
		fit->FixParameter(4,59);  // uncertainty < 1 (0.3)
		//width
		fit->FixParameter(5,14); // uncertainty < 1 (0.2
	}

	//Fit the histo
	TFitResultPtr res = histo->Fit(fit,"RS");
        //GetCovarianceMatrix (Matrix &mat)

	//Create a new function for the off-track contribution
	TF1* fOff = new TF1("fOff","landau",0,500);
	fOff->SetParameter(0,fit->GetParameter(3));
	fOff->SetParameter(1,fit->GetParameter(4));
	fOff->SetParameter(2,fit->GetParameter(5));
	float IntegOff = fOff->Integral(0,300);
	//rescale according to the range and bin width (assume constant binning)
	IntegOff/=fit->GetXaxis()->GetBinWidth(1);
	cout<<IntegOff<<" "<<histo->GetEntries()<<endl;	
	cout<<histo->Integral()<<endl;
	float fraction = 0;
	if(histo->GetEntries()!=0) fraction = IntegOff/histo->GetEntries();
	histo->Draw();
	fOff->Draw("same");
	cout<<"fraction = "<<fraction<<endl;
	
	//Propagate the uncertainty on the normalization of the 1st landau
	float RelUncert = 0;
	if(fraction!=0){
		cout<<res.Get()->GetErrors()[0]/fit->GetParameter(0)<<endl;
		if(fit->GetParameter(0)!=0) RelUncert = res.Get()->GetErrors()[0]/fit->GetParameter(0);
	}
	
	
	//fill the parameters
	for(int i=0;i<6;i++)
		param.push_back(fit->GetParameter(i));

	return std::pair<float,float>(make_pair(fraction,RelUncert*fraction));
}

///*


TH1F* ClusterChargePlot(TH2F* histo, bool fixParam = false){
	TH1F* hRes = new TH1F("hOffFraction","Fraction of off-track clusters",histo->GetNbinsX(),histo->GetXaxis()->GetXmin(),histo->GetXaxis()->GetXmax());
	
	//Histo of the fitted parameters
	TH1F** hParam = new TH1F*[6];
	for(int i=0;i<6;i++){
		stringstream name;
		name<<"hParam_"<<i;
		hParam[i] = new TH1F(name.str().c_str(),"Fraction of off-track clusters",histo->GetNbinsX(),histo->GetXaxis()->GetXmin(),histo->GetXaxis()->GetXmax());
	}
	//
	
	for(int i=1;i<histo->GetNbinsX();i++){
	//for(int i=74;i<75;i++){
		stringstream name;
		name<<histo->GetName()<<"_px"<<"_"<<i;
		TH1D* proj = histo->ProjectionY(name.str().c_str(),i,i);
		proj->Draw();
		//Call ClusterCharge1DFitter
		
		vector<float> param; // retrieve the parameters of the fit
		std::pair<float,float> res  = ClusterCharge1DFitter(proj, param, fixParam);
		hRes->SetBinContent(i, res.first);
		cout<<"error = "<<res.second<<endl;
		hRes->SetBinError(i, res.second);
	
		//fill histo of parameters
		for(int ib=0;ib<6;ib++){
			cout<<"param "<<ib<<" = "<<param[ib]<<endl;
			hParam[ib]->SetBinContent(i,param[ib]);
		}
	}
	/*	
	TCanvas* cParam = new TCanvas();
	cParam->Divide(3,2);
	for(int i=0;i<6;i++){
		cParam->cd(i+1);
		hParam[i]->Draw();
	}
	*/
	return hRes;
}
//*/

void test(){
	gStyle->SetOptStat(0);
	//Retrieve the 2D plots
	//TFile* fin = new TFile("/opt/sbg/scratch1/cms/mjansova/store/tmp/VR4/sEventFileModules_1_100_part_5_l_1.root_analysis.root","READ");
	//TFile* fin = new TFile("/opt/sbg/cms/ui6_data1/mjansova/CMSSW_8_0_7_patch1/src/RecoLocalTracker/SiStripZeroSuppression/test/sEventFileModules_fewmod_part_5_l_1.root_analysis.root","READ");
	TFile* fin = new TFile("/opt/sbg/cms/ui6_data1/mjansova/CMSSW_8_0_7_patch1/src/RecoLocalTracker/SiStripZeroSuppression/test/sEventFileModules_fewmod_part_5_l_1.root_analysis.root","READ");
	TH2F* h2D = (TH2F*) fin->Get("BXvsCHall");
	//TH2F* h2D = (TH2F*) fin->Get("BXvsCH");
	//TH2F* h2D = (TH2F*) fin->Get("BXvsCH2");
	cout<<h2D<<endl;
	cout<<h2D->GetNbinsX()<<endl;
	h2D->Draw();
	//return;
	TH1F* h1 = (TH1F*) fin->Get("normal clus average charge");
	cout<<h1<<endl;

	//ClusterCharge1DFitter(h1);
	//h1->Draw();
	TCanvas* c1 = new TCanvas("c1");
	bool fixParam  = true;
	TH1F* hres = ClusterChargePlot(h2D, fixParam);
	hres->GetXaxis()->SetTitle("BX");
	hres->GetXaxis()->SetRangeUser(-20,30);
	hres->GetYaxis()->SetTitle("Fraction of off-track clusters");
	c1->cd();
	//hres->Draw("E1");
	hres->Draw();
}
