#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TVector3.h>
#include "./endian.h"
#include "MLFdatum.h"
using namespace std;

const int chmax=1152;
const int n_layer=2;
const int timezero=11800; 
int UNIXtime;
int evt;
int good_evt;
int good_evt_flag=0;//good event flag
int goodevt_thres=500; //threshold for good event
int tdc_thres = 20000; //threshold for tdc, for time difference and clustering
int cl_tdiff = 40;//cluster search time difference
int cl_search_lgth=1;//cluster search length
int coin_srch=1;//coincidence search area
int coin_tdiff= 50;//coincidence search time window
int overshoot_time=50;//over shoot correction time window
//int overshoot_time=150;//over shoot correction time window
vector<int> layer_s1;
vector<int> layer_s2;
//vector<int> nhit;
int nhit[n_layer];
int nhit_s1=0;
int nhit_s2=0;
int nhit_s1_mu=0;
int nhit_s2_mu=0;
vector<int> tdc_s1;
vector<int> tdc_s2;
int sizetmp=0;
int ch;
int tdc_tmp;

int x_def=11;
int y_def=11;
int x_def2=12;
int y_def2=11;

int main(int argc,char *argv[]){
    if ( argc < 2 ) {
        cout << "usage: ./make_tree [data filename] \n" << endl;;
        return 0;
    }
    string input=argv[1];
    string output=argv[1];
    output+=".root";
    int nevent = 0;
    if ( argc > 2 ) nevent= atoi( argv[2] );
    
    //
    ifstream chmap;
    chmap.open("chmap.dat");
    if( !chmap.is_open() ){

      cout<<"channel map cannot be opened."<<endl;
      return 0;
    }

    int daqch;
    vector<int> layer_list;
    vector<int> layer;

    vector<int> channel;

    vector<double> x;
    vector<double> y;
    vector<double> z;
    vector<int> posx;
    vector<int> posy;
    vector<int> posx_s1;
    vector<int> posy_s1;
    vector<int> posx_s2;
    vector<int> posy_s2;

    vector<int> tdc;
    vector<int> tdc1;
    vector<int> tdc2;
    vector<int> tdiff;
    vector<int> posxdif;
    vector<int> posydif;
    vector<int> hitdif;

    vector<int> mapx;
    vector<int> mapy;
    vector<int> mapfl;

    vector<int> posxnorm;
    vector<int> posynorm;

    int posxdiftmp=0;
    int posydiftmp=0;
    int hitdiftmp=0;

    int ip_tmp;
    int id_tmp;
    int layer_tmp;
    int sector_tmp;
    int ach;
    int dch;
    int index=0;
    double xpos;
    double ypos;
    double zpos;

    int time1=0;
    int time2=0;
    int timeflag=0;
    int td_tmp=0;

    int time1b=0;
    int time2b=0;
    int timeflagb=0;
    int td_tmpb=0;

    layer_list.clear();

    channel.clear();
    x.clear();    
    y.clear();    
    z.clear();

    posx.clear();
    posy.clear();
    posx_s1.clear();
    posy_s1.clear();
    posx_s2.clear();
    posy_s2.clear();

    tdc.clear();
    tdc1.clear();
    tdc2.clear();    
    tdiff.clear();

    posxdif.clear();
    posydif.clear();
    posxnorm.clear();
    posynorm.clear();
    hitdif.clear();

    mapx.clear();
    mapy.clear();
    mapfl.clear();

    while(!chmap.eof()&&index<chmax){

       chmap >> id_tmp >> ip_tmp >> dch >> daqch >> ach >> layer_tmp >> sector_tmp >> xpos >> ypos >> zpos;
       
       layer_list.push_back(layer_tmp);
       x.push_back(xpos);
       //x.insert(x.begin()+daqch,xpos);
       y.push_back(ypos+12*sector_tmp);
       //y.insert(y.begin()+daqch,ypos+12*sector_tmp);
       z.push_back(zpos);
       index++;

    }
    
    TFile *fpout=new TFile(output.c_str(),"recreate");
    TTree *tree=new TTree("tree","tree");
    tree->Branch("evt",&evt,"evt/I");
    tree->Branch("gd_evt",&good_evt,"good_evt/I");
    tree->Branch("gfl",&good_evt_flag,"good_evt_flag/I");
    tree->Branch("UNIXtime",&UNIXtime,"UNIXtime/I");
    tree->Branch("layer",&layer,"layer/I");
    tree->Branch("ch",&channel);
    //tree->Branch("layer_s1",&layer_s1,"layer_s1/I");
    //tree->Branch("layer_s2",&layer_s2,"layer_s2/I");
    tree->Branch("nhit",nhit,Form("n_hit[%d]/I", n_layer));
    tree->Branch("nhit_s1",&nhit_s1,"nhit_s1/I");
    tree->Branch("nhit_s2",&nhit_s2,"nhit_s2/I");
    tree->Branch("nhit_s1_mu",&nhit_s1_mu,"nhit_s1_mu/I");
    tree->Branch("nhit_s2_mu",&nhit_s2_mu,"nhit_s2_mu/I");

    tree->Branch("posx",&posx);
    tree->Branch("posy",&posy);
    tree->Branch("posx_s1",&posx_s1);
    tree->Branch("posy_s1",&posy_s1);
    tree->Branch("posx_s2",&posx_s2);
    tree->Branch("posy_s2",&posy_s2);

    tree->Branch("tdc",&tdc);
    tree->Branch("tdc1",&tdc1);
    tree->Branch("tdc2",&tdc2);
    //tree->Branch("tdiff",&tdiff);


    tree->Branch("posxdif",&posxdif);
    tree->Branch("posydif",&posydif);
    tree->Branch("hitdif",&hitdif);

    tree->Branch("posxnorm",&posxnorm);
    tree->Branch("posynorm",&posynorm);

    tree->Branch("mapx",&mapx);
    tree->Branch("mapy",&mapy);
    tree->Branch("mapfl",&mapfl);

    FILE *fpin=fopen(input.c_str(),"rb");
    const int nbuf = 64* 1024;
    MLFdatum item[ nbuf ];
    int has_unprocessed_hit = 0;

    evt=0;
    good_evt=0;
    
    //channel map
    //S2D: 0->287
    //S1D: 288->575
    //S2U: 576->863
    //S1U: 864->1151

    int t_ref=0;

    int fl=0;
    
    while(1){
      int nitem = fread(&item,sizeof(MLFdatum),nbuf,fpin );
      
      for (int i=0; i<nitem; i++) {
	switch (item[i].GetType()) {
	case 0x22:
	  
	  ch=-1;
	  tdc_tmp=-9999;
	  ch = item[i].GetModule()*32 + item[i].GetCh();
	  tdc_tmp = item[i].GetTime();
	  
	  if(tdc_tmp>10000)continue;
	  

	  //if( TMath::Abs( tdc_tmp - t_ref ) < overshoot_time  || tdc_tmp<timezero+8000){//overshoot skip
	  //if( TMath::Abs( tdc_tmp - t_ref ) < overshoot_time ){//overshoot skip
	    //cout<<"overshoot!!"<<endl;
	    //fl=1;
	  //}

	  t_ref=tdc_tmp;

	  if(fl==0){

	    channel.push_back(ch);


	    layer.push_back(layer_list[ch]);
	    
	    posx.push_back(x[ch]);
	    posy.push_back(y[ch]);
	    //posr.push_back(TMath::Sqrt( (x[ch]-11.5)*(x[ch]-11.5)+(y[ch]-11.5)*(y[ch]-11.5) ) );
	    tdc.push_back(tdc_tmp);
	    
	    nhit[layer_list[ch]]++;
	    
	    if(layer_list[ch]==0){
	      
	      nhit_s1++;

	      if(tdc_tmp>timezero)nhit_s1_mu++;

	      posx_s1.push_back(x[ch]);
	      posy_s1.push_back(y[ch]);
	      tdc1.push_back(tdc_tmp);
	      tdc2.push_back(0);

	    }
            
	    else if(layer_list[ch]==1){
	      
	      nhit_s2++;
	      if(tdc_tmp>timezero)nhit_s2_mu++;
	      posx_s2.push_back(x[ch]);
	      posy_s2.push_back(y[ch]);
	      tdc1.push_back(0);
	      tdc2.push_back(tdc_tmp);
	      
	    }
	    

	  }//flag end

	  if(fl==1)fl=0;
	  //cout<<"check"<<endl;
	  has_unprocessed_hit = 1;
	  break;
	  
	  
	case 0x5c://event end
	  {
	    double t;
	    Long64_t v = item[i].GetVal();
	    Double_t dummy;
	    Long64_t vv = (((Long64_t)ntohl(v & 0xFFFFFFFF)) << 32) | ntohl(v >> 32);
	    Int_t count_40MHz = (vv & 0x7FF);
	    Int_t count_32kHz = ((vv >> 11) & 0x7FFF);
	    Int_t count_1Hz = (vv >> 26) & 0xFFFFFFFF;
	    t = (Double_t)(0x40000000LL+count_1Hz)+modf((Double_t)count_32kHz *(1.0 / 32000.0), &dummy)+modf((Double_t)count_40MHz * (1.0 / 40000000.0), &dummy);
	    UNIXtime= (Int_t) t;

	    //cluster hitmap check

	    for(int i=0; i!=posx.size(); ++i){

	      if(layer[i]==0)continue;

	      for(int l=0; l!=posx.size(); ++l){

		if(layer[l]==1)continue;
		if(TMath::Abs(tdc[i]-tdc[l])>40)continue;
		if( TMath::Abs(posy[i]-posy[l])>5 || TMath::Abs(posx[i]-posx[l])>5)continue;

		mapx.push_back( posx[l]-posx[i] );
		mapy.push_back( posy[l]-posy[i] );
		if(posy[i]>11)mapfl.push_back(1);
		else{mapfl.push_back(0);}

	      }


	    }


	    //cluster window check
	
	    //double rate=0;

	    int checkfl=0;

	    for(int i=0; i!=posx.size(); ++i){

	    

	      //if(posx[i]<2 || posx[i]>22 || posy[i]<2 || posy[i]>22 )continue;
	      for(int k=i+1; k != posx.size(); ++k){
		
		if( layer[i] != layer[k] )continue;
		if( TMath::Abs(tdc[i]-tdc[k]) > 150)continue;
		if( TMath::Abs(posy[i]-posy[k])>5 || TMath::Abs(posx[i]-posx[k])>5)continue;
		
		posxdiftmp=( posx[i]-posx[k] );
		posydiftmp=( posy[i]-posy[k] );

		//if( TMath::Sqrt( posxdiftmp*posxdiftmp + posydiftmp*posydiftmp )>2)continue;

		//sizetmp++;

		posxnorm.push_back( posx[i] );
		posynorm.push_back( posy[i] );
		
		hitdiftmp=( tdc[i]-tdc[k] );
		posxdif.push_back( posxdiftmp );
		posydif.push_back( posydiftmp );
		hitdif.push_back( hitdiftmp );
		
	      }
	      
	      checkfl=0;
	      sizetmp=0;

	    }

	    
	    //event increment and good event check

	    evt++;
	    if(nhit_s1>goodevt_thres&&nhit_s2>goodevt_thres){
	      good_evt++;
	      good_evt_flag=1;
	    }

	    
	    //

	    //fill tree

	    tree->Fill();
	    
	    /////
	    
	    //initiallization must be added here
	    
	    good_evt_flag=0;
	    nhit[0]=0;
	    nhit[1]=0;
	    nhit_s1=0;
	    nhit_s2=0;
	    nhit_s1_mu=0;
	    nhit_s2_mu=0;

	    time1=0;
	    time2=0;
	    timeflag=0;
	    td_tmp=0;
	    td_tmpb=0;
	    time1b=0;
	    time2b=0;
	    timeflagb=0;

	    //clear and shrink
	    layer.clear();	    layer.shrink_to_fit();
	    channel.clear();	    channel.shrink_to_fit();

	    posx.clear();	    posx.shrink_to_fit();
	    posy.clear();	    posy.shrink_to_fit();
	    posx_s1.clear();	    posx_s1.shrink_to_fit();
	    posy_s1.clear();	    posy_s1.shrink_to_fit();
	    posx_s2.clear();	    posx_s2.shrink_to_fit();
	    posy_s2.clear();	    posy_s2.shrink_to_fit();

	    tdc.clear();	    tdc.shrink_to_fit();
	    tdc1.clear();	    tdc1.shrink_to_fit();
	    tdc2.clear();	    tdc2.shrink_to_fit();
  	    tdiff.clear();  	    tdiff.shrink_to_fit();

	    posxdif.clear();	    posxdif.shrink_to_fit();
	    posydif.clear();	    posydif.shrink_to_fit();
	    hitdif.clear();	    hitdif.shrink_to_fit();

	    posxnorm.clear();	    posxnorm.shrink_to_fit();
	    posynorm.clear();	    posynorm.shrink_to_fit();

	    mapx.clear();	    mapx.shrink_to_fit();
	    mapy.clear();	    mapy.shrink_to_fit();
	    mapfl.clear();	    mapfl.shrink_to_fit();

	    /////////
	    
	    if ( nevent != 0 && evt > nevent ) break;
	    if ( evt % 1000 == 0 ) cout << evt << " events analyzed" << endl;
	    break;
	  }
	case 0x20:
	  break;
	  
	default:
	  continue;
	}
      }
	
      if ( nevent != 0 && evt > nevent ) break;
      if (nitem < nbuf) break;
    }

    tree->Write();
    cout << "Analyzed Events " << evt << ", " << good_evt << " Good Events" << endl;
    fclose(fpin);
    fpout->cd();
    fpout->Close();
    return 0;
}
