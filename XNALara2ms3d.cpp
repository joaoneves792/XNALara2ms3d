
#include <iostream>


#include "MS3DFile.h"


int main(int argc, char** argv){
	if(argc < 3){
		std::cout << "Usage: ./XNALara2ms3d <input.mesh> <destination.ms3d>";
	}
	
	const char* fileToLoad = argv[1];
	const char* fileToSave = argv[2];


	std::cout << "Loading file: " << fileToLoad << " ...\n";
	CMS3DFile* model = new CMS3DFile();
	
	if(!model->LoadXNAFromFile(fileToLoad)){
		std::cout << "Failed to load the XNALara model from the file!\n";
		return 1;
	}

	std::cout << "-----------------------------\n";
	std::cout << "Loaded model has:\n";
	std::cout << "Groups: " << model->GetNumGroups() << "\n";
	std::cout << "-----------------------------\n";

	std::cout << "Saving to file: " << fileToSave << " ...\n";
	if(!model->SaveToFile(fileToSave)){
		std::cout << "Failed to save the ms3d model to the output file!\n";
		return 2;
	}

	model->Clear();
	delete model;
	std::cout << "Done!\n";
	return 0;
}
