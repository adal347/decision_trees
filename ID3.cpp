#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <sstream>

#include <algorithm>
#include <map>
#include <vector>
#include <utility>

using namespace std;

typedef pair<int,map<string, int> > AttributeInnerMap;

struct node
{

	struct node* parent;
	std::vector<struct node*> childrens;
	string name;
	// 0:attribute ; 1:value; 2:answer;
	int type;
	bool answer;

};

double Entropy(AttributeInnerMap Set){

	double sum = 0.0;

	for(std::map<string, int>::iterator ite = Set.second.begin();ite != Set.second.end(); ite++){
		if(ite->second > 0){
			sum -= (double) ite->second / Set.first * log((double) ite->second / Set.first);
		}
	}
	return sum;
}

double InfoGain(AttributeInnerMap Set,vector<AttributeInnerMap> SubSetV) {

	double acum = Entropy(Set);

	for(int i = 0; i < SubSetV.size();i++){
		acum = ((double)SubSetV[i].first / Set.first) * Entropy(SubSetV[i]);
	}
	return acum;
}

bool DoNeedSplit(vector<string> index, map<string, vector<string> > dictionary, vector<vector<string> > samples) {

	AttributeInnerMap Set;
	int answer_index = index.size()-1;
	Set.first=samples.size();

	for (int i = 0; i < dictionary[index[answer_index]].size(); i++) {
		int count = 0;
		for(int j = 0; j < samples.size(); j++) {
			if(samples[j][answer_index] == dictionary[index[answer_index]][i]) {
				count++;
			}
		}
		Set.second[dictionary[index[answer_index]][i]] = count;
	}

	if(Entropy(Set) != 0.0) {
		return true;
	} else {
		return false;
	}
}

int ToSplit(vector<string> index, map<string, vector<string> > dictionary, vector<vector<string> > samples) {

	int instances = samples.size();
	vector<AttributeInnerMap> SubSetV;
	AttributeInnerMap Set, aux;
	Set.first = instances;
	int answer_index = index.size()-1;
	double max_gain = -1.0;
	int index_int = -1;

	for (int i = 0; i < dictionary[index[answer_index]].size(); i++) {
		int count=0;
		for(int j = 0; j < samples.size(); j++) {
			if(samples[j][answer_index] == dictionary[index[answer_index]][i]){
				count++;
			}
		}
		Set.second[dictionary[index[answer_index]][i]] = count;
	}

	for(int i = 0; i < index.size()-1; i++) {
		for (int j = 0; j < dictionary[index[i]].size(); j++) {
			int inst = 0;
			for (int k = 0; k < dictionary[index[answer_index]].size(); k++) {
				int count = 0;
				for (int n = 0; n < samples.size(); n++) {
					if(samples[n][answer_index] == dictionary[index[answer_index]][k] && samples[n][i] == dictionary[index[i]][j]) {
						count++;
					}
				}
				aux.second[dictionary[index[answer_index]][k]] = count;
				inst += count;
			}
			aux.first = inst;
			SubSetV.push_back(aux);
			aux.second.clear();
		}
		double gainN = InfoGain(Set, SubSetV);
		SubSetV.clear();
		if(gainN > max_gain) {
			max_gain = gainN;
			index_int = i;
		}
	}
	return index_int;
}

