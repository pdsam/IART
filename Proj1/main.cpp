#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Image{
    int orientation; // H = 0 ; V = 1
    int id;
    int numberTags;
    vector<string> tags;

};


vector<Image> loadInput(unsigned int input){
    ifstream file;
    string line;
    vector<Image> photos;

    static char const *files[] = {
	"input/a_example.txt",
	"input/b_lovely_landscapes.txt",
	"input/c_memorable_moments.txt",
	"input/d_pet_pictures.txt",
	"input/e_shiny_selfies.txt"
    };

    if (input > sizeof(files)/sizeof(files[0])){
	    return photos;
    }

    return photos;
}



int main(){

	return 0;

}
