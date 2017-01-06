#define MAINxH
#include "tools.h" 
#include "namics.h"
#include "input.h"
#include "lattice.h"
#include "segment.h"
#include "alias.h"
#include "molecule.h"
#include "system.h"
#include "newton.h"
#include "engine.h"
#include "output.h"

string version="0.0.0.0.0.0.0.0";
//meaning:
// engine version number =0
// newton version number =0
// system version number =0
// lattice version number =0
// molecule version number =0
// segment version number =0
// alias version number =0
// output version number =0

double* BlasResult=(double*)AllOnDev(1);
double e=1.60217e-19;
double T=298.15;
double k_B=1.38065e-23;
double k_BT=k_B*T;
double eps0=8.85418e-12;
double eps=80;
bool debug =true;
//double factor=e*e/(eps*eps0*b_length*k_BT); 

int main(int argc, char *argv[]) {
	string fname;
	string filename;
	string line;
	ofstream out_file;
	bool cuda; 
	int start=0;
	int n_starts=0;

#ifdef CUDA
	GPU_present();
	cuda=true;
#else 
	cuda=false; 
#endif
	vector<Input*> In; 
	vector<Lattice*> Lat; 
	vector<Segment*> Seg; 
	vector<Alias*> Al;
	vector<Molecule*> Mol;
	vector<System*> Sys; 
	vector<Newton*> New; 
	vector<Engine*> Eng;
 	vector<Output*> Out;

	if (argc == 2) fname = argv[1]; else {printf("Use: namics filename -without extension- \n"); return 1;}
	filename = fname + ".in";
	In.push_back(new Input(filename)); if (In[0]->Input_error) {return 0;}
 	n_starts=In[0]->GetNumStarts(); if (n_starts==0) n_starts++; 
	while (start<n_starts) { start++;
		cout <<"Problem nr " << start << " out of " << n_starts << endl; 
		
		Lat.push_back(new Lattice(In,In[0]->LatList[0])); if (!Lat[0]->CheckInput(start)) {return 0;}

		int n_seg=In[0]->MonList.size();  	
		for (int i=0; i<n_seg; i++) Seg.push_back(new Segment(In,Lat,In[0]->MonList[i],i,n_seg));
		for (int i=0; i<n_seg; i++) { 
			for (int k=0; k<n_seg; k++) Seg[i]->PutChiKEY(Seg[k]->name); 
			if (!Seg[i]->CheckInput(start)) return 0;
		}
		int n_al = In[0]->AliasList.size(); 
		for (int i=0; i<n_al; i++) {
			Al.push_back(new Alias(In,In[0]->AliasList[i])); 
			if (!Al[i]->CheckInput(start)) return 0;
		} 

		int n_mol = In[0]->MolList.size();  
		for (int i=0; i<n_mol; i++) {Mol.push_back(new Molecule(In,Lat,Seg,Al,In[0]->MolList[i])); if (!Mol[i]->CheckInput(start)) return 0;}

		Sys.push_back(new System(In,Lat,Seg,Mol,In[0]->SysList[0])); Sys[0]->cuda=cuda;  
		if (!Sys[0]->CheckInput(start)) {return 0;} if (!Sys[0]->CheckChi_values(n_seg)) return 0; 

		New.push_back(new Newton(In,Lat,Seg,Mol,Sys,In[0]->NewtonList[0])); if (!New[0]->CheckInput(start)) {return 0;}

		Eng.push_back(new Engine(In,Sys,New,In[0]->EngineList[0])); if (!Eng[0]->CheckInput(start)) {return 0;} 

		int n_out = In[0]->OutputList.size(); 
		if (n_out ==0) cout <<"Warning: no output defined" << endl; 
		
		for (int i=0; i<n_out; i++) { 
			Out.push_back(new Output(In,Lat,Seg,Mol,Sys,New,Al,Eng,In[0]->OutputList[i],i,n_out)); 
			if (!Out[i]->CheckInput(start)) {cout << "input_error in output " << endl; return 0;} 
		}
 		Eng[0]->Doit(); 
		Lat[0]->PushOutput();
		New[0]->PushOutput();
		Eng[0]->PushOutput();
		int length = In[0]->MonList.size();
		for (int i=0; i<length; i++) Seg[i]->PushOutput();
		length = In[0]->MolList.size();
		for (int i=0; i<length; i++) Mol[i]->PushOutput();
		length = In[0]->AliasList.size();
		for (int i=0; i<length; i++) Al[i]->PushOutput();
		Sys[0]->PushOutput(); //needs to be after pushing output for seg.

		for (int i=0; i<n_out; i++) Out[i]->WriteOutput(); 
		for (int i=0; i<n_out; i++) delete Out[i]; Out.clear();
		delete Eng[0]; Eng.clear(); 
		delete New[0]; New.clear();
		delete Sys[0]; Sys.clear();
 		for (int i=0; i<n_mol; i++) delete Mol[i]; Mol.clear();
		for (int i=0; i<n_al; i++)  delete Al[i]; Al.clear();
		for (int i=0; i<n_seg; i++) delete Seg[i]; Seg.clear();	
		delete Lat[0]; Lat.clear();
	}	
	
	return 0;
}