void Splitting(struct node* Node,vector<string> index, map<string, vector<string> > dictionary, vector<vector<string> > samples, string level) {

	if(DoNeedSplit(index, dictionary, samples)) {
		int index_split = ToSplit(index, dictionary, samples);
		vector<string> copy_index;
		vector<vector<string> > branch_samples;
		Node->name = index[index_split];
		// Attribute
		Node->type = 0;
		Node->answer = false;
		for (int i = 0; i < dictionary[index[index_split]].size(); i++) {
			struct node* child = new node;
			// Val
			child->type = 1;
			child->parent = Node;
			child->name = dictionary[index[index_split]][i];
			child->answer = false;
			cout << level << Node->name << ": " << child->name << endl;
			Node->childrens.push_back(child);

			copy_index = index;

			copy_index.erase(copy_index.begin()+(index_split));
			for(int j = 0;j < samples.size(); j++) {
				if(samples[j][index_split] == dictionary[index[index_split]][i]) {
					branch_samples.push_back(samples[j]);
				}
			}
			for(int j = 0; j < branch_samples.size() ; j++) {
				branch_samples[j].erase(branch_samples[j].begin()+index_split);
			}

			Splitting(child, copy_index, dictionary, branch_samples, "  ");
			copy_index.clear();
			branch_samples.clear();
		}
	} else {
		int index_answer = index.size() - 1;
		struct node* child = new node;
		// Answer
		child->type = 2;
		child->parent = Node;
		child->name = samples[0][index_answer];
		child->answer = true;
		cout << level << "  ANSWER: " << child->name << endl;
		Node->childrens.push_back(child);
	}
}


class TreeID3 {

	private:
		struct node* root;
		vector<string> samplesT;
		vector<string> index;
		map<string, vector<string> > dictionary;
		string action;
		vector<string> action_values;
		vector<vector<string> > Samples;

	public:

		TreeID3(vector<string> samples, vector<string> index_attribute, map<string,vector<string> > dictionary_map): samplesT(samples), index(index_attribute), dictionary(dictionary_map) {
			root = new node;
			root->parent = NULL;
			root->answer = false;
			root->type = 0;

			for(int i = 0; i < samplesT.size(); i++) {
				for(int j = 0; j < samplesT[i].length(); j++) {
					if(samplesT[i][j] == ',') {
						samplesT[i][j] = ' ';
					}
				}
				stringstream extract(samplesT[i]);
				string aux;
				vector<string> auxiliar;
				while(extract >> aux){
					auxiliar.push_back(aux);
				}
				Samples.push_back(auxiliar);
				auxiliar.clear();
			}
			action = index[index.size()-1];
			for(int i = 0; i<dictionary[action].size();i++){
				action_values.push_back(dictionary[action][i]);
			}
		}

	void CreateTree() {
		Splitting(root, index, dictionary, Samples, "");
	}
};

int main(int argc, char *argv[]) {

	string buffer, recov, eater,aux;
  char devourer;
  char rm_chars[]="{}";

  std::map<string, vector<string> > dictionary_map;
  std::vector<string> index_attribute;
  std::vector<string> samples_trainings;
  std::vector<string> aux_vector_string;

  while(cin >> devourer) {
		if(devourer == '%') {
			getline(cin, eater);
    } else {
			break;
    }
  }

	//eats @relation line
	getline(cin,eater);
	//eats blank line
  getline(cin,eater);

	getline(cin,buffer);
	while(buffer.compare("") != 0) {
		stringstream extract(buffer);
	  extract >> recov;
	  if(recov == "@attribute") {
			extract >> recov;
	    aux = recov;
	    index_attribute.push_back(aux);
	    buffer = "";
	    while(extract >> recov){
	    	buffer += recov;
	    }
	    for (int i = 0; i < strlen(rm_chars); i++) {
    		buffer.erase (std::remove(buffer.begin(), buffer.end(), rm_chars[i]), buffer.end());
	    }

			for(int i = 0; i < buffer.length(); i++){
				if(buffer[i]==',') {
					buffer[i]=' ';
				}
			}

			stringstream extract(buffer);

			while(extract >> recov){
				aux_vector_string.push_back(recov);
	    }

			dictionary_map[aux]=aux_vector_string;

			while(!aux_vector_string.empty()) {
				aux_vector_string.pop_back();
			}
		}
		getline(cin, buffer);
	}

	while(getline(cin, buffer)) {
		stringstream extract(buffer);
		extract >> recov;
		if(recov == "@data") {
			while(getline(cin, buffer)) {
				if (buffer.find('%') != std::string::npos){
					continue;
				} else {
					samples_trainings.push_back(buffer);
				}
			}
		}
	}
	TreeID3 tree(samples_trainings, index_attribute, dictionary_map);

	tree.CreateTree();
}
