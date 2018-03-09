#include "engine.h"

Engine::Engine(vector<Input*> In_,vector<Lattice*> Lat_,vector<Segment*> Seg_,vector<Molecule*> Mol_,vector<System*> Sys_,vector<Newton*> New_, string name_) {
	In=In_; name=name_;   Lat=Lat_; Mol=Mol_; Seg = Seg_; Sys=Sys_; New=New_; 
	KEYS.push_back("brand");  
}
Engine::~Engine() {
}

void Engine::AllocateMemory() {
	if (debug) cout <<"nothing to allocate in Engine" << endl; 
}


void Engine::PutParameter(string new_param) {
	KEYS.push_back(new_param); 
}

bool Engine::CheckInput(int start) {
if (debug) cout <<"Check Engine" << endl;
	bool success=true;
//right now all the checks for engine are done in input.cpp. could be move back here.
	success=In[0]->CheckParameters("engine",name,start,KEYS,PARAMETERS,VALUES);
	if (success) {
		vector<string> options;
		options.push_back("mesodyn"); options.push_back("MC"); options.push_back("MD"); 
		if (GetValue("brand").size()>0) {
                   if (!In[0]->Get_string(GetValue("brand"),brand,options,"In engine " + name + " value of brand " + brand + " is not recognised"));
		} ;
		if (brand=="mesodyn") {
			Mdyn.push_back(new Mesodyn(In,Lat,Seg,Mol,Sys,New,In[0]->MesodynList[0])); success=Mdyn[0]->CheckInput(start);
		}

	}
	return success; 
}
 
string Engine::GetValue(string parameter){
	int i=0;
	int length = PARAMETERS.size();
	while (i<length) {
		if (parameter==PARAMETERS[i]) {
			return VALUES[i]; 
		}
		i++;
	}
	return "" ; 
}

void Engine::push(string s, Real X) {
	Reals.push_back(s);
	Reals_value.push_back(X); 
}
void Engine::push(string s, int X) {
	ints.push_back(s);
	ints_value.push_back(X); 
}
void Engine::push(string s, bool X) {
	bools.push_back(s);
	bools_value.push_back(X); 
}
void Engine::push(string s, string X) {
	strings.push_back(s);
	strings_value.push_back(X); 	
}
void Engine::PushOutput() {
	strings.clear();
	strings_value.clear();
	bools.clear();
	bools_value.clear();
	Reals.clear();
	Reals_value.clear();
	ints.clear();
	ints_value.clear();  
}
Real* Engine::GetPointer(string s) {
	//vector<string> sub;
	//nothing yet
	return NULL;
}

int Engine::GetValue(string prop,int &int_result,Real &Real_result,string &string_result){
	int i=0;
	int length = ints.size();
	while (i<length) {
		if (prop==ints[i]) { 
			int_result=ints_value[i];
			return 1;
		}
		i++;
	}
	i=0;
	length = Reals.size();
	while (i<length) {
		if (prop==Reals[i]) { 
			Real_result=Reals_value[i];
			return 2;
		}
		i++;
	}
	i=0;
	length = bools.size();
	while (i<length) {
		if (prop==bools[i]) { 
			if (bools_value[i]) string_result="true"; else string_result="false"; 
			return 3;
		}
		i++;
	}
	i=0;
	length = strings.size();
	while (i<length) {
		if (prop==strings[i]) { 
			string_result=strings_value[i]; 
			return 3;
		}
		i++;
	}
	return 0; 
}
